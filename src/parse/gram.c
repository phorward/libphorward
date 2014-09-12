/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	gram.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

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

	if( !( sym = (ppsym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		fprintf( stderr, "Symbol '%s' already exists\n", name );
		pfree( name );
		return (ppsym*)NULL;
	}

	switch( ( sym->type = type ) )
	{
		case PPSYMTYPE_NONTERM:
			sym->prods = plist_create( 0, PLIST_MOD_PTR  );
			break;

		case PPSYMTYPE_CCL:
			sym->ccl = p_ccl_create( 0, 255, def );
			name = pstrdup( p_ccl_to_str( sym->ccl, TRUE ) );
			break;

		case PPSYMTYPE_STRING:
			name = pstrdup( def );
			break;

		default:
			break;
	}

	sym->id = -1;
	sym->name = name;
	sym->first = plist_create( 0, PLIST_MOD_PTR );

	if( sym->type != PPSYMTYPE_NONTERM )
		plist_push( sym->first, sym );

	return sym;
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

/* Parse BNF */

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

	sprintf( deriv, "%s'", base );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !plist_get_by_key( g->symbols, deriv ) )
			return deriv;

		strcat( deriv, "'" );
	}

	return (char*)NULL;
}

static pboolean parse_factor( ppgram* g, ppsym* lhs, ppprod* p, char** def )
{
	ppsym*		sym		= (ppsym*)NULL;
	ppsym*		mod;
	char*		start;
	char*		end;
	char		name	[ NAMELEN + 1 ];
	char		stopch;
	char		op;
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

		if( !( sym = (ppsym*)plist_access(
								plist_get_by_key( g->symbols, name ) ) ) )
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
			sym = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, name );
		else
			sym = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, name );
	}
	else
	{
		return TRUE;
	}

	SKIPWHITE();

	/* Replication modifiers */
	if( **def == PPMOD_KLEENE
			|| **def == PPMOD_POSITIVE
				|| **def == PPMOD_OPTIONAL )
	{
		op = *( (*def)++ );

		if( op == PPMOD_KLEENE || op == PPMOD_POSITIVE )
		{
			mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

			pp_prod_create( g, mod, mod, sym, (ppsym*)NULL );
			pp_prod_create( g, mod, sym, (ppsym*)NULL );

			sym = mod;
		}

		if( op == PPMOD_OPTIONAL || op == PPMOD_KLEENE )
		{
			mod = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, lhs->name ), (char*)NULL );

			pp_prod_create( g, mod, sym, (ppsym*)NULL );
			pp_prod_create( g, mod, (ppsym*)NULL );

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
	char*		start;
	char*		end;
	ppsym*		sym;
	plistel*	e;
	int			defcount	= 0;
	char		name		[ NAMELEN + 1 ];
	int			dflags		= 0;

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

			if( !( sym = (ppsym*)plist_access(
										plist_get_by_key(
											g->symbols, name ) ) ) )
				sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
											name, (char*)NULL );

			sym->flags |= PPFLAG_DEFINED | dflags;

			/* Read symbol attributation */
			while( TRUE )
			{
				SKIPWHITE();

				/* Goal symbol defined */
				if( **def == '$' )
				{
					(*def)++;
					if( g->goal && g->goal != sym )
						/* TODO: error reporting */
						fprintf( stderr,
							"Symbol '%s' already defined as goal\n",
							g->goal->name );
					else
						g->goal = sym;
				}
				/* Nonterminal is a node in the AST? */
				else if( **def == '#' )
				{
					(*def)++;
					sym->flags |= PPFLAG_ASTNODE;
				}
				else
					break;
			}

			if( !read_char( def, ':' ) )
				return -1;

			if( !parse_alternatives( g, sym, def ) )
				return -1;

			/* More descriptive symbol attribution */
			while( TRUE )
			{
				SKIPWHITE();

				/* Goal symbol defined */
				if( **def == '%' )
				{
					(*def)++;
					SKIPWHITE();

					start = end = *def;

					while( ( isalnum( *end ) || *end == '_' )
							&& end - start < NAMELEN )
						end++;

					*def = end;

					if( !strncmp( start, "goal", end - start ) )
					{
						if( g->goal && g->goal != sym )
							/* TODO: error reporting */
							fprintf( stderr,
								"Symbol '%s' already defined as goal\n",
								g->goal->name );
						else
							g->goal = sym;
					}
					else if( !strncmp( start, "emit", end - start ) )
						sym->flags |= PPFLAG_ASTNODE;
					else if( !strncmp( start, "noemit", end - start ) )
						sym->flags &= ~PPFLAG_ASTNODE;
					else if( !strncmp( start, "whitespace", end - start ) )
						sym->flags |= PPFLAG_WHITESPACE;
				}
				else
					break;
			}

			if( !read_char( def, ';' ) )
				return -1;

			defcount++;
		}
		else if( *start == '%' )
		{
			(*def)++;
			SKIPWHITE();

			end = start;
			if( isalpha( *start ) || *start == '_' )
				while( ( isalnum( *end ) || *end == '_' )
						&& end - start < NAMELEN )
					end++;
			else
			{
				PARSEERROR( def, "identifier" );
				return -1;
			}

			if( !strncmp( start, "emitall", end - start ) )
				dflags |= PPFLAG_ASTNODE;
			else if( !strncmp( start, "emitnone", end - start ) )
				dflags &= ~PPFLAG_ASTNODE;

			*def = end;
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

ppgram* pp_gram_free( ppgram* g )
{
	plistel*	e;
	ppsym*		sym;
	ppprod*		prod;

	if( !g )
		return (ppgram*)NULL;

	/* Erase symbols */
	plist_for( g->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		pfree( sym->name );

		plist_free( sym->prods );
		plist_free( sym->first );
		p_ccl_free( sym->ccl );
	}

	plist_free( g->symbols );

	/* Erase prods */
	plist_for( g->prods, e )
	{
		prod = (ppprod*)plist_access( e );

		plist_free( prod->rhs );
		pfree( prod->strval );
	}

	plist_free( g->prods );

	pfree( g );

	return (ppgram*)NULL;
}

ppgram* pp_gram_create( char* def )
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
	int			cnt;
	int			pcnt;
	ppgram*		g;

	/* Setup grammar description */
	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ),
					PLIST_MOD_RECYCLE
						| PLIST_MOD_EXTKEYS
							| PLIST_MOD_UNIQUE );

	g->prods = plist_create( sizeof( ppprod ),
					PLIST_MOD_RECYCLE );

	g->eof = pp_sym_create( g, PPSYMTYPE_SPECIAL, "eof", (char*)NULL );

	/* Parse grammar into description */
	if( def && *def && pp_gram_read( g, &def ) <= 0 )
		return pp_gram_free( g );

	/* Look for unique goal sequence */
	if( plist_count( g->goal->prods ) > 1 )
	{
		s = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, g->goal->name ), (char*)NULL );

		pp_prod_create( g, s, g->goal, (ppsym*)NULL );
		g->goal = s;
	}

	/* Set ID values for symbols and productions */
	for( cnt = 0, e = plist_first( g->symbols ); e; e = plist_next( e ), cnt++ )
	{
		s = (ppsym*)plist_access( e );
		s->id = cnt;
	}

	for( cnt = 0, e = plist_first( g->prods ); e; e = plist_next( e ), cnt++ )
	{
		p = (ppprod*)plist_access( e );
		p->id = cnt;
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

	plist_free( call );
	plist_free( done );

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
		printf( "%s%s%s%s%s %-*s : ",
			g->goal == p->lhs ? "G" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			p->lhs->flags & PPFLAG_ASTNODE ? "A" : " ",
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

