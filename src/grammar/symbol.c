/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	grammar.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgsymbol* pg_symbol_create( pggrammar* grammar, pgsymtype type, char* name )
{
	pgsymbol	sym;
	plistel*	e;
	char*		symname;

	PROC( "pg_symbol_create" );
	PARMS( "type", "%d", type );
	PARMS( "name", "%s", name );

	if( !( grammar && name && *name ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	if( plist_get_by_key( grammar->symbols, name ) )
	{
		TODO;
		fprintf( stderr, "Symbol '%s' already exists in this grammar\n", name );
		return (pgsymbol*)NULL;
	}

	memset( &sym, 0, sizeof( pgsymbol ) );
	sym.type = type;
	sym.grammar = grammar;
	sym.id = plist_count( grammar->symbols );
	sym.name = pstrdup( name );

	e = plist_insert( grammar->symbols, (plistel*)NULL, sym.name, (pbyte*)&sym );

	return (pgsymbol*)plist_access( e );
}

/* Destructor */

pgsymbol* pg_symbol_free( pgsymbol* symbol )
{
	plistel*		se;
	plistel*		nse;
	pgsymbol*	move;

	if( !( symbol ) )
		return (pgsymbol*)NULL;

	/* Find unit in symbol table */
	if( !( nse = se = plist_get_by_ptr( symbol->grammar->symbols, symbol ) ) )
		return symbol;

	/* Move up all symbols from here */
	while( ( nse = plist_next( nse ) ) )
	{
		move = (pgsymbol*)plist_access( nse );
		move->id--;
	}

	/* Free members */
	pfree( symbol->name );


	/* --- Other dependencies --- */

		/* End-Of-Input */
	if( pg_grammar_get_eoi( symbol->grammar ) == symbol )
		pg_grammar_set_eoi( symbol->grammar, (pgterminal*)NULL );

		/* Goal */
	if( pg_grammar_get_goal( symbol->grammar ) == symbol )
		pg_grammar_set_goal( symbol->grammar, (pgterminal*)NULL );

	/* Remove from symbol list */
	plist_remove( symbol->grammar->symbols, se );

	return (pgsymbol*)NULL;
}

BOOLEAN pg_symbol_reset( pgsymbol* s )
{
	if( !( s ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	s->first = list_free( s->first );
	s->follow = list_free( s->follow );

	return TRUE;
}

/* Debug */

void pg_symbol_print( pgsymbol* symbol, FILE* f )
{
	if( !( symbol ) )
	{
		WRONGPARAM;
		return;
	}

	if( !f )
		f = stderr;

	fprintf( f, "%s%s", pg_symbol_is_terminal( symbol ) ? "@" : "",
							pg_symbol_get_name( symbol ) );
}

/* Check */

/** Check if //symbol// is of type **PGSYMTYPE_TERMINAL**. */
BOOLEAN pg_symbol_is_terminal( pgsymbol* symbol )
{
	if( symbol && pg_symbol_get_type( symbol ) == PGSYMTYPE_TERMINAL  )
		return TRUE;

	return FALSE;
}

/** Check if //symbol// is of type **PGSYMTYPE_NONTERMINAL**. */
BOOLEAN pg_symbol_is_nonterminal( pgsymbol* symbol )
{
	if( symbol && pg_symbol_get_type( symbol ) == PGSYMTYPE_NONTERMINAL  )
		return TRUE;

	return FALSE;
}

/* Retrieve: By offset */

pgsymbol* pg_symbol_get( pggrammar* g, int i )
{
	if( !( g && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	return (pgsymbol*)plist_access( plist_get( g->symbols, i ) );
}

/* Attribute: id */

/* GET ONLY! */
int pg_symbol_get_id( pgsymbol* s )
{
	if( !( s ) )
	{
		WRONGPARAM;
		return -1;
	}

	return plist_offset( plist_get_by_ptr( s->grammar->symbols, s ) );
}

/* Attribute: type */

/* GET ONLY! */
pgsymtype pg_symbol_get_type( pgsymbol* s )
{
	if( !( s ) )
	{
		WRONGPARAM;
		return PGSYMTYPE_UNDEFINED;
	}

	return s->type;
}

/* Attribute: grammar */

/* GET ONLY! */
pggrammar* pg_symbol_get_grammar( pgsymbol* s )
{
	if( !( s ) )
	{
		WRONGPARAM;
		return (pggrammar*)NULL;
	}

	return s->grammar;
}


/* Attribute: name */

/* GET ONLY! */
char* pg_symbol_get_name( pgsymbol* s )
{
	if( !( s ) )
	{
		WRONGPARAM;
		return "";
	}

	return s->name;
}
