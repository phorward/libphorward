/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	terminal.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

/* Constructor */

pgterminal* pg_terminal_create( pggrammar* grammar, char* name, char* pattern )
{
	pgsymbol*	sym;

	if( !( grammar && name && *name ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	if( !( sym = pg_symbol_create( grammar, PGSYMTYPE_TERMINAL, name ) ) )
		return (pgterminal*)NULL;

	pg_terminal_parse_pattern( sym, pattern );

	return sym;
}

/* Destructor */

pgterminal* pg_terminal_drop( pgterminal* terminal )
{
	if( !terminal )
		return (pgterminal*)NULL;

	if( !pg_symbol_is_terminal( terminal ) )
	{
		WRONGPARAM;
		return (pgterminal*)NULL;
	}

	pg_terminal_set_pattern( terminal, (pregex_ptn*)NULL );

	return pg_symbol_free( terminal );
}

/* Retrieval */

pgterminal* pg_terminal_get( pggrammar* g, int offset )
{
	int			i;
	pgsymbol*	s;

	if( !( g && offset >= 0 ) )
	{
		WRONGPARAM;
		return (pgterminal*)NULL;
	}

	for( i = 0; s = pg_symbol_get( g, i ); i++ )
	{
		if( pg_symbol_get_type( s ) == PGSYMTYPE_TERMINAL )
		{
			if( !offset )
				return (pgterminal*)s;

			offset--;
		}
	}

	return (pgterminal*)NULL;
}

/* Attribute: pattern */

BOOLEAN pg_terminal_parse_pattern( pgterminal* terminal, uchar* pattern )
{
	pregex_ptn*		ptn		= (pregex_ptn*)NULL;

	if( !pg_symbol_is_terminal( terminal ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( pattern )
	{
		if( pregex_ptn_parse( &ptn, (pregex_accept*)NULL, pattern,
				PREGEX_MOD_NONE ) != ERR_OK )
		{
			return FALSE;
		}
	}

	pg_terminal_set_pattern( terminal, ptn );

	return TRUE;
}

BOOLEAN pg_terminal_set_pattern( pgterminal* terminal, pregex_ptn* ptn )
{
	if( !pg_symbol_is_terminal( terminal ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( terminal->ptn )
		pregex_ptn_free( terminal->ptn );

	terminal->ptn = ptn;

	return TRUE;
}

pregex_ptn* pg_terminal_get_pattern( pgterminal* terminal )
{
	if( !pg_symbol_is_terminal( terminal ) )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	return terminal->ptn;
}
