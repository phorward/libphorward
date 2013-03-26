/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parser.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

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


