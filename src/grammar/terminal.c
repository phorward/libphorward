/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	terminal.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgterm* pg_term_create( pggrammar* grammar, char* name, char* pattern )
{
	pgsymbol*	sym;

	if( !( grammar && name && *name ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	if( !( sym = pg_symbol_create( grammar, PGSYMTYPE_TERMINAL, name ) ) )
		return (pgterm*)NULL;

	if( pattern )
		pg_term_parse_pattern( sym, pattern );
	else
		pg_term_set_pattern( sym,
			pregex_ptn_create_string( name, PREGEX_MOD_NONE ) );

	return sym;
}

/* Destructor */

pgterm* pg_term_drop( pgterm* terminal )
{
	if( !terminal )
		return (pgterm*)NULL;

	if( !pg_symbol_is_term( terminal ) )
	{
		WRONGPARAM;
		return (pgterm*)NULL;
	}

	pg_term_set_pattern( terminal, (pregex_ptn*)NULL );

	return pg_symbol_free( terminal );
}

/* Retrieval */

pgterm* pg_term_get( pggrammar* g, int offset )
{
	int			i;
	pgsymbol*	s;

	if( !( g && offset >= 0 ) )
	{
		WRONGPARAM;
		return (pgterm*)NULL;
	}

	for( i = 0; s = pg_symbol_get( g, i ); i++ )
	{
		if( pg_symbol_get_type( s ) == PGSYMTYPE_TERMINAL )
		{
			if( !offset )
				return (pgterm*)s;

			offset--;
		}
	}

	return (pgterm*)NULL;
}

/* Attribute: pattern */

BOOLEAN pg_term_parse_pattern( pgterm* terminal, char* pattern )
{
	pregex_ptn*		ptn		= (pregex_ptn*)NULL;

	if( !pg_symbol_is_term( terminal ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( pattern )
	{
		if( pregex_ptn_parse( &ptn, pattern, PREGEX_MOD_NONE ) != ERR_OK )
			return FALSE;
	}

	pg_term_set_pattern( terminal, ptn );

	return TRUE;
}

BOOLEAN pg_term_set_pattern( pgterm* terminal, pregex_ptn* ptn )
{
	if( !pg_symbol_is_term( terminal ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( terminal->ptn )
		pregex_ptn_free( terminal->ptn );

	if( !ptn->accept )
		ptn->accept = (pregex_accept*)pmalloc( sizeof( pregex_accept ) );

	terminal->ptn = ptn;

	return TRUE;
}

pregex_ptn* pg_term_get_pattern( pgterm* terminal )
{
	if( !pg_symbol_is_term( terminal ) )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	return terminal->ptn;
}
