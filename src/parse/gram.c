/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	gram.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define DERIVCHAR	'\''

/** Creates a new symbol of the type //type// in the grammar //g//.

	//name// is the name for nonterminal symbols, for terminal symbols it
	can be left empty.
	//def// contains the definition of the symbol in case of a terminal type.
	It will be ignored else.
*/
ppsym* pp_sym_create( ppgram* g, ppsymtype type, char* name, char* def )
{
	ppsym*	sym;

	if( !g )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	name = pstrdup( name );

	/* Insert into symbol table */
	if( !( sym = (ppsym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		fprintf( stderr, "Symbol '%s' already exists\n", name );

		pfree( name );
		return (ppsym*)NULL;
	}

	sym->id = -1;
	sym->name = name;

	switch( ( sym->type = type ) )
	{
		case PPSYMTYPE_NONTERM:
			sym->prods = plist_create( 0, PLIST_MOD_PTR  );
			break;

		case PPSYMTYPE_CCL:
			sym->ccl = p_ccl_create( 0, 255, def ); /* TODO */

			if( !sym->name )
				sym->name = pstrdup( p_ccl_to_str( sym->ccl, TRUE ) );

			break;

		case PPSYMTYPE_STRING:
			sym->str = pstrdup( def );

			if( !sym->name )
				sym->name = pstrdup( def );

			break;

		case PPSYMTYPE_REGEX:
			sym->re = pregex_create( def, 0 );
			break;

		default:
			break;
	}

	/* FIRST */
	sym->first = plist_create( 0, PLIST_MOD_PTR );

	if( sym->type != PPSYMTYPE_NONTERM )
		plist_push( sym->first, sym );

	return sym;
}

/** Get the //n//th symbol from grammar //g//.
Returns (ppsym*)NULL if no symbol was found. */
ppsym* pp_sym_get( ppgram* g, int n )
{
	if( !( g && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get( g->symbols, n ) );
}

/** Get a symbol from grammar //g// by its //name//. */
ppsym* pp_sym_get_by_name( ppgram* g, char* name )
{
	if( !( g && name && *name ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get_by_key( g->symbols, name ) );
}

/** Returns the string representation of symbol //p//.

	Nonterminals are not expanded, they are just returned as their name.
	The returned pointer is part of //sym// and can be referenced multiple
	times. It may not be freed by the caller. */
char* pp_sym_to_str( ppsym* sym )
{
	if( !sym )
	{
		WRONGPARAM;
		return "";
	}

	if( !sym->strval )
	{
		sym->strval = (char*)pmalloc(
						( strlen( sym->name ) + 2 + 1 + 1 )
							* sizeof( char ) );

		switch( sym->type )
		{
			case PPSYMTYPE_NONTERM:
				strcpy( sym->strval, sym->name );
				break;

			case PPSYMTYPE_CCL:
				sprintf( sym->strval, "'%s'", sym->name );
				break;

			case PPSYMTYPE_STRING:
				sprintf( sym->strval, "\"%s\"", sym->name );
				break;

			case PPSYMTYPE_REGEX:
				sprintf( sym->strval, "/%s/", sym->name );
				break;

			case PPSYMTYPE_SPECIAL:
				sprintf( sym->strval, "@%s", sym->name );
				break;

			default:
				MISSINGCASE;
				break;
		}
	}

	return sym->strval;
}

/** Creates a new production on left-hand-side //lhs//
	within the grammar //g//. */
ppprod* pp_prod_create( ppgram* g, ppsym* lhs, ... )
{
	ppprod*	prod;
	ppsym*	sym;
	va_list	varg;

	if( !( g && lhs ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	prod = (ppprod*)plist_malloc( g->prods );

	prod->id = -1;
	prod->lhs = lhs;
	prod->rhs = plist_create( 0, PLIST_MOD_PTR );

	plist_push( lhs->prods, prod );

	va_start( varg, lhs );

	while( ( sym = va_arg( varg, ppsym* ) ) )
		pp_prod_append( prod, sym );

	va_end( varg );

	return prod;
}

/** Get the //n//th production from grammar //g//.
Returns (ppprod*)NULL if no symbol was found. */
ppprod* pp_prod_get( ppgram* g, int n )
{
	if( !( g && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	return (ppprod*)plist_access( plist_get( g->prods, n ) );
}

/** Appends the symbol //sym// to the right-hand-side of production //p//. */
pboolean pp_prod_append( ppprod* p, ppsym* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	plist_push( p->rhs, sym );
	pfree( p->strval );

	return TRUE;
}

/** Returns the //off//s element from the right-hand-side of
	production //p//. Returns (ppsym*)NULL if the requested element does
	not exist. */
ppsym* pp_prod_getfromrhs( ppprod* p, int off )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get( p->rhs, off ) );
}

/** Returns the string representation of production //p//.

	The returned pointer is part of //p// and can be referenced multiple times.
	It may not be freed by the caller. */
char* pp_prod_to_str( ppprod* p )
{
	plistel*	e;
	ppsym*		sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	if( !p->strval )
	{
		if( p->lhs )
			p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );

		p->strval = pstrcatstr( p->strval, " : ", FALSE );

		plist_for( p->rhs, e )
		{
			sym = (ppsym*)plist_access( e );

			if( e != plist_first( p->rhs ) )
				p->strval = pstrcatstr( p->strval, " ", FALSE );

			if( sym->type != PPSYMTYPE_NONTERM )
				p->strval = pstrcatstr( p->strval, "@", FALSE );

			p->strval = pstrcatstr( p->strval, sym->name, FALSE );
		}
	}

	return p->strval;
}

/* ------------------------------- Parse BNF -------------------------------- */

#define NAMELEN			80

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

static pboolean parse_alternatives( ppgram* g, ppsym* lhs, char** def );

static char* derive_name( ppgram* g, char* base )
{
	int		i;
	static
	char	deriv	[ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !pp_sym_get_by_name( g, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

static ppsymtype parse_terminal( char* name, char** def )
{
	char		stopch;
	char*		start;
	char*		end;

	SKIPWHITE();

	if( **def == '\"' || **def == '\'' )
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
			return PPSYMTYPE_STRING;

		return PPSYMTYPE_CCL;
	}
	else if( **def == '/' )
	{
		start = end = ++(*def);

		while( *end != '/' && ( end - start < NAMELEN ) )
		{
			if( *end == '\\' )
				end++;

			end++;
		}

		*def = end + 1;
		sprintf( name, "%.*s", end - start, start );

		return PPSYMTYPE_REGEX;
	}

	/* No symbol found */
	return PPSYMTYPE_NONTERM;
}

static pboolean parse_ident( char* name, char** def )
{
	char*		start;
	char*		end;

	SKIPWHITE();

	if( isalpha( **def ) )
	{
		start = end = *def;

		while( ( isalnum( *end ) || *end == '_' )
				&& ( end - start < NAMELEN ) )
			end++;

		sprintf( name, "%.*s", end - start, start );
		*def = end;

		return TRUE;
	}

	return FALSE;
}

static pboolean parse_integer( int* val, char** def )
{
	char*		start;
	char*		end;

	SKIPWHITE();

	if( isdigit( **def ) )
	{
		start = end = *def;

		while( isdigit( *end ) )
			end++;

		*def = end;
		*val = atoi( start );
		return TRUE;
	}

	return FALSE;
}

static pboolean parse_factor( ppgram* g, ppsym* lhs, ppprod* p, char** def )
{
	ppsymtype	type;
	ppsym*		sym		= (ppsym*)NULL;
	ppsym*		mod;
	char		name	[ NAMELEN * 2 + 1 ];
	char		op;
	int			i;

	SKIPWHITE();

	if( parse_ident( name, def ) )
	{
		if( !( sym = pp_sym_get_by_name( g, name ) ) )
			sym = pp_sym_create( g, PPSYMTYPE_NONTERM, name, (char*)NULL );

		sym->flags |= PPFLAG_CALLED;
	}
	else if( **def == '(' )
	{
		(*def)++;

		sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
					derive_name( g, lhs->name ), (char*)NULL );

		if( !parse_alternatives( g, sym, def ) )
			return FALSE;

		if( !read_char( def, ')' ) )
			return FALSE;
	}
	else if( ( type = parse_terminal( name, def ) ) > 0 )
	{
		sym = pp_sym_create( g, type, (char*)NULL, name );
		sym->flags |= PPFLAG_CALLED | PPFLAG_DEFINED;
	}
	else
	{
		return TRUE;
	}

	SKIPWHITE();

	/* Replication quantifiers */
	if( **def == PPMOD_KLEENE /* no or many */
			|| **def == PPMOD_POSITIVE /* one or many */
				|| **def == PPMOD_OPTIONAL /* none or one */ )
	{
		op = *( (*def)++ );

		if( op == PPMOD_KLEENE || op == PPMOD_POSITIVE )
		{
			sprintf( name, "%s%c", lhs->name, PPMOD_POSITIVE );

			if( !( mod = pp_sym_get_by_name( g, name ) ) )
			{
				mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

				if( g->flags & PPFLAG_PREVENTLREC )
					pp_prod_create( g, mod, sym, mod, (ppsym*)NULL );
				else
					pp_prod_create( g, mod, mod, sym, (ppsym*)NULL );

				pp_prod_create( g, mod, sym, (ppsym*)NULL );
			}

			sym = mod;
		}

		if( op == PPMOD_OPTIONAL || op == PPMOD_KLEENE )
		{
			sprintf( name, "%s%c", sym->name, PPMOD_OPTIONAL );

			if( !( mod = pp_sym_get_by_name( g, name ) ) )
			{
				mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, lhs->name ), (char*)NULL );

				pp_prod_create( g, mod, sym, (ppsym*)NULL );
				pp_prod_create( g, mod, (ppsym*)NULL );
			}

			sym = mod;
		}
	}

	plist_push( p->rhs, sym );
	return TRUE;
}

static pboolean parse_sequence( ppgram* g, ppsym* lhs, ppprod* p, char** def )
{
	pboolean	err;

	while( ( err = parse_factor( g, lhs, p, def ) ) )
	{
		SKIPWHITE();
		if( **def == ';' || **def == '|' || **def == ')' || **def == '%' )
			break;
	}

	return err;
}

static pboolean parse_alternatives( ppgram* g, ppsym* lhs, char** def )
{
	ppprod*		p;

	while( TRUE )
	{
		p = pp_prod_create( g, lhs, (ppsym*)NULL );

		if( !parse_sequence( g, lhs, p, def ) )
			return FALSE;

		if( **def != '|' )
			break;
		(*def)++;
	}

	return TRUE;
}

static int pp_gram_read( ppgram* g, char** def )
{
	ppsymtype	type;
	ppsym*		sym;
	ppsym*		rsym;
	plistel*	e;
	ppprod*		p;
	int			defcount	= 0;
	char		name		[ NAMELEN + 1 ];
	char		symdef		[ NAMELEN + 1 ];
	int			dflags		= 0;
	int			emit_max	= 0;
	pboolean	doemit;
	pboolean	emitall		= FALSE;
	int			emit;

	while( *def )
	{
		SKIPWHITE();

		if( ! **def )
			break;

		if( parse_ident( name, def ) )
		{
			SKIPWHITE();
			doemit = emitall;
			emit = 0;

			/* Is nonterminal definition? */
			if( **def == ':' )
			{
				(*def)++;

				if( !( sym = pp_sym_get_by_name( g, name ) ) )
					sym = pp_sym_create(
							g, PPSYMTYPE_NONTERM,
								name, (char*)NULL );

				sym->flags |= PPFLAG_DEFINED | dflags;

				if( !parse_alternatives( g, sym, def ) )
					return -1;
			}
			/* Or is named terminal definition? */
			else if( **def == '=' )
			{
				(*def)++;
				SKIPWHITE();

				if( !( type = parse_terminal( symdef, def ) ) )
					PARSEERROR( def, "terminal definition" );

				sym = pp_sym_create( g, type, name, symdef );
				sym->flags |= dflags;
			}
			else
			{
				PARSEERROR( def, "':' or '=' for nonterminal"
									" or named terminal definition" );
				return -1;
			}

			/* More descriptive symbol attribution */
			while( TRUE )
			{
				SKIPWHITE();

				if( **def == '%' )
				{
					(*def)++;
					SKIPWHITE();

					if( !parse_ident( name, def ) )
					{
						PARSEERROR( def, "identifier" );
						return -1;
					}

					if( !strcmp( name, "goal" ) )
					{
						if( sym->type > PPSYMTYPE_NONTERM )
							/* TODO: error reporting */
							fprintf( stderr,
								"Goal symbol must be a nonterminal.\n" );
						else if( g->goal && g->goal != sym )
							/* TODO: error reporting */
							fprintf( stderr,
								"Symbol '%s' already defined as goal\n",
								g->goal->name );
						else
							g->goal = sym;
					}
					else if( !strcmp( name, "emit") )
					{
						doemit = TRUE;

						parse_integer( &emit, def );
					}
					else if( !strcmp( name, "noemit" ) )
						doemit = FALSE;
					else if( !strcmp( name, "lexem" ) )
						sym->flags |= PPFLAG_LEXEM;
					else if( !strcmp( name, "whitespace" ) )
					{
						if( sym->type == PPSYMTYPE_NONTERM )
							printf( "%whitespace currently only allowed for "
									"terminal symbols in this implementation" );
						else
							sym->flags |= PPFLAG_WHITESPACE | PPFLAG_CALLED;
					}
				}
				else
					break;
			}

			/* Set emit ID if configured */
			if( doemit )
			{
				if( emit )
				{
					sym->emit = emit;

					if( emit > emit_max )
						emit_max = emit;
				}
				else
					sym->emit = ++emit_max;
			}

			SKIPWHITE();

			if( !read_char( def, ';' ) )
				return -1;

			defcount++;
		}
		else if( **def == '%' )
		{
			(*def)++;
			SKIPWHITE();

			if( !parse_ident( name, def ) )
			{
				PARSEERROR( def, "identifier" );
				return -1;
			}

			if( !strcmp( name, "emitall" ) )
				emitall = TRUE;
			else if( !strcmp( name, "emitnone" ) )
				emitall = FALSE;
			else if( !strcmp( name, "rrec" ) )
				g->flags |= PPFLAG_PREVENTLREC;
			else if( !strcmp( name, "lrec" ) )
				g->flags &= ~PPFLAG_PREVENTLREC;
			else if( !strcmp( name, "whitespace" ) )
			{
				SKIPWHITE();

				/* on the fly */
				if( ( type = parse_terminal( name, def ) ) > 0 )
					sym = pp_sym_create( g, type, (char*)NULL, name );
				/* or named? */
				else if( parse_ident( name, def ) )
					sym = pp_sym_get_by_name( g, name );
				else
					sym = (ppsym*)NULL;

				if( !sym || sym->type == PPSYMTYPE_NONTERM )
				{
					PARSEERROR( def, "terminal definition for whitespace" );
					return -1;
				}

				sym->flags |= PPFLAG_CALLED | PPFLAG_WHITESPACE;
			}
		}
		else
		{
			PARSEERROR( def, "nonterminal definition" );
			return -1;
		}
	}

	/* If no goal symbol provided, mark first nontermal as goal */
	if( !g->goal )
	{
		plist_for( g->symbols, e )
		{
			sym = (ppsym*)plist_access( e );

			if( sym->type == PPSYMTYPE_NONTERM )
			{
				g->goal = sym;
				break;
			}
		}

		if( !g->goal )
		{
			/* TODO: error reporting */
			fprintf( stderr, "Grammar must at least provide one "
								"nonterminal as goal symbol\n",
						g->goal->name );
			return -1;
		}
	}

	/* Goal symbol is always "called"! */
	g->goal->flags |= PPFLAG_CALLED;

	return defcount;
}

/* Prepares the grammar //g// by computing all necessary stuff required for
runtime and parser generator.

Includes:
- Symbol and productiosn IDs
- FIRST-set computation
- Mark left-recursions
- Lexem flag pull-trough
*/
pboolean pp_gram_prepare( ppgram* g )
{
	plistel*	e;
	plistel*	er;
	plistel*	ep;
	ppprod*		p;
	ppprod*		cp;
	ppsym*		s;
	pboolean	nullable;
	plist*		call;
	plist*		done;
	int			id;
	int			cnt;
	int			pcnt;

	if( !g )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !g->goal )
	{
		/* No such goal! */
		return FALSE;
	}

	/* Redefine list of whitespace symbols */
	plist_erase( g->ws );

	/* Set ID values for symbols and productions */
	for( id = 0, e = plist_first( g->symbols ); e; e = plist_next( e ), id++ )
	{
		s = (ppsym*)plist_access( e );
		s->id = id;

		if( s->flags & PPFLAG_WHITESPACE )
			plist_push( g->ws, s );
	}

	for( id = 0, e = plist_first( g->prods ); e; e = plist_next( e ), id++ )
	{
		p = (ppprod*)plist_access( e );
		p->id = id;
	}

	/* Compute FIRST sets and mark left-recursions */
	cnt = 0;
	call = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	do
	{
		pcnt = cnt;
		cnt = 0;

		plist_for( g->prods, e )
		{
			cp = (ppprod*)plist_access( e );
			plist_push( call, cp );

			while( plist_pop( call, &p ) )
			{
				plist_push( done, p );

				plist_for( p->rhs, er )
				{
					s = (ppsym*)plist_access( er );

					nullable = FALSE;

					if( s->type == PPSYMTYPE_NONTERM )
					{
						/* Union first set */
						plist_union( cp->lhs->first, s->first );

						/* Put prods on stack */
						plist_for( s->prods, ep )
						{
							p = (ppprod*)plist_access( ep );

							if( plist_count( p->rhs ) == 0 )
							{
								nullable = TRUE;
								continue;
							}

							if( p == cp )
							{
								p->lhs->flags |= PPFLAG_LEFTREC;
								p->flags |= PPFLAG_LEFTREC;
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
					cp->flags |= PPFLAG_NULLABLE;
					cp->lhs->flags |= PPFLAG_NULLABLE;
				}

				cnt += plist_count( cp->lhs->first );
			}

			plist_erase( done );
		}
	}
	while( pcnt < cnt );

	plist_clear( call );
	plist_clear( done );

	/* Pull-trough all lexem symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		if( s->type != PPSYMTYPE_NONTERM
				|| !( s->flags & PPFLAG_LEXEM ) )
			continue;

		plist_push( call, s );
	}

	while( plist_pop( call, &s ) )
	{
		plist_push( done, s );

		plist_for( s->prods, e )
		{
			p = (ppprod*)plist_access( e );
			plist_for( p->rhs, er )
			{
				s = (ppsym*)plist_access( er );
				s->flags |= PPFLAG_LEXEM;

				if( s->type == PPSYMTYPE_NONTERM )
				{
					if( !plist_get_by_ptr( done, s )
						&& !plist_get_by_ptr( call, s ) )
						plist_push( call, s );
				}
			}
		}
	}

	/* Clear all lists */
	plist_free( call );
	plist_free( done );

	return TRUE;
}

ppgram* pp_gram_create( char* def )
{
	ppsym*		s;
	ppgram*		g;

	/* Setup grammar description */
	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ),
					PLIST_MOD_RECYCLE
						| PLIST_MOD_EXTKEYS
							| PLIST_MOD_UNIQUE );

	g->prods = plist_create( sizeof( ppprod ),
					PLIST_MOD_RECYCLE );
	g->ws = plist_create( sizeof( ppsym* ), PLIST_MOD_PTR );

	g->eof = pp_sym_create( g, PPSYMTYPE_SPECIAL, "eof", (char*)NULL );

	/* Parse grammar into description */
	if( def && *def )
	{
		#ifdef PPGRAM_SELF_CONTAIN
		fprintf( stderr, "WARNING: USING SELF COINTAINED PARSER!!\n" );
		pp_gram4myself( g );
		#else
		if( pp_gram_read( g, &def ) <= 0 )
			return pp_gram_free( g );
		#endif

		/* Look for unique goal sequence */
		if( plist_count( g->goal->prods ) > 1 )
		{
			s = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, g->goal->name ), (char*)NULL );

			pp_prod_create( g, s, g->goal, (ppsym*)NULL );
			g->goal = s;
		}

		/* Prepare grammar */
		pp_gram_prepare( g );
	}

	return g;
}

void pp_gram_print( ppgram* g )
{
	plistel*	e;
	plistel*	f;

	ppprod*		p;
	ppsym*		s;
	int			maxlhslen	= 0;

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		if( s->type == PPSYMTYPE_NONTERM && pstrlen( s->name ) > maxlhslen )
			maxlhslen = pstrlen( s->name );
	}

	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		printf( "%2d %s%s%s%s%s %-*s : ",
			p->lhs->emit,
			g->goal == p->lhs ? "G" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			p->lhs->flags & PPFLAG_LEXEM ? "X" : " ",
			p->lhs->flags & PPFLAG_WHITESPACE ? "W" : " ",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (ppsym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				printf( " " );

			printf( "%s", pp_sym_to_str( s ) );
		}

		printf( "\n" );
	}

	printf( "\n" );

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		if( s->type != PPSYMTYPE_NONTERM )
			continue;

		printf( "FIRST %-*s {", maxlhslen, s->name );
		plist_for( s->first, f )
		{
			s = (ppsym*)plist_access( f );
			printf( " " );
			printf( "%s", pp_sym_to_str( s ) );
		}

		printf( " }\n" );
	}
}

/** Free grammar //g// and all its related memory. */
ppgram* pp_gram_free( ppgram* g )
{
	plistel*	e;
	ppsym*		s;
	ppprod*		p;

	if( !g )
		return (ppgram*)NULL;

	/* Erase symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		pfree( s->name );

		pfree( s->str );
		p_ccl_free( s->ccl );
		pregex_free( s->re );

		plist_free( s->prods );
		plist_free( s->first );
	}

	/* Erase productions */
	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		plist_free( p->rhs );
	}

	plist_free( g->symbols );
	plist_free( g->prods );
	plist_free( g->ws );
	pfree( g );

	return (ppgram*)NULL;
}



/* Parser for yourself. */

#define T_CCL			PPSYMTYPE_CCL
#define T_STRING		PPSYMTYPE_STRING
#define T_REGEX			PPSYMTYPE_REGEX
#define T_IDENT			10
#define T_INT			11

#define T_SYMBOL		20

#define T_KLEENE		25
#define T_POSITIVE		26
#define T_OPTIONAL		27

#define T_PRODUCTION	30
#define T_NONTERMDEF	31
#define T_INLINE		32

#define T_TERMDEF		35

#define T_FLAG			40
#define T_GFLAG			41
#define T_EMIT			42

ppgram* pp_ast2gram( parray* ast )
{
	typedef struct
	{
		int			emit;
		char*		buf;
		ppsym*		sym;
		parray*		seq;
		int			i;
	} ATT;

	ATT			att;
	ATT*		attp;
	parray*		st;
	ppmatch*	e;
	ppgram*		g;
	ppsym*		scope		= (ppsym*)NULL;
	ppprod*		p;
	int			i;
	pboolean	whitespace;
	pboolean	doemit;
	pboolean	emitall		= FALSE;
	int			emit;
	int			emit_max	= 0;
	char		name		[ NAMELEN * 2 + 1 ];

	g = pp_gram_create( (char*)NULL );
	st = parray_create( sizeof( ATT ), 0 );

	for( e = (ppmatch*)parray_first( ast );
			e <= (ppmatch*)parray_last( ast ); e++ )
	{
		memset( &att, 0, sizeof( ATT ) );

		if( e->type & PPMATCH_BEGIN && !( e->type & PPMATCH_END ) )
		{
			if( e->emit == T_NONTERMDEF )
			{
				e++;

				att.buf = pstrndup( e->start, e->end - e->start );

				if( !( scope = pp_sym_get_by_name( g, att.buf ) ) )
					scope = pp_sym_create( g, PPSYMTYPE_NONTERM,
								att.buf, (char*)NULL );

				pfree( att.buf );
			}
			else if( e->emit == T_PRODUCTION )
				/* Push an empty sequence delimiter */
				parray_push( st, &att );

			continue;
		}

		switch( ( att.emit = e->emit ) )
		{
			case T_IDENT:
				att.buf = pstrndup( e->start, e->end - e->start );
				break;

			case T_INT:
				att.i = atoi( e->start );
				break;

			case T_CCL:
			case T_STRING:
			case T_REGEX:
				att.buf = pstrndup( e->start + 1, e->end - e->start - 2 );

				if( e->emit == T_REGEX )
				{
					char*	s;
					char*	p;

					for( p = s = att.buf; *p = *s; p++, s++ )
						if( *s == '\\' && *( s + 1) )
							*p = *( ++s );
				}
				break;

			case T_INLINE:
				/* Inline nonterminal */
				att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, scope->name ), (char*)NULL );
				att.sym->flags |= PPFLAG_DEFINED;

				while( ( attp = (ATT*)parray_last( st ) )
							&& attp->emit == T_PRODUCTION )
				{
					parray_pop( st );
					p = pp_prod_create( g, att.sym, (ppsym*)NULL );

					while( parray_count( attp->seq ) )
						pp_prod_append( p,
							*( (ppsym**)parray_pop( attp->seq ) ) );

					parray_free( attp->seq );
				}
				break;

			case T_SYMBOL:
				/* Symbol */
				attp = parray_pop( st );

				if( attp->sym )
					att.sym = attp->sym;
				else if( attp->buf &&
							!( att.sym = pp_sym_get_by_name( g, attp->buf ) ) )
				{
					if( attp->emit == T_IDENT )
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
										attp->buf, (char*)NULL );
					else
					{
						att.sym = pp_sym_create( g, attp->emit,
										attp->buf, attp->buf );
						att.sym->flags |= PPFLAG_DEFINED;
					}

					pfree( attp->buf );
				}
				else
				{
					fprintf( stderr, "CHECK YOUR CODE:\n" );
					fprintf( stderr, "%s, %d: Can't find symbol.\n",
										__FILE__, __LINE__ );
				}

				att.sym->flags |= PPFLAG_CALLED;
				break;

			case T_KLEENE:
			case T_POSITIVE:
			case T_OPTIONAL:
				attp = parray_pop( st );

				if( e->emit == T_KLEENE || e->emit == T_POSITIVE )
				{
					sprintf( name, "%s%c", attp->sym->name, PPMOD_POSITIVE );

					if( !( att.sym = pp_sym_get_by_name( g, name ) ) )
					{
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
														name, (char*)NULL );

						if( g->flags & PPFLAG_PREVENTLREC )
							pp_prod_create( g, att.sym,
								attp->sym, att.sym, (ppsym*)NULL );
						else
							pp_prod_create( g, att.sym,
								att.sym, attp->sym, (ppsym*)NULL );

						pp_prod_create( g, att.sym, attp->sym, (ppsym*)NULL );
					}

					attp->sym = att.sym;
				}

				if( e->emit == T_OPTIONAL || e->emit == T_KLEENE )
				{
					sprintf( name, "%s%c", attp->sym->name, PPMOD_OPTIONAL );

					if( !( att.sym = pp_sym_get_by_name( g, name ) ) )
					{
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
														name, (char*)NULL );

						pp_prod_create( g, att.sym, attp->sym, (ppsym*)NULL );
						pp_prod_create( g, att.sym, (ppsym*)NULL );
					}
				}

				att.emit = T_SYMBOL;
				break;

			case T_PRODUCTION:
				att.seq = parray_create( sizeof( ppsym* ), 16 );

				while( ( attp = (ATT*)parray_last( st ) )
							&& attp->emit == T_SYMBOL )
				{
					parray_pop( st );
					parray_push( att.seq, &attp->sym );
				}

				if( !attp->emit )
					parray_pop( st ); /* Pop seqence delimiter */

				break;

			case T_FLAG:
				att.buf = pstrndup( e->start, e->end - e->start );
				break;

			case T_GFLAG:
				if( !strncmp( e->start, "emitall", e->end - e->start ) )
					emitall = TRUE;
				else if( !strncmp( e->start, "emitnone", e->end - e->start ) )
					emitall = FALSE;
				else if( !strncmp( e->start, "lrec", e->end - e->start ) )
					g->flags &= ~PPFLAG_PREVENTLREC;
				else if( !strncmp( e->start, "rrec", e->end - e->start ) )
					g->flags |= PPFLAG_PREVENTLREC;

				att.emit = 0;
				break;

			case T_EMIT:
				if( ( attp = (ATT*)parray_last( st ) ) && attp->emit == T_INT )
				{
					parray_pop( st );
					att.i = attp->i;
				}
				break;

			case T_NONTERMDEF:
			case T_TERMDEF:
				doemit = emitall;
				emit = 0;
				whitespace = FALSE;

				while( ( attp = (ATT*)parray_last( st ) )
							&& ( attp->emit == T_FLAG
									|| attp->emit == T_EMIT ) )
				{
					parray_pop( st );

					if( attp->emit == T_EMIT )
					{
						doemit = TRUE;
						emit = attp->i;
						continue;
					}
					else if( strcmp( attp->buf, "goal" ) == 0 )
					{
						if( scope && !g->goal ) /* fixme */
						{
							g->goal = scope;
							scope->flags |= PPFLAG_CALLED;
						}
					}
					else if( strcmp( attp->buf, "noemit" ) == 0 )
						doemit = FALSE;
					else if( strcmp( attp->buf, "whitespace" ) == 0 )
						whitespace = TRUE;

					pfree( attp->buf );
				}

				if( e->emit == T_TERMDEF )
				{
					attp = parray_pop( st ); 	/* Definition */
					attp = parray_pop( st );	/* Identifier */

					scope = pp_sym_create( g, ( attp + 1 )->emit,
										attp->buf, ( attp + 1 )->buf );
					scope->flags |= PPFLAG_DEFINED;

					pfree( ( attp + 1 )->buf );
					pfree( attp->buf );

					/* Whitespace now only for terminals */
					if( whitespace )
					{
						scope->flags |= PPFLAG_WHITESPACE;
						plist_push( g->ws, scope );
					}
				}

				while( e->emit == T_NONTERMDEF
						&& ( attp = (ATT*)parray_last( st ) )
							&& attp->emit == T_PRODUCTION )
				{
					parray_pop( st );
					p = pp_prod_create( g, scope, (ppsym*)NULL );

					while( parray_count( attp->seq ) )
						pp_prod_append( p,
							*( (ppsym**)parray_pop( attp->seq ) ) );

					parray_free( attp->seq );
				}

				scope->flags |= PPFLAG_DEFINED;

				/* Set emit ID if configured */
				if( doemit )
				{
					if( emit )
					{
						scope->emit = emit;

						if( emit > emit_max )
							emit_max = emit;
					}
					else
						scope->emit = ++emit_max;
				}

				att.emit = 0;
				break;
		}

		if( att.emit > 0 )
			parray_push( st, &att );
	}

	if( parray_count( st ) )
	{
		fprintf( stderr, "CHECK YOUR CODE:\n" );
		fprintf( stderr, "%s, %d: Still %d elements on stack\n",
			__FILE__, __LINE__, parray_count( st ) );

		while( attp = parray_pop( st ) )
			printf( "%d\n", attp->emit );
	}

	parray_free( st );

	/* Prepare grammar */
	pp_gram_prepare( g );

	return g;
}

void pp_gram2gram( ppgram* g )
{
	ppsym* 		ident;
	ppsym*		integer;
	ppsym*		ccl;
	ppsym*		string;
	ppsym*		regex;

	ppsym*		semicolon;
	ppsym*		colon;
	ppsym*		pipe;
	ppsym*		equal;
	ppsym*		star;
	ppsym*		plus;
	ppsym*		quest;
	ppsym*		percent;
	ppsym*		lpar;
	ppsym*		rpar;

	ppsym*		opt_int;
	ppsym*		emit;
	ppsym*		flagemit;
	ppsym*		flag;
	ppsym*		flags;
	ppsym*		flagslist;
	ppsym*		optflagslist;

	ppsym*		inlin;
	ppsym*		symbol;
	ppsym*		mod_kleene;
	ppsym*		mod_positive;
	ppsym*		mod_optional;
	ppsym*		modifier;
	ppsym*		sequence;
	ppsym*		alternative;
	ppsym*		alternation;
	ppsym*		nontermdef;
	ppsym*		termdef;
	ppsym*		gflag;
	ppsym*		gflags;
	ppsym*		gflagslist;
	ppsym*		definition;
	ppsym*		definitions;
	ppsym*		grammar;

	ppsym*		s_emit;
	ppsym*		s_noemit;
	ppsym*		s_goal;
	ppsym*		s_whitespace;
	ppsym*		s_emitall;
	ppsym*		s_emitnone;
	ppsym*		s_lrec;
	ppsym*		s_rrec;

	ppsym*		ws;

	/*
	============================================================================
	                This is the incomplete grammar draft
	============================================================================

	ident		= /[A-Za-z_][A-Za-z0-9_]*°/
				%emit <T_IDENT>
				;

	int			= /[0-9]+/
				%emit <T_INT>
				;

	ccl 		= /'.*'/
				%emit <T_CCL>
				;

	string 		= /".*"/
				%emit <T_STRING>
				;

	regex 		= /\/(\.|[^\/])*\//
				%emit <T_REGEX>
				;

	// Flags -------------------------------------------------------------------

	opt_int		: int
				|
				;


	emit		: "emit" opt_int

				%emit <T_EMIT>
				;

	flag		: "noemit"
				| "goal"

				%emit <T_FLAG>
				;

	flagemit	: flag | emit
				;

	flags		: flags flagemit | flagemit
				;

	flagslist	: flaglist '%' flags | '%' flags
				;

	optflagslist: flagslist
				|
				;

	// Nonterminals ------------------------------------------------------------

	inline		: '(' alternation ')'
				%emit <T_INLINE>

	symbol 		: ident | ccl | string | regex | inline
				%emit <T_SYMBOL>
				;

	mod_kleene	: symbol '*'
				%emit <T_KLEENE>
				;

	mod_positive: symbol '+'
				%emit <T_POSITIVE>
				;

	mod_optional: symbol '?'
				%emit <T_OPTIONAL>
				;

	modifier	: mod_kleene
				| mod_positive
				| mod_optional
				| symbol
				;

	sequence	: sequence modifier | modifier ;
	alternative : sequence | ;
	alternation	: alternation '|' alternative | alternative ;

	nontermdef	: ident ':' alternation optflagslist ';'

				%emit <T_NONTERMDEF>
				;

	// Terminals ---------------------------------------------------------------

	termdef		: ident '=' ccl optflagslist ';'
				| ident '=' string  optflagslist ';'
				| ident '=' regex optflagslist ';'

				%emit <T_TERMDEF>
				;

	// Grammar -----------------------------------------------------------------

	gflag		: "emitall"
				| "emitnone"
				| "lrec"
				| "rrec"

				%emit <T_FLAG>
				;

	gflags		: gflags gflag | gflag
				;

	gflagslist	: gflagslist '%' gflags | '%' gflags
				;

	definition	: nontermdef | termdef | gflagslist ;

	definitions	: definitions definition | definition ;

	grammar		: definitions ;
	*/

	/* Terminals */
	semicolon = pp_sym_create( g, PPSYMTYPE_CCL, "semicolon", ";" );
	colon = pp_sym_create( g, PPSYMTYPE_CCL, "colon", ":" );
	pipe = pp_sym_create( g, PPSYMTYPE_CCL, "pipe", "|" );
	equal = pp_sym_create( g, PPSYMTYPE_CCL, "equal", "=" );
	star = pp_sym_create( g, PPSYMTYPE_CCL, "star", "*" );
	plus = pp_sym_create( g, PPSYMTYPE_CCL, "plus", "+" );
	quest = pp_sym_create( g, PPSYMTYPE_CCL, "quest", "?" );
	percent = pp_sym_create( g, PPSYMTYPE_CCL, "percent", "%" );
	lpar = pp_sym_create( g, PPSYMTYPE_CCL, "lpar", "(" );
	rpar = pp_sym_create( g, PPSYMTYPE_CCL, "rpar", ")" );

	s_emitall = pp_sym_create( g, PPSYMTYPE_STRING, "s_emitall", "emitall" );
	s_emitnone = pp_sym_create( g, PPSYMTYPE_STRING, "s_emitnone", "emitnone" );
	s_emit = pp_sym_create( g, PPSYMTYPE_STRING, "s_emit", "emit" );
	s_noemit = pp_sym_create( g, PPSYMTYPE_STRING, "s_noemit", "noemit" );
	s_goal = pp_sym_create( g, PPSYMTYPE_STRING, "s_goal", "goal" );
	s_whitespace = pp_sym_create( g, PPSYMTYPE_STRING,
									"s_whitespace", "whitespace" );
	s_lrec = pp_sym_create( g, PPSYMTYPE_STRING, "s_lrec", "lrec" );
	s_rrec = pp_sym_create( g, PPSYMTYPE_STRING, "s_rrec", "rrec" );

	ident = pp_sym_create( g, PPSYMTYPE_REGEX,
				"ident", "[A-Za-z_][A-Za-z0-9_]*" );
	ident->emit = T_IDENT;

	integer = pp_sym_create( g, PPSYMTYPE_REGEX, "int", "[0-9]+" );
	integer->emit = T_INT;

	ccl = pp_sym_create( g, PPSYMTYPE_REGEX, "ccl", "'.*'" );
	ccl->emit = T_CCL;

	string = pp_sym_create( g, PPSYMTYPE_REGEX, "string", "\".*\"" );
	string->emit = T_STRING;

	regex = pp_sym_create( g, PPSYMTYPE_REGEX, "regex", "/(\\\\.|[^\\\\/])*/" );
	regex->emit = T_REGEX;

	/* Nonterminals */
	opt_int = pp_sym_create( g, PPSYMTYPE_NONTERM, "opt_int", (char*)NULL );

	emit = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );
	emit->emit = T_EMIT;

	flag = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );
	flag->emit = T_FLAG;

	flagemit = pp_sym_create( g, PPSYMTYPE_NONTERM, "emitflag", (char*)NULL );
	flags = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );
	flagslist = pp_sym_create( g, PPSYMTYPE_NONTERM, "flagslist", (char*)NULL );

	optflagslist = pp_sym_create( g, PPSYMTYPE_NONTERM,
										"optflagslist", (char*)NULL );

	inlin = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );
	inlin->emit = T_INLINE;

	symbol = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	symbol->emit = T_SYMBOL;

	mod_kleene = pp_sym_create( g, PPSYMTYPE_NONTERM,
									"mod_kleene", (char*)NULL );
	mod_kleene->emit = T_KLEENE;

	mod_positive = pp_sym_create( g, PPSYMTYPE_NONTERM,
									"mod_positive", (char*)NULL );
	mod_positive->emit = T_POSITIVE;

	mod_optional = pp_sym_create( g, PPSYMTYPE_NONTERM,
									"mod_optional", (char*)NULL );
	mod_optional->emit = T_OPTIONAL;

	modifier = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );
	sequence = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	alternative = pp_sym_create( g, PPSYMTYPE_NONTERM,
					"alternative", (char*)NULL );
	alternative->emit = T_PRODUCTION;

	alternation = pp_sym_create( g, PPSYMTYPE_NONTERM,
					"alternation", (char*)NULL );

	nontermdef = pp_sym_create( g, PPSYMTYPE_NONTERM,
					"nontermdef", (char*)NULL );
	nontermdef->emit = T_NONTERMDEF;

	termdef = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );
	termdef->emit = T_TERMDEF;

	gflag = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	gflag->emit = T_GFLAG;

	gflags = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );
	gflagslist = pp_sym_create( g, PPSYMTYPE_NONTERM,
									"gflagslist", (char*)NULL );
	definition = pp_sym_create( g, PPSYMTYPE_NONTERM,
					"definition", (char*)NULL );
	definitions = pp_sym_create( g, PPSYMTYPE_NONTERM,
					"definitions", (char*)NULL );

	grammar = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );

	/* Productions */
	pp_prod_create( g, opt_int, integer, (ppsym*)NULL );
	pp_prod_create( g, opt_int, (ppsym*)NULL );

	pp_prod_create( g, emit, s_emit, opt_int, (ppsym*)NULL );

	pp_prod_create( g, flag, s_noemit, (ppsym*)NULL );
	pp_prod_create( g, flag, s_goal, (ppsym*)NULL );
	pp_prod_create( g, flag, s_whitespace, (ppsym*)NULL );

	pp_prod_create( g, flagemit, emit, (ppsym*)NULL );
	pp_prod_create( g, flagemit, flag, (ppsym*)NULL );

	pp_prod_create( g, flags, flags, flagemit, (ppsym*)NULL );
	pp_prod_create( g, flags, flagemit, (ppsym*)NULL );

	pp_prod_create( g, flagslist, flagslist, percent, flags, (ppsym*)NULL );
	pp_prod_create( g, flagslist, percent, flags, (ppsym*)NULL );

	pp_prod_create( g, optflagslist, flagslist, (ppsym*)NULL );
	pp_prod_create( g, optflagslist, (ppsym*)NULL );

	pp_prod_create( g, inlin, lpar, alternation, rpar, (ppsym*)NULL );

	pp_prod_create( g, symbol, ident, (ppsym*)NULL );
	pp_prod_create( g, symbol, ccl, (ppsym*)NULL );
	pp_prod_create( g, symbol, string, (ppsym*)NULL );
	pp_prod_create( g, symbol, regex, (ppsym*)NULL );
	pp_prod_create( g, symbol, inlin, (ppsym*)NULL );

	pp_prod_create( g, mod_kleene, symbol, star, (ppsym*)NULL );

	pp_prod_create( g, mod_positive, symbol, plus, (ppsym*)NULL );

	pp_prod_create( g, mod_optional, symbol, quest, (ppsym*)NULL );

	pp_prod_create( g, modifier, mod_kleene, (ppsym*)NULL );
	pp_prod_create( g, modifier, mod_positive, (ppsym*)NULL );
	pp_prod_create( g, modifier, mod_optional, (ppsym*)NULL );
	pp_prod_create( g, modifier, symbol, (ppsym*)NULL );

	pp_prod_create( g, sequence, sequence, modifier, (ppsym*)NULL );
	pp_prod_create( g, sequence, modifier, (ppsym*)NULL );

	pp_prod_create( g, alternative, sequence, (ppsym*)NULL );
	pp_prod_create( g, alternative, (ppsym*)NULL );

	pp_prod_create( g, alternation, alternation,
					pipe, alternative, (ppsym*)NULL );
	pp_prod_create( g, alternation, alternative, (ppsym*)NULL );

	pp_prod_create( g, nontermdef, ident, colon, alternation, optflagslist,
						semicolon, (ppsym*)NULL );

	pp_prod_create( g, termdef, ident, equal, ccl,
						optflagslist, semicolon, (ppsym*)NULL );
	pp_prod_create( g, termdef, ident, equal, string,
						optflagslist, semicolon, (ppsym*)NULL );
	pp_prod_create( g, termdef, ident, equal, regex,
						optflagslist, semicolon, (ppsym*)NULL );

	pp_prod_create( g, gflag, s_emitall, (ppsym*)NULL );
	pp_prod_create( g, gflag, s_emitnone, (ppsym*)NULL );
	pp_prod_create( g, gflag, s_lrec, (ppsym*)NULL );
	pp_prod_create( g, gflag, s_rrec, (ppsym*)NULL );

	pp_prod_create( g, gflags, gflags, gflag, (ppsym*)NULL );
	pp_prod_create( g, gflags, gflag, (ppsym*)NULL );

	pp_prod_create( g, gflagslist, gflagslist, percent, gflags, (ppsym*)NULL );
	pp_prod_create( g, gflagslist, percent, gflags, (ppsym*)NULL );

	pp_prod_create( g, definition, termdef, (ppsym*)NULL );
	pp_prod_create( g, definition, nontermdef, (ppsym*)NULL );
	pp_prod_create( g, definition, gflagslist, (ppsym*)NULL );

	pp_prod_create( g, definitions, definitions, definition, (ppsym*)NULL );
	pp_prod_create( g, definitions, definition, (ppsym*)NULL );

	pp_prod_create( g, grammar, definitions, (ppsym*)NULL );

	/* That other skirmish... */
	g->goal = grammar;

	ws = pp_sym_create( g, PPSYMTYPE_REGEX, "whitespace", "[ \\t\\r\\n]+" );
	ws->flags = PPFLAG_WHITESPACE;
}
