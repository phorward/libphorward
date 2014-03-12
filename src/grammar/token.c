/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	token.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pgtoken* pg_token_create( pgsymbol* sym, pgvalue* lexem )
{
	pgtoken*	tok;

	tok = (pgtoken*)pmalloc( sizeof( pgtoken ) );
	tok->symbol = sym;

	if( lexem )
		pg_token_set_lexem( tok, lexem );

	return tok;
}

pboolean pg_token_reset( pgtoken* tok )
{
	if( !tok )
	{
		WRONGPARAM;
		return FALSE;
	}

	tok->symbol = (pgsymbol*)NULL;
	tok->lexem = pfree( tok->lexem );

	return TRUE;
}

pgtoken* pg_token_free( pgtoken* tok )
{
	if( !tok )
		return (pgtoken*)NULL;

	pg_token_reset( tok );
	pfree( tok );

	return (pgtoken*)NULL;
}

void pg_token_print( pgtoken* tok )
{
	fprintf( stderr, "%d:%s >%s<\n",
				pg_symbol_get_id( tok->symbol ),
					pg_symbol_get_name( tok->symbol ),
						pg_token_get_lexem( tok ) );
}

/* GET & SET */

pboolean pg_token_set_symbol( pgtoken* tok, pgsymbol* symbol )
{
	if( !tok )
	{
		WRONGPARAM;
		return FALSE;
	}

	tok->symbol = symbol;
	return TRUE;
}

pgsymbol* pg_token_get_symbol( pgtoken* tok )
{
	if( !tok )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	return tok->symbol;
}

/*
	Attribute: Lexem
*/

pboolean pg_token_set_lexem( pgtoken* tok, pgvalue* lexem )
{
	if( !tok )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( tok->lexem )
		pg_value_free( tok->lexem );

	tok->lexem = lexem;
	return TRUE;
}

pgvalue* pg_token_get_lexem( pgtoken* tok )
{
	if( !tok )
	{
		WRONGPARAM;
		return (pgvalue*)NULL;
	}

	return tok->lexem;
}

/*
	Attribute: Wlexem
*/
/*
pboolean pg_token_set_wlexem( pgtoken* tok, wchar_t* lexem )
{
	if( !tok )
	{
		WRONGPARAM;
		return FALSE;
	}

	tok->flags |= PG_TOKFLAG_WCHAR;
	tok->lexem = (char*)lexem;
	return TRUE;
}

wchar_t* pg_token_get_wlexem( pgtoken* tok )
{
	if( !tok )
	{
		WRONGPARAM;
		return (wchar_t*)NULL;
	}

	if( !( tok->flags & PG_TOKFLAG_WCHAR ) )
		return L"";

	return (wchar_t*)pgetstr( tok->lexem );
}

*/
