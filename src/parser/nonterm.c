/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	nonterm.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

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

	while( nonterminal->productions )
		pg_production_drop( (pgproduction*)list_access(
								nonterminal->productions ) );

	return pg_symbol_free( nonterminal );
}
