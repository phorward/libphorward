/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bnf.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

static void bnf_grammar( pggrammar* g )
{
	#include "bnfgen.h"
	pg_grammar_set_goal( g, N_grammar );
}

void pg_grammar_from_bnf( void )
{
	pggrammar*	g;
	pgparser*	p;

	g = pg_grammar_create();
	bnf_grammar( g );

	pg_grammar_print( g );

	p = pg_parser_create( g, PGPARADIGM_LALR1 );

	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING,
							"@ident /A-Za-z+/;"
							"start: @ident;"
							"test: start @ident;" );

	pg_parser_parse( p );
}
