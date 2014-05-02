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

pgnonterm* pg_nonterm_create( pggrammar* grammar, char* name )
{
	pgsymbol*	sym;

	if( !( grammar && name && *name ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	if( !( sym = pg_symbol_create( grammar, PGSYMTYPE_NONTERMINAL, name ) ) )
		return (pgterm*)NULL;

	sym->productions = plist_create( 0, PLIST_MOD_PTR );

	if( !( grammar->goal ) )
		grammar->goal = sym;

	return sym;
}

/* Destructor */

pgnonterm* pg_nonterm_drop( pgterm* nonterminal )
{
	if( !nonterminal )
		return (pgnonterm*)NULL;

	if( !pg_symbol_is_nonterm( nonterminal ) )
	{
		WRONGPARAM;
		return (pgnonterm*)NULL;
	}

	while( plist_count( nonterminal->productions ) )
		pg_prod_drop( (pgprod*)plist_access(
								plist_first( nonterminal->productions ) ) );

	plist_free( nonterminal->productions );

	return pg_symbol_free( nonterminal );
}

/* Retrieval */

pgnonterm* pg_nonterm_get( pggrammar* g, int offset )
{
	int			i;
	pgsymbol*	s;

	if( !( g && offset >= 0 ) )
	{
		WRONGPARAM;
		return (pgnonterm*)NULL;
	}

	for( i = 0; s = pg_symbol_get( g, i ); i++ )
	{
		if( pg_symbol_get_type( s ) == PGSYMTYPE_NONTERMINAL )
		{
			if( !offset )
				return (pgnonterm*)s;

			offset--;
		}
	}

	return (pgnonterm*)NULL;
}

/* Attribute: emit */

pboolean pg_nonterm_get_emit( pgnonterm* nt )
{
	if( !nt )
	{
		WRONGPARAM;
		return FALSE;
	}

	return nt->emit;
}


pboolean pg_nonterm_set_emit( pgnonterm* nt, pboolean emit )
{
	if( !nt )
	{
		WRONGPARAM;
		return FALSE;
	}

	nt->emit = emit;
	return TRUE;
}
