/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	sym.c
Usage:	Symbol management function.
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
	char*	fulldef;

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
	sym->grm = g;
	sym->name = name;
	sym->def = pstrdup( def );

	switch( ( sym->type = type ) )
	{
		case PPSYMTYPE_NONTERM:
			sym->prods = plist_create( 0, PLIST_MOD_PTR  );
			break;

		/* THIS HAS TO BE REVISED... */
		case PPSYMTYPE_CCL:
			fulldef = (char*)pmalloc( pstrlen( def ) + 2 + 1 );
			sprintf( fulldef, "[%s]", def );

			sym->ccl = p_ccl_create( 0, 255, def ); /* TODO */
			sym->ptn = plex_define( g->lex, fulldef, 1, 0 );

			pfree( fulldef );
			break;

		case PPSYMTYPE_STRING:
			sym->ptn = plex_define( g->lex, def, 1, PREGEX_COMP_STATIC );
			sym->str = pstrdup( def );
			break;

		case PPSYMTYPE_REGEX:
			sym->ptn = plex_define( g->lex, def, 1, 0 );
			sym->re = pregex_create( def, 0 );
			break;

		case PPSYMTYPE_FUNCTION:
			if( !( g->getsymfunc ) )
			{
				fprintf( stderr,
					"Hey friend! There is no getsymfunc() provided "
					"along your grammar object...but don't worry: "
					"Just fix your code :)\n" );
				break;
			}

			if( !( sym->sf = *(g->getsymfunc)( def ) ) )
			{
				/* TODO */
				fprintf( stderr,
					"The symbol function '%s' is not defined\n", def );
			}
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

/** Frees a symbol. */
ppsym* pp_sym_drop( ppsym* sym )
{
	plistel* 	e;
	ppprod*		p;

	if( !sym )
		return (ppsym*)NULL;

	if( sym->flags & PPFLAG_FREEEMIT )
		pfree( sym->emit );

	switch( sym->type )
	{
		case PPSYMTYPE_NONTERM:
			/* Drop related productions */
			while( ( e = plist_first( sym->prods ) ) )
				pp_prod_drop( (ppprod*)plist_access( e ) );

			plist_free( sym->prods );
			break;

		case PPSYMTYPE_CCL:
			p_ccl_free( sym->ccl );
			break;

		case PPSYMTYPE_STRING:
			pfree( sym->str );
			break;

		case PPSYMTYPE_REGEX:
			pregex_free( sym->re );
			break;

		default:
			break;
	}

	/* Remove all references from productions */
	plist_for( sym->grm->prods, e )
	{
		p = (ppprod*)plist_access( e );
		pp_prod_remove( p, sym );
	}

	/* Remove whitespace occurences */
	if( ( e = plist_get_by_ptr( sym->grm->ws, sym ) ) ) /* fixme */
		plist_remove( sym->grm->ws, e );

	/* Remove symbol from pool */
	plist_remove( sym->grm->symbols,
		plist_get_by_ptr( sym->grm->symbols, sym ) );

	pfree( sym->name );

	return (ppsym*)NULL;
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

/** Find a nameless terminal symbol by its pattern. */
ppsym* pp_sym_get_nameless_term_by_def( ppgram* g, char* name )
{
	int		i;
	ppsym*	sym;

	if( !( g && name && *name ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	for( i = 0; ( sym = pp_sym_get( g, i ) ); i++ )
	{
		if( sym->type == PPSYMTYPE_NONTERM
				|| !( sym->flags & PPFLAG_NAMELESS ) )
			continue;

		if( strcmp( sym->name, name ) == 0 )
			return sym;
	}

	return (ppsym*)NULL;
}

/** Get the //n//th production from symbol //sym//. //sym// must be of type
nonterminal.

Returns (ppprod*)NULL if the production is not found or the symbol is
configured differently. */
ppprod* pp_sym_getprod( ppsym* sym, int n )
{
	if( !( sym && sym->type == PPSYMTYPE_NONTERM && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	return (ppprod*)plist_access( plist_get( sym->prods, n ) );
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
				sprintf( sym->strval, "[%s]", sym->name );
				break;

			case PPSYMTYPE_STRING:
				sprintf( sym->strval, "%c%s%c",
					sym->emit ? '"' : '\'',
					sym->name,
					sym->emit ? '"' : '\'' );
				break;

			case PPSYMTYPE_REGEX:
				sprintf( sym->strval, "/%s/", sym->name );
				break;

			case PPSYMTYPE_SPECIAL:
				sprintf( sym->strval, "@%s", sym->name );
				break;

			case PPSYMTYPE_FUNCTION:
				sprintf( sym->strval, "%s()", sym->name );
				break;

			default:
				MISSINGCASE;
				break;
		}
	}

	return sym->strval;
}
