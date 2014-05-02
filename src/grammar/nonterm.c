/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	nonterm.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgnonterminal* pg_nonterminal_create( pggrammar* grammar, char* name )
{
	pgsymbol*	sym;

	if( !( grammar && name && *name ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	if( !( sym = pg_symbol_create( grammar, PGSYMTYPE_NONTERMINAL, name ) ) )
		return (pgterminal*)NULL;

	sym->productions = plist_create( 0, PLIST_MOD_PTR );

	if( !( grammar->goal ) )
		grammar->goal = sym;

	return sym;
}

/* Destructor */

pgnonterminal* pg_nonterminal_drop( pgterminal* nonterminal )
{
	if( !nonterminal )
		return (pgnonterminal*)NULL;

	if( !pg_symbol_is_nonterminal( nonterminal ) )
	{
		WRONGPARAM;
		return (pgnonterminal*)NULL;
	}

	while( plist_count( nonterminal->productions ) )
		pg_production_drop( (pgproduction*)plist_access(
								plist_first( nonterminal->productions ) ) );

	plist_free( nonterminal->productions );

	return pg_symbol_free( nonterminal );
}

/* Retrieval */

pgnonterminal* pg_nonterminal_get( pggrammar* g, int offset )
{
	int			i;
	pgsymbol*	s;

	if( !( g && offset >= 0 ) )
	{
		WRONGPARAM;
		return (pgnonterminal*)NULL;
	}

	for( i = 0; s = pg_symbol_get( g, i ); i++ )
	{
		if( pg_symbol_get_type( s ) == PGSYMTYPE_NONTERMINAL )
		{
			if( !offset )
				return (pgnonterminal*)s;

			offset--;
		}
	}

	return (pgnonterminal*)NULL;
}

/* Attribute: emit */

pboolean pg_nonterminal_get_emit( pgnonterminal* nt )
{
	if( !nt )
	{
		WRONGPARAM;
		return FALSE;
	}

	return nt->emit;
}


pboolean pg_nonterminal_set_emit( pgnonterminal* nt, pboolean emit )
{
	if( !nt )
	{
		WRONGPARAM;
		return FALSE;
	}

	nt->emit = emit;
	return TRUE;
}
