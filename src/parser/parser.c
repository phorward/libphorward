/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parser.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgparser* pg_parser_create( pggrammar* grammar, pgparadigm paradigm )
{
	pgparser*	parser;

	if( !( grammar && ( paradigm > PGPARADIGM_UNDEFINED
							&& paradigm < PGPARADIGM_EOP ) ) )
	{
		WRONGPARAM;
		return (pgparser*)NULL;
	}

	parser = (pgparser*)pmalloc( sizeof( pgparser ) );
	parser->grammar = grammar;
	parser->paradigm = paradigm;

	parser->optimize = TRUE;

	if( !pg_parser_generate( parser ) )
		return parser;

	parser->lexer = pg_lexer_create( parser );
	return parser;
}

/* Destructor */

pgparser* pg_parser_free( pgparser* parser )
{
	if( !( parser ) )
		return (pgparser*)NULL;

	pfree( parser );

	return (pgparser*)NULL;
}

/* Generate */

BOOLEAN pg_parser_generate( pgparser* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( pg_parser_is_lr( p ) )
		return pg_parser_lr_closure( p );
	else if( pg_parser_is_ll( p ) )
		return pg_parser_ll_closure( p );
	else
		MISSINGCASE;

	return FALSE;
}

/* Run */

BOOLEAN pg_parser_parse( pgparser* p )
{
	if( pg_parser_is_lr( p ) )
		return pg_parser_lr_parse( p );
	else if( pg_parser_is_ll( p ) )
		return pg_parser_ll_parse( p );
	else
		MISSINGCASE;

	return FALSE;
}

/* Check */

/** Check if parser is bottom-up? */
BOOLEAN pg_parser_is_lr( pgparser* p )
{
	if( p && ( p->paradigm == PGPARADIGM_LR0
				|| p->paradigm == PGPARADIGM_SLR1
					|| p->paradigm == PGPARADIGM_LR1
						|| p->paradigm == PGPARADIGM_LALR1 ) )
		return TRUE;

	return FALSE;
}

/** Check if parser is top-down? */
BOOLEAN pg_parser_is_ll( pgparser* p )
{
	if( p && ( p->paradigm == PGPARADIGM_LL1 ) )
		return TRUE;

	return FALSE;
}

/* Attribute: Grammar */

/* GET ONLY! */
pggrammar* pg_parser_get_grammar( pgparser* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (pggrammar*)NULL;
	}

	return p->grammar;
}

/* Attribute: optimize */

pboolean pg_parser_get_optimize( pgparser* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return p->optimize;
}

pboolean pg_parser_set_optimize( pgparser* p, pboolean optimize )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( p->optimize == optimize )
		return FALSE;

	p->optimize = optimize;
	return TRUE;
}

/* Attribute: source */

char* pg_parser_get_source( pgparser* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return p->source;
}

pboolean pg_parser_set_source( pgparser* p, char* source )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	psetstr( &( p->source ), source );
	return TRUE;
}
