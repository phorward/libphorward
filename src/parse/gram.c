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
			break;

		case PPSYMTYPE_STRING:
			sym->str = pstrdup( def );
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
	{
		/* Terminals reference only themself as FIRST set. */
		plist_push( sym->first, sym );

		/* Define symbol with no name as nameless, use def as name.
			The name is used by several debug functions, so this
			flagging prevents from NULL-pointer bugs. */
		if( !sym->name )
		{
			sym->name = pstrdup( def );
			sym->flags |= PPFLAG_NAMELESS;
		}
	}

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

ppgram* pp_gram_create( char* bnf )
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
	if( bnf && *bnf )
	{
		if( !pp_bnf_to_gram( g, bnf ) )
			return pp_gram_free( g );

		/* Look for unique goal sequence */
		if( plist_count( g->goal->prods ) > 1 )
		{
			s = pp_sym_create( g, PPSYMTYPE_NONTERM,
							pp_derive_name( g, g->goal->name ), (char*)NULL );

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

