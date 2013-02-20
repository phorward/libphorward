/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See \LICENSE for more information.

File:	grammar.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

pggrammar* pg_grammar_create( void )
{
	pggrammar*	g;

	g = (pggrammar*)pmalloc( sizeof( pggrammar ) );

	g->symbols = plist_create( sizeof( pgsymbol ),
						PLIST_MOD_RECYCLE | PLIST_MOD_UNIQUE |
							PLIST_MOD_EXTKEYS );

	return g;
}

pggrammar* pg_grammar_free( pggrammar* g )
{
	if( !( g ) )
		return (pggrammar*)NULL;

	g->symbols = plist_free( g->symbols );
	TODO;
	pfree( g );

	return (pggrammar*)NULL;
}

/* Debug */

void pg_grammar_print( pggrammar* g )
{
	LIST*			l;
	pgproduction*	p;

	LISTFOR( g->productions, l )
	{
		p = (pgproduction*)list_access( l );
		printf( "% 2d %s\n", pg_production_get_id( p ),
								pg_production_to_string( p ) );
	}
}

