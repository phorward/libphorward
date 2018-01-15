/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	sym.c
Usage:	Symbol management function.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Creates a new symbol in the grammar //g//.

	//name// is the unique name for the symbol. It can be left empty,
	configuring the symbol as an unnamed symbol.
*/
ppsym* pp_sym_create( ppgram* g, char* name )
{
	ppsym*	sym;

	if( !g )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	if( name && pp_sym_get_by_name( g, name ) )
	{
		fprintf( stderr, "Symbol '%s' already exists in this grammar\n", name );
		return (ppsym*)NULL;
	}

	name = pstrdup( name );

	/* Insert into symbol table */
	if( !( sym = (ppsym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		pfree( name );
		return (ppsym*)NULL;
	}

	sym->id = ++( g->sym_id );
	sym->grm = g;
	sym->name = name;

	if( !sym->name )
		sym->flags |= PPFLAG_NAMELESS;

	sym->first = plist_create( 0, PLIST_MOD_PTR );

	return sym;
}

/** Frees a symbol. */
ppsym* pp_sym_free( ppsym* sym )
{
	if( !sym )
		return (ppsym*)NULL;

	if( sym->flags & PPFLAG_FREEEMIT )
		pfree( sym->emit );

	pfree( sym->name );

	plist_free( sym->first );

	/* Remove symbol from pool */
	plist_remove( sym->grm->symbols,
		plist_get_by_ptr( sym->grm->symbols, sym ) );

	return (ppsym*)NULL;
}

/** Removes a symbol from its grammar. */
ppsym* pp_sym_drop( ppsym* sym )
{
	plistel* 	e;
	ppprod*		p;
	int			i;

	if( !sym )
		return (ppsym*)NULL;

	/* Remove all references from other productions */
	for( e = plist_first( sym->grm->prods ); e; )
	{
		p = (ppprod*)plist_access( e );
		e = plist_next( e );

		if( p->lhs == sym )
		{
			pp_prod_free( p );
			continue;
		}

		pp_prod_remove( p, sym );
	}

	/* Clear yourself */
	return pp_sym_free( sym );
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
		if( PPSYM_IS_NONTERMINAL( sym ) || !( sym->flags & PPFLAG_NAMELESS ) )
			continue;

		if( sym->name && strcmp( sym->name, name ) == 0 )
			return sym;
	}

	return (ppsym*)NULL;
}

/** Get the //n//th production from symbol //sym//.
//sym// must be a nonterminal.

Returns (ppprod*)NULL if the production is not found or the symbol is
configured differently. */
ppprod* pp_sym_getprod( ppsym* sym, int n )
{
	plistel*	e;
	ppprod*		p;

	if( !( sym && PPSYM_IS_NONTERMINAL( sym ) && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	plist_for( sym->grm->prods, e )
	{
		p = (ppprod*)plist_access( e );
		if( p->lhs == sym && !n-- )
			return p;
	}

	return (ppprod*)NULL;
}

/** Returns the string representation of symbol //sym//.

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
						( pstrlen( sym->name ) + 1 + 3 + 1 + 1 )
							* sizeof( char ) );

		if( sym->name )
			sprintf( sym->strval, "%c:%s",
				PPSYM_IS_TERMINAL( sym ) ? 'T' : 'N',
					sym->name );
		else
			sprintf( sym->strval, "%c:%03d",
				PPSYM_IS_TERMINAL( sym ) ? 'T' : 'N',
					sym->id );
	}

	return sym->strval;
}
