/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	yalr.c
Usage:	yalr (yet another language recogition) is a current experimental purpose
		project, serving a better, not too overloaded parser generator
		supporting character-level parsing with bottom-up and top-down parsing
		facilities. Its a jam of several attemps into one project.

		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define NAMELEN			80
#define MALLOCSTEP		255

typedef enum
{
	YASYMTYPE_NONTERM,
	YASYMTYPE_CCL,
	YASYMTYPE_STRING,
	YASYMTYPE_SPECIAL
} yasymtype;

typedef struct _yasym	yasym;
typedef struct _yaprod	yaprod;
typedef struct _yagram	yagram;

#define YAFLAG_CALLED	1
#define YAFLAG_DEFINED	2
#define YAFLAG_NULLABLE	4
#define YAFLAG_LEFTREC	8

struct _yaprod
{
	int			id;
	yasym*		lhs;
	plist*		rhs;

	int			flags;

	char*		strval;
};

struct _yasym
{
	int			id;
	yasymtype	type;

	char*		name;
	pboolean	emit;

	int			flags;

	plist*		first;

	pccl*		ccl;
	plist*		productions;
};

struct _yagram
{
	plist*		symbols;
	plist*		productions;

	plist*		states;

	yasym*		ws;
	yasym*		goal;
	yasym*		eof;
};

/* Closure item */
typedef struct
{
	yaprod*			prod;			/* Production */
	int				dot;			/* Dot offset */
	plist*			lookahead;		/* Lookahead symbols */
} yalritem;

/* LR-State */
typedef struct
{
	plist*			kernel;			/* Kernel items */
	plist*			epsilon;		/* Empty items */

	plist*			actions;		/* Action row entries */
	plist*			gotos;			/* Goto row entries */

	yaprod*			def_prod;		/* Default production */

	pboolean		done;			/* Done flag */
	pboolean		closed;			/* Closed flag */
} yalrstate;

/* LR-Transition */
typedef struct
{
	yasym*			symbol;			/* Symbol */

	yalrstate*		shift;			/* Shift to state */
	yaprod*			reduce;			/* Reduce by production */
} yalrcolumn;


typedef struct
{
	enum
	{
		YAMATCH_BEGIN,
		YAMATCH_END
	} 			type;

	yasym*		sym;
	yaprod*		prod;

	char*		start;
	char*		end;

	int			line;
	int			col;
} yamatch;


yasym* ya_sym_create( yagram* g, yasymtype type, char* name, char* def )
{
	yasym*	sym;

	if( !g )
	{
		WRONGPARAM;
		return (yasym*)NULL;
	}

	name = pstrdup( name );

	if( !( sym = (yasym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		fprintf( stderr, "Symbol '%s' already exists\n", name );
		pfree( name );
		return (yasym*)NULL;
	}

	switch( ( sym->type = type ) )
	{
		case YASYMTYPE_NONTERM:
			sym->productions = plist_create( 0, PLIST_MOD_PTR  );
			break;

		case YASYMTYPE_CCL:
			sym->ccl = p_ccl_create( 0, 255, def );
			name = pstrdup( p_ccl_to_str( sym->ccl, TRUE ) );
			break;

		case YASYMTYPE_STRING:
			name = pstrdup( def );
			break;

		default:
			break;
	}

	sym->id = plist_count( g->symbols ) - 1;
	sym->name = name;
	sym->first = plist_create( 0, PLIST_MOD_PTR );

	if( sym->type != YASYMTYPE_NONTERM )
		plist_push( sym->first, sym );

	return sym;
}

pboolean ya_prod_append( yaprod* p, yasym* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	plist_push( p->rhs, sym );
	return TRUE;
}

yaprod* ya_prod_create( yagram* g, yasym* lhs, ... )
{
	yaprod*	prod;
	yasym*	sym;
	va_list	varg;

	if( !( g && lhs ) )
	{
		WRONGPARAM;
		return (yaprod*)NULL;
	}

	prod = (yaprod*)plist_malloc( g->productions );

	prod->id = plist_count( g->productions ) - 1;
	prod->lhs = lhs;
	prod->rhs = plist_create( 0, PLIST_MOD_PTR );

	plist_push( lhs->productions, prod );

	va_start( varg, lhs );

	while( ( sym = va_arg( varg, yasym* ) ) )
		ya_prod_append( prod, sym );

	va_end( varg );

	return prod;
}

yasym* ya_prod_getfromrhs( yaprod* p, int off )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (yasym*)NULL;
	}

	return (yasym*)plist_access( plist_get( p->rhs, off ) );
}

char* ya_prod_to_str( yaprod* p )
{
	plistel*	e;
	yasym*		sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	p->strval = (char*)pfree( p->strval );

	if( p->lhs )
		p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );

	p->strval = pstrcatstr( p->strval, " : ", FALSE );

	plist_for( p->rhs, e )
	{
		sym = (yasym*)plist_access( e );

		if( e != plist_first( p->rhs ) )
			p->strval = pstrcatstr( p->strval, " ", FALSE );

		if( sym->type != YASYMTYPE_NONTERM )
			p->strval = pstrcatstr( p->strval, "@", FALSE );

		p->strval = pstrcatstr( p->strval, sym->name, FALSE );
	}

	return p->strval;
}

/* Parse BNF */

#define PARSEERROR( def, expect )  \
	fprintf( stderr, "Parse error at >%.30s< %s%s%s\n", \
		*def, ( pstrlen( *def ) > 30 ) ? "..." : "", \
			expect ? ", expecting " : "", expect ? expect : "" )

#define SKIPWHITE()  \
	while( isspace( **def ) ) (*def)++

static pboolean read_char( char** def, char ch )
{
	char	chstr	[3+1];

	if( **def != ch )
	{
		sprintf( chstr, "'%c'", ch );
		PARSEERROR( def, chstr );
		return FALSE;
	}

	(*def)++;
	return TRUE;
}

static pboolean parse_alternatives( yagram* g, yasym* lhs, char** def );

static char* derive_name( yagram* g, char* base )
{
	int		i;
	static
	char	deriv	[ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s'", base );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !plist_get_by_key( g->symbols, deriv ) )
			return deriv;

		strcat( deriv, "'" );
	}

	return (char*)NULL;
}

static pboolean parse_factor( yagram* g, yasym* lhs, yaprod* p, char** def )
{
	yasym*		sym		= (yasym*)NULL;
	yasym*		mod;
	char*		start;
	char*		end;
	char		name	[ NAMELEN + 1 ];
	char		stopch;
	int			i;

	SKIPWHITE();

	if( isalpha( **def ) )
	{
		start = end = *def;

		while( ( isalnum( *end ) || *end == '_' )
				&& ( end - start < NAMELEN ) )
			end++;

		sprintf( name, "%.*s", end - start, start );
		*def = end;

		if( !( sym = (yasym*)plist_access(
								plist_get_by_key( g->symbols, name ) ) ) )
			sym = ya_sym_create( g, YASYMTYPE_NONTERM, name, (char*)NULL );

		sym->flags |= YAFLAG_CALLED;
	}
	else if( **def == '(' )
	{
		(*def)++;

		sym = ya_sym_create( g, YASYMTYPE_NONTERM,
					derive_name( g, lhs->name ), (char*)NULL );

		if( !parse_alternatives( g, sym, def ) )
			return FALSE;

		if( !read_char( def, ')' ) )
			return FALSE;
	}
	else if( **def == '\"' || **def == '\'' )
	{
		stopch = **def;
		start = end = ++(*def);

		while( *end != stopch && ( end - start < NAMELEN ) )
		{
			if( *end == '\\' )
				end++;

			end++;
		}

		*def = end + 1;
		sprintf( name, "%.*s", end - start, start );

		if( stopch == '\"' )
			sym = ya_sym_create( g, YASYMTYPE_STRING, (char*)NULL, name );
		else
			sym = ya_sym_create( g, YASYMTYPE_CCL, (char*)NULL, name );
	}
	else
	{
		return TRUE;
	}

	SKIPWHITE();
	if( **def == '*' || **def == '+' || **def == '?' )
	{
		if( **def == '*' || **def == '+' )
		{
			mod = ya_sym_create( g, YASYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

			ya_prod_create( g, mod, mod, sym, (yasym*)NULL );
			ya_prod_create( g, mod, sym, (yasym*)NULL );

			sym = mod;
		}

		if( **def == '?' || **def == '*' )
		{
			mod = ya_sym_create( g, YASYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

			ya_prod_create( g, mod, sym, (yasym*)NULL );
			ya_prod_create( g, mod, (yasym*)NULL );

			sym = mod;
		}

		(*def)++;
	}

	plist_push( p->rhs, sym );
	return TRUE;
}

static pboolean parse_sequence( yagram* g, yasym* lhs, yaprod* p, char** def )
{
	pboolean	err;

	while( ( err = parse_factor( g, lhs, p, def ) ) )
	{
		SKIPWHITE();
		if( **def == ';' || **def == '|' || **def == ')' )
			break;
	}

	return err;
}

static pboolean parse_alternatives( yagram* g, yasym* lhs, char** def )
{
	yaprod*		p;

	while( TRUE )
	{
		p = ya_prod_create( g, lhs, (yasym*)NULL );

		if( !parse_sequence( g, lhs, p, def ) )
			return FALSE;

		if( **def != '|' )
			break;
		(*def)++;
	}

	return TRUE;
}

static int ya_gram_read( yagram* g, char** def )
{
	char*		start;
	char*		end;
	yasym*		nonterm;
	int			defcount	= 0;
	char		name		[ NAMELEN + 1 ];

	while( *def )
	{
		SKIPWHITE();

		start = end = *def;

		if( ! *start )
			break;

		if( isalpha( *start ) || *start == '_' )
		{
			while( ( isalnum( *end ) || *end == '_' )
					&& end - start < NAMELEN )
				end++;

			*def = end;
			sprintf( name, "%.*s", end - start, start );

			if( !( nonterm = (yasym*)plist_access(
										plist_get_by_key(
											g->symbols, name ) ) ) )
				nonterm = ya_sym_create( g, YASYMTYPE_NONTERM,
											name, (char*)NULL );

			nonterm->flags |= YAFLAG_DEFINED;

			if( !g->goal )
			{
				g->goal = nonterm;
				nonterm->flags |= YAFLAG_CALLED;
			}

			SKIPWHITE();
			if( !read_char( def, ':' ) )
				return -1;

			if( !parse_alternatives( g, nonterm, def ) )
				return -1;

			if( !read_char( def, ';' ) )
				return -1;

			defcount++;
		}
		else
		{
			PARSEERROR( def, "nonterminal definition" );
			return -1;
		}
	}

	return defcount;
}

yagram* ya_gram_free( yagram* g )
{
	plistel*	e;
	yasym*		sym;
	yaprod*		prod;

	if( !g )
		return (yagram*)NULL;

	plist_for( g->symbols, e )
	{
		sym = (yasym*)plist_access( e );
		plist_free( sym->productions );
	}

	plist_for( g->productions, e )
	{
	}

	return (yagram*)NULL;
}

yagram* ya_gram_create( char* def )
{
	plistel*	e;
	plistel*	er;
	plistel*	ep;
	yaprod*		p;
	yaprod*		cp;
	yasym*		s;
	pboolean	nullable;
	plist*		call;
	plist*		done;
	int			cnt			= 0;
	int			pcnt;
	yagram*		g;

	g = (yagram*)pmalloc( sizeof( yagram ) );

	g->symbols = plist_create( sizeof( yasym ),
					PLIST_MOD_EXTKEYS | PLIST_MOD_UNIQUE );
	g->productions = plist_create( sizeof( yaprod ), PLIST_MOD_NONE );
	g->states = plist_create( sizeof( yalrstate ), PLIST_MOD_RECYCLE );

	g->eof = ya_sym_create( g, YASYMTYPE_SPECIAL, "eof", (char*)NULL );

	if( def && *def && ya_gram_read( g, &def ) <= 0 )
		return ya_gram_free( g );

	/* Look for unique goal sequence */
	if( plist_count( g->goal->productions ) > 1 )
	{
		s = ya_sym_create( g, YASYMTYPE_NONTERM,
						derive_name( g, g->goal->name ), (char*)NULL );

		ya_prod_create( g, s, g->goal, (yasym*)NULL );
		g->goal = s;
	}

	/* Compute FIRST sets and mark left-recursions */
	call = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	do
	{
		pcnt = cnt;
		cnt = 0;

		plist_for( g->productions, e )
		{
			cp = (yaprod*)plist_access( e );
			plist_push( call, cp );

			while( plist_pop( call, &p ) )
			{
				plist_push( done, p );

				plist_for( p->rhs, er )
				{
					s = (yasym*)plist_access( er );

					nullable = FALSE;

					if( s->type == YASYMTYPE_NONTERM )
					{
						/* Union first set */
						plist_union( cp->lhs->first, s->first );

						/* Put productions on stack */
						plist_for( s->productions, ep )
						{
							p = (yaprod*)plist_access( ep );

							if( plist_count( p->rhs ) == 0 )
							{
								nullable = TRUE;
								continue;
							}

							if( p == cp )
							{
								p->lhs->flags |= YAFLAG_LEFTREC;
								p->flags |= YAFLAG_LEFTREC;
							}
							else if( !plist_get_by_ptr( done, p ) )
								plist_push( call, p );
						}
					}
					/* Extend first set if required */
					else if( !plist_get_by_ptr( cp->lhs->first, s ) )
						plist_push( cp->lhs->first, s );

					if( !nullable )
						break;
				}

				/* Flag nullable */
				if( !er )
				{
					cp->flags |= YAFLAG_NULLABLE;
					cp->lhs->flags |= YAFLAG_NULLABLE;
				}

				cnt += plist_count( cp->lhs->first );
			}

			plist_erase( done );
		}
	}
	while( pcnt < cnt );

	plist_free( call );
	plist_free( done );

	return g;
}

void ya_sym_print( yasym* s )
{
	if( !s )
		return;

	switch( s->type )
	{
		case YASYMTYPE_NONTERM:
			printf( "%s", s->name );
			break;

		case YASYMTYPE_CCL:
			printf( "'%s'", s->name );
			break;

		case YASYMTYPE_STRING:
			printf( "\"%s\"", s->name );
			break;

		case YASYMTYPE_SPECIAL:
			printf( "@%s", s->name );
			break;

		default:
			MISSINGCASE;
			break;
	}
}

void ya_gram_print( yagram* g )
{
	plistel*	e;
	plistel*	f;

	yaprod*		p;
	yasym*		s;
	int			maxlhslen	= 0;

	plist_for( g->symbols, e )
	{
		s = (yasym*)plist_access( e );
		if( s->type == YASYMTYPE_NONTERM && pstrlen( s->name ) > maxlhslen )
			maxlhslen = pstrlen( s->name );
	}

	plist_for( g->productions, e )
	{
		p = (yaprod*)plist_access( e );
		printf( "%s%s%s   %-*s : ",
			g->goal == p->lhs ? "G" : " ",
			p->flags & YAFLAG_LEFTREC ? "L" : " ",
			p->flags & YAFLAG_NULLABLE ? "N" : " ",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (yasym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				printf( " " );

			ya_sym_print( s );
		}

		printf( "\n" );
	}

	printf( "\n" );

	plist_for( g->symbols, e )
	{
		s = (yasym*)plist_access( e );
		if( s->type != YASYMTYPE_NONTERM )
			continue;

		printf( "FIRST %-*s {", maxlhslen, s->name );
		plist_for( s->first, f )
		{
			s = (yasym*)plist_access( f );
			printf( " " );
			ya_sym_print( s );
		}

		printf( " }\n" );
	}
}

static void ya_lritem_print( yalritem* it )
{
	int			i;
	yasym*		sym;
	plistel*	e;

	if( ( !it ) )
	{
		WRONGPARAM;
		return;
	}

	ya_sym_print( it->prod->lhs );
	printf( " : " );

	for( i = 0; i < plist_count( it->prod->rhs ); i++ )
	{
		sym = ya_prod_getfromrhs( it->prod, i );

		if( i > 0 )
			printf( " " );

		if( i == it->dot )
			printf( ". " );

		ya_sym_print( sym );
	}

	if( i == it->dot )
	{
		printf( " ." );

		if( it->lookahead )
		{
			printf( "   { " );

			plist_for( it->lookahead, e )
			{
				if( e != plist_first( it->lookahead ) )
					printf( " " );

				/* printf( "(%s)", ((yasym*)plist_access( e ))->name ); */
				ya_sym_print( (yasym*)plist_access( e ) );
			}

			printf( " }" );
		}
	}

	printf( "\n" );
}

static void ya_lritems_print( plist* items, char* what )
{
	plistel*	e;

	if( ( !items ) )
		return;

	if( what && *what )
		printf( "%s (%d):\n", what, plist_count( items ) );

	if( !plist_count( items ) )
		printf( "\t(empty)\n" );

	plist_for( items, e )
	{
		if( what && *what )
			printf( "\t" );

		ya_lritem_print( (yalritem*)plist_access( e ) );
	}
}

static yalritem* ya_lritem_create( plist* list, yaprod* prod, int dot )
{
	yalritem*	item;

	if( !( list && prod ) )
	{
		WRONGPARAM;
		return (yalritem*)NULL;
	}

	if( dot < 0 )
		dot = 0;
	else if( dot > plist_count( prod->rhs ) )
		dot = plist_count( prod->rhs );

	item = (yalritem*)plist_malloc( list );
	item->prod = prod;
	item->dot = dot;
	item->lookahead = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_AUTOSORT );

	return item;
}

static yalritem* ya_lritem_free( yalritem* it )
{
	if( !( it ) )
		return (yalritem*)NULL;

	it->lookahead = plist_free( it->lookahead );

	return (yalritem*)NULL;
}

static yalrcolumn* ya_lrcolumn_create(
	yalrstate* st, yasym* sym, yalrstate* shift, yaprod* reduce )
{
	yalrcolumn*		col;

	if( sym->type != YASYMTYPE_NONTERM )
		col = (yalrcolumn*)plist_malloc( st->actions );
	else
		col = (yalrcolumn*)plist_malloc( st->gotos );

	col->symbol = sym;
	col->shift = shift;
	col->reduce = reduce;

	return col;
}

static yalrstate* ya_lrstate_create( yagram* gram, plist* kernel )
{
	plistel*	e;
	yalrstate*	state;

	if( !( gram ) )
	{
		WRONGPARAM;
		return (yalrstate*)NULL;
	}

	state = plist_malloc( gram->states );

	state->kernel = plist_create( sizeof( yalritem ), PLIST_MOD_NONE );
	state->epsilon = plist_create( sizeof( yalritem ), PLIST_MOD_NONE );

	state->actions = plist_create( sizeof( yalrcolumn ), PLIST_MOD_NONE );
	state->gotos = plist_create( sizeof( yalrcolumn ), PLIST_MOD_NONE );

	plist_for( kernel, e )
		plist_push( state->kernel, plist_access( e ) );

	return state;
}

static yalrstate* ya_lrstate_free( yalrstate* state )
{
	if( !state )
		return (yalrstate*)NULL;

	plist_free( state->kernel );
	plist_free( state->epsilon );

	plist_free( state->actions );
	plist_free( state->gotos );

	pfree( state );

	return (yalrstate*)NULL;
}

static BOOLEAN ya_parser_lr_compare( plist* set1, plist* set2 )
{
	plistel*	e;
	plistel*	f;
	yalritem*	it1;
	yalritem*	it2;
	int			same	= 0;

	if( plist_count( set1 ) == plist_count( set2 ) )
	{
		plist_for( set1, e )
		{
			it1 = (yalritem*)plist_access( e );

			plist_for( set2, f )
			{
				it2 = (yalritem*)plist_access( f );

				if( it1->prod == it2->prod && it1->dot == it2->dot )
				{
					/*
					if( para != PGPARADIGM_LR1
							|| !plist_diff( it1->lookahead, it2->lookahead ) )
					*/
					same++;
				}
			}
		}

		if( plist_count( set1 ) == same )
			return TRUE;
	}

	return FALSE;
}

static yalrstate* ya_parser_lr_get_undone( yagram* gram )
{
	yalrstate*	st;
	plistel*	e;

	plist_for( gram->states, e )
	{
		st = (yalrstate*)plist_access( e );

		if( !st->done )
			return st;
	}

	return (yalrstate*)NULL;
}

BOOLEAN ya_parser_lr_closure( yagram* gram )
{
	yalrstate*		st;
	yalrstate*		nst;
	yalritem*		it;
	yalritem*		kit;
	yalritem*		cit;
	yasym*			sym;
	yasym*			lhs;
	yaprod*			prod;
	yalrcolumn*		col;
	yalrcolumn*		ccol;
	plist*			closure;
	plist*			part;
	plistel*		e;
	plistel*		f;
	plistel*		g;
	plistel*		h;
	int				i;
	int				j;
	int				cnt;
	int				prev_cnt;
	int*			prodcnt;

	PROC( "ya_parser_lr_closure" );
	PARMS( "gram", "%p", gram );

	if( !( gram ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	MSG( "Creating a closure seed" );
	nst = ya_lrstate_create( gram, (plist*)NULL );
	it = ya_lritem_create( nst->kernel,
				(yaprod*)plist_access(
					plist_first( gram->goal->productions ) ), 0 );

	plist_push( it->lookahead, gram->eof );

	MSG( "Initializing part and closure lists" );
	part = plist_create( sizeof( yalritem ), PLIST_MOD_RECYCLE );
	closure = plist_create( sizeof( yalritem ), PLIST_MOD_RECYCLE );

	MSG( "Run the closure loop" );
	while( ( st = ya_parser_lr_get_undone( gram ) ) )
	{
		fprintf( stderr, "---\nClosing state %d\n",
					plist_offset( plist_get_by_ptr( gram->states, st ) ) );
		st->done = TRUE;

		MSG( "Closing state" );
		VARS( "State", "%d", plist_offset(
								plist_get_by_ptr( gram->states, st ) ) );

		/* Close all items of the current state */
		cnt = 0;
		plist_clear( closure );

		ya_lritems_print( st->kernel, "Kernel" );

		/* Duplicate state kernel to closure */
		MSG( "Duplicate current kernel to closure" );
		plist_for( st->kernel, e )
		{
			kit = (yalritem*)plist_access( e );

			/* Add only items that have a symbol on the right of the dot */
			if( ya_prod_getfromrhs( kit->prod, kit->dot ) )
			{
				it = ya_lritem_create( closure, kit->prod, kit->dot );
				it->lookahead = plist_dup( kit->lookahead );
			}
		}

		/* Close the closure! */
		MSG( "Performing closure" );
		do
		{
			prev_cnt = cnt;
			cnt = 0;

			/* Loop trought all items of the current state */
			plist_for( closure, e )
			{
				it = (yalritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !( lhs = ya_prod_getfromrhs( it->prod, it->dot ) )
						|| !( lhs->type == YASYMTYPE_NONTERM ) )
					continue;

				/* Add all productions of the nonterminal to the closure,
					if not already in */
				for( i = 0; ( prod = (yaprod*)plist_access(
										plist_get( lhs->productions, i ) ) );
						i++ )
				{
					plist_for( closure, f )
					{
						cit = (yalritem*)plist_access( f );
						if( cit->prod == prod && cit->dot == 0 )
							break;
					}

					if( !f )
						cit = ya_lritem_create( closure, prod, 0 );

					/* Merge lookahead */

					/*
						Find out all lookahead symbols by merging the
						FIRST-sets of all nullable and the first
						non-nullable items on the productions right-hand
						side.
					*/
					for( j = it->dot + 1;
							( sym = ya_prod_getfromrhs( it->prod, j ) );
								j++ )
					{
						plist_union( cit->lookahead, sym->first );

						if( !sym->flags & YAFLAG_NULLABLE )
							break;
					}

					/*
						If all symbols right to the dot are nullable
						(or there are simply none), then add the current
						items lookahead to the closed items lookahead.
					*/
					if( !sym )
						plist_union( cit->lookahead, it->lookahead );
				}
			}

			cnt = plist_count( closure );
		}
		while( prev_cnt != cnt );
		MSG( "Closure algorithm done" );

		/* Move all epsilon closures into state's epsilon list */
		for( e = plist_first( closure ); e; )
		{
			it = (yalritem*)plist_access( e );

			if( plist_count( it->prod->rhs ) > 0 )
				e = plist_next( e );
			else
			{
				plist_for( st->epsilon, f )
				{
					kit = (yalritem*)plist_access( f );
					if( kit->prod == it->prod )
						break;
				}

				if( !f )
					plist_push( st->epsilon, it );
				else
					plist_union( kit->lookahead, it->lookahead );

				f = e;
				e = plist_next( e );
				plist_remove( closure, f );
			}
		}

		MSG( "Closure finished!" );
		ya_lritems_print( closure, "Closure" );

		/* Create new states from the items in the closure having a symbol
			right to their dot */
		do
		{
			sym = (yasym*)NULL;
			plist_clear( part );

			for( e = plist_first( closure ); e; )
			{
				it = (yalritem*)plist_access( e );

				/* Check if symbol right to the dot is a nonterminal */
				if( !sym && !( sym = ya_prod_getfromrhs( it->prod, it->dot ) ) )
				{
					e = plist_next( e );
					continue;
				}

				/* Add item to new state kernel */
				if( sym == ya_prod_getfromrhs( it->prod, it->dot ) )
				{
					it->dot++;
					plist_push( part, it );

					f = plist_prev( e );
					plist_remove( closure, e );

					if( !( e = f ) )
						e = plist_first( closure );
				}
				else
					e = plist_next( e );
			}

			/* Stop if no more partitions found (there is no first item) */
			if( !( it = (yalritem*)plist_access( plist_first( part ) ) ) )
				break;

			/*
				Can we do a shift and reduce in one transition?

				Watch for partitions that are

					x -> y .z

				where x is nonterminal, y is a possible sequence of
				terminals and/or nonterminals, or even epsilon, and z is a
				terminal or nonterminal.
			*/
			if( 1 && ( plist_count( part ) == 1
					&& !ya_prod_getfromrhs( it->prod, it->dot ) ) )
			{
				MSG( "State optimization" );

				if( !st->closed )
					ya_lrcolumn_create( st, sym,
							(yalrstate*)it->prod, it->prod );

				/* Forget current partition
					- its not needed anymore... */
				plist_for( part, e )
					ya_lritem_free( (yalritem*)plist_access( e ) );

				continue;
			}

			MSG( "Check in state pool for same kernel configuration" );
			plist_for( gram->states, e )
			{
				nst = (yalrstate*)plist_access( e );

				if( ya_parser_lr_compare( part, nst->kernel ) )
					break;
			}

			/* State does not already exists?
				Create it as new! */
			if( !e )
			{
				MSG( "No such state, creating new state from current config" );
				ya_lritems_print( part, "NEW Kernel" );
				nst = ya_lrstate_create( gram, part );
			}
			else
			/* State already exists?
				Merge lookaheads (if needed). */
			{
				MSG( "There is a state with such configuration" );

				/* Merge lookahead */
				cnt = 0;
				prev_cnt = 0;

				for( e = plist_first( nst->kernel ),
						f = plist_first( part ); e;
							e = plist_next( e ), f = plist_next( f ) )
				{
					it = (yalritem*)plist_access( e );
					cit = (yalritem*)plist_access( f );

					prev_cnt += plist_count( it->lookahead );
					plist_union( it->lookahead, cit->lookahead );
					cnt += plist_count( it->lookahead );
				}

				if( cnt != prev_cnt )
					nst->done = FALSE;

				ya_lritems_print( st->kernel, "EXT Kernel" );
			}

			if( sym && !st->closed )
				ya_lrcolumn_create( st, sym, nst, (yaprod*)NULL );
		}
		while( TRUE );

		st->closed = TRUE;
		MSG( "State closed" );
	}

	plist_free( closure );
	plist_free( part );

	MSG( "Performing reductions" );

	prodcnt = (int*)pmalloc( plist_count( gram->productions ) * sizeof( int ) );

	plist_for( gram->states, e )
	{
		st = (yalrstate*)plist_access( e );

		for( part = st->kernel; part;
				part = ( part == st->kernel ? st->epsilon : (plist*)NULL ) )
		{
			plist_for( part, f )
			{
				it = (yalritem*)plist_access( f );

				/* Only for items which have the dot at the end */
				if( ya_prod_getfromrhs( it->prod, it->dot ) )
					continue;

				/* Put entries for each lookahead */
				plist_for( it->lookahead, g )
				{
					sym = (yasym*)plist_access( g );
					ya_lrcolumn_create( st, sym, (yalrstate*)NULL, it->prod );
				}
			}
		}

		/* Detect default productions */
		memset( prodcnt, 0, plist_count( gram->productions ) * sizeof( int ) );
		cnt = 0;

		plist_for( st->actions, f )
		{
			col = (yalrcolumn*)plist_access( f );

			if( col->reduce && prodcnt[ col->reduce->id ]++ > cnt )
			{
				cnt = prodcnt[ col->reduce->id ];
				st->def_prod = col->reduce;
			}
		}

		/* Remove all parser actions that match the default production */
		if( st->def_prod )
		{
			for( f = plist_first( st->actions ); f; )
			{
				col = (yalrcolumn*)plist_access( f );

				if( col->reduce == st->def_prod )
				{
					g = plist_next( f );
					plist_remove( st->actions, f );
					f = g;
				}
				else
					f = plist_next( f );
			}
		}
	}

	pfree( prodcnt );

	/* Detect and report conflicts */

	fprintf( stderr, "\n--== CONFLICTS ==--\n" );

	cnt = 0;
	plist_for( gram->states, e )
	{
		st = (yalrstate*)plist_access( e );

		plist_for( st->actions, f )
		{
			col = (yalrcolumn*)plist_access( f );

			for( g = plist_next( f ); g; g = plist_next( g ) )
			{
				ccol = (yalrcolumn*)plist_access( g );

				if( ccol->symbol == col->symbol )
				{
					/* TODO Conflict resolution */
					fprintf( stderr,
						"\nState %d: %s/reduce on %s",
							plist_offset( e ),
								col->shift ? "shift" : "reduce",
									sym->name );
				}
			}
		}
	}

	if( !cnt )
		fprintf( stderr, "\n(No conflicts detected)\n" );

	/* Print final states */

	fprintf( stderr, "\n--== FINAL STATES ==--\n" );

	plist_for( gram->states, e )
	{
		st = (yalrstate*)plist_access( e );
		fprintf( stderr, "\n-- State %d %p --\n",
			plist_offset( e ), plist_access( e ) );

		ya_lritems_print( st->kernel, "Kernel" );
		ya_lritems_print( st->epsilon, "Epsilon" );

		fprintf( stderr, "\n" );

		plist_for( st->actions, f )
		{
			col = (yalrcolumn*)plist_access( f );

			if( col->shift && col->reduce )
				fprintf( stderr, " <- Shift/Reduce on '%s' by "
									"production '%s'\n",
							col->symbol->name,
								ya_prod_to_str( col->reduce ) );
			else if( col->shift )
				fprintf( stderr, " -> Shift on '%s', goto state %d\n",
							col->symbol->name,
								plist_offset( plist_get_by_ptr(
									gram->states, col->shift ) ) );
			else if( col->reduce )
				fprintf( stderr, " <- Reduce on '%s' by production '%s'\n",
							col->symbol->name,
								ya_prod_to_str( col->reduce ) );
			else
				fprintf( stderr, " XX Error on '%s'\n", col->symbol->name );
		}

		if( st->def_prod )
			fprintf( stderr, " <- Reduce default on '%s'\n",
				ya_prod_to_str( st->def_prod ) );

		plist_for( st->gotos, f )
		{
			col = (yalrcolumn*)plist_access( f );

			if( col->shift && col->reduce )
				fprintf( stderr, " <- Goto/Reduce by production "
										"'%s' in '%s'\n",
							ya_prod_to_str( col->reduce ),
								col->symbol->name );
			else if( col->shift )
				fprintf( stderr, " -> Goto state %d on '%s'\n",
							plist_offset( plist_get_by_ptr(
									gram->states, col->shift ) ),
								col->symbol->name );
			else
				MISSINGCASE;
		}
	}

	MSG( "Finished" );
	RETURN( TRUE );
}

/* Top-down parser supporting left-recursion */

static pboolean is_direct_lrec( yaprod* p )
{
	int		i;
	yasym*	s;

	for( i = 0; ( s = ya_prod_getfromrhs( p, i ) ); i++ )
	{
		if( s == p->lhs )
			return TRUE;

		if( !( s->flags & YAFLAG_NULLABLE ) )
			break;
	}

	return FALSE;
}

pboolean ya_run( plist* ast, yagram* grm, yasym* sym, char* start, char** end )
{
	pboolean	myast	= FALSE;
	yasym*		rsym;
	yaprod*		p;
	char*		ptr;
	int			loop	= 0;
	int			i;
	int			j;
	pboolean	pnext;
	pboolean	rnext;
	yamatch*	match;
	yamatch*	smatch;
	yamatch*	ematch;
	plistel*	sme;
	plistel*	e;

	if( !( grm && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !ast )
	{
		ast = plist_create( sizeof( yamatch ), PLIST_MOD_RECYCLE );
		myast = TRUE;
	}

	if( !sym )
		sym = grm->goal;

	do
	{
		/*
		printf( "loop = %d\n", loop );
		*/

		smatch = (yamatch*)plist_malloc( ast );
		smatch->type = YAMATCH_BEGIN;
		sme = plist_last( ast );

		for( i = 0, pnext = TRUE;
				pnext && ( p = (yaprod*)plist_access(
									plist_get( sym->productions, i ) ) ); i++ )
		{
			/*
			printf( "Trying %s\n", ya_prod_to_str( p ) );
			*/

			if( !( ( loop == 0 && !is_direct_lrec( p ) )
						|| ( loop > 0 && is_direct_lrec( p ) ) ) )
				continue;

			for( j = 0, ptr = start, rnext = TRUE;
					rnext && ( rsym = ya_prod_getfromrhs( p, j ) ); j++ )
			{
				/*
				printf( "Testing >%s<\n", ptr );
				*/
				switch( rsym->type )
				{
					case YASYMTYPE_CCL:
						if( p_ccl_test( rsym->ccl, *ptr ) )
							ptr++;
						else
							rnext = FALSE;

						break;

					case YASYMTYPE_STRING:
						if( !strncmp( ptr, rsym->name, strlen( rsym->name ) ) )
							ptr += strlen( rsym->name );
						else
							rnext = FALSE;

						break;

					case YASYMTYPE_NONTERM:
						for( e = plist_prev( sme ); e; e = plist_prev( e ) )
						{
							match = (yamatch*)plist_access( e );
							if( match->start < ptr )
							{
								e = (plistel*)NULL;
								break;
							}

							if( match->type == YAMATCH_END
									&& match->sym == rsym )
							{
								ptr = match->end;
								break;
							}
						}

						if( !e )
							rnext = ya_run( ast, grm, rsym, ptr, &ptr );

						break;

					default:
						MISSINGCASE;
						rnext = FALSE;
						break;
				}
			}

			if( rnext )
				break;

			while( plist_next( sme ) )
				plist_remove( ast, plist_next( sme ) );
		}

		if( !( pnext && p ) )
		{
			plist_remove( ast, sme );
			break;
		}

		ematch = (yamatch*)plist_malloc( ast );
		ematch->type = YAMATCH_END;
		smatch->start = ematch->start = start;
		smatch->end = ematch->end = ptr;
		smatch->sym = ematch->sym = sym;
		smatch->prod = ematch->prod = p;

		*end = ptr;
		loop++;
	}
	while( sym->flags & YAFLAG_LEFTREC );

	if( myast )
	{
		char	gap[80+1];
		*gap = '\0';

		for( e = plist_first( ast ); e; e = plist_next( e ) )
		{
			match = (yamatch*)plist_access( e );

			if( match->type == YAMATCH_END && *gap )
				gap[ strlen( gap ) - 1 ] = '\0';

			printf( "%s%s %s >%.*s<\n",
				gap,
				match->type == YAMATCH_BEGIN ? "{" : "}",
				match->sym->name,
				match->end - match->start,
				match->start );

			if( match->type == YAMATCH_BEGIN )
				strcat( gap, " " );
		}

		plist_free( ast );
	}

	return loop ? TRUE : FALSE;
}


int main( int argc, char** argv )
{
	yagram*	g;
	char*	s;
	char*	e;

	if( argc > 1 )
	{
		g = ya_gram_create( argv[ 1 ] );
		ya_gram_print( g );
		ya_parser_lr_closure( g );

		if( argc > 2 )
		{
			s = e = argv[2];
			if( ya_run( (plist*)NULL, g, (yasym*)NULL, s, &e ) )
				printf( ">%.*s<\n", e - s, s );
			else
				printf( "FAIL\n" );
		}
	}

	return 0;
}
