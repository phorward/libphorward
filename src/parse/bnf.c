/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bnf.c
Usage:	Self-hosted grammar for ppgram, generated from bnf.syn
----------------------------------------------------------------------------- */

/* DO NOT CHANGE THIS FILE MANUALLY. IT WILL GO AWAY! */

#include "phorward.h"

void pp_bnf_define( ppgram* g )
{
	ppsym*	sym[ 42 ];
	ppprod*	prod[ 43 ];
	/* Symbols */

	sym[ 1 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "[\\t\\n\\r ]+" );
	sym[ 1 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 2 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "//[^\\n]*\\n" );
	sym[ 2 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 3 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "/\\*([^*]|\\*[^/])*\\*/" );
	sym[ 3 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 4 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ccl", "\\[(\\\\.|[^\\\\\\]])*\\]" );
	sym[ 4 ]->emit = sym[ 4 ]->name;

	sym[ 5 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "string", "'[^']*'" );
	sym[ 5 ]->emit = sym[ 5 ]->name;

	sym[ 6 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "token", "\"[^\"]*\"" );
	sym[ 6 ]->emit = sym[ 6 ]->name;

	sym[ 7 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "regex", "/(\\\\.|[^/\\\\])*/" );
	sym[ 7 ]->emit = sym[ 7 ]->name;

	sym[ 8 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ident", "[A-Z_a-z][0-9A-Z_a-z]*" );
	sym[ 8 ]->emit = sym[ 8 ]->name;

	sym[ 9 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "int", "[0-9]+" );
	sym[ 9 ]->emit = sym[ 9 ]->name;

	sym[ 10 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "function", "[A-Z_a-z][0-9A-Z_a-z]*\\(\\)" );
	sym[ 10 ]->emit = sym[ 10 ]->name;

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_STRING, "flag_emit", "@" );
	sym[ 11 ]->emit = sym[ 11 ]->name;

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_STRING, "flag_goal", "$" );
	sym[ 12 ]->emit = sym[ 12 ]->name;

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_STRING, "flag_lexem", "!" );
	sym[ 13 ]->emit = sym[ 13 ]->name;

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "flag_ignore", "%(ignore|skip)" );
	sym[ 14 ]->emit = sym[ 14 ]->name;

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );
	sym[ 15 ]->emit = sym[ 15 ]->name;

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "(" );

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ")" );

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 20 ]->emit = sym[ 20 ]->name;

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "*" );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "+" );

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "?" );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "production", (char*)NULL );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );
	sym[ 27 ]->emit = sym[ 27 ]->name;

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "|" );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );
	sym[ 29 ]->emit = sym[ 29 ]->name;

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag_emit?", (char*)NULL );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'", (char*)NULL );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'+", (char*)NULL );

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'?", (char*)NULL );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ":" );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ";" );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );
	sym[ 36 ]->emit = sym[ 36 ]->name;

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef''", (char*)NULL );

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 40 ];

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	prod[ 0 ] = pp_prod_create( g, sym[ 15 ] /* inline */,
		sym[ 11 ], /* "@" */
		sym[ 16 ], /* ident? */
		sym[ 17 ], /* "(" */
		sym[ 18 ], /* alternation */
		sym[ 19 ], /* ")" */
		(ppsym*)NULL );

	prod[ 1 ] = pp_prod_create( g, sym[ 16 ] /* ident? */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 2 ] = pp_prod_create( g, sym[ 16 ] /* ident? */,
		(ppsym*)NULL );

	prod[ 3 ] = pp_prod_create( g, sym[ 15 ] /* inline */,
		sym[ 17 ], /* "(" */
		sym[ 18 ], /* alternation */
		sym[ 19 ], /* ")" */
		(ppsym*)NULL );

	prod[ 4 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 5 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 4 ], /* /\\[(\\\\.|[^\\\\\\]])*\\]/ */
		(ppsym*)NULL );

	prod[ 6 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 7 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 6 ], /* /"[^"]*"/ */
		(ppsym*)NULL );

	prod[ 8 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 7 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 9 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 10 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );

	prod[ 10 ] = pp_prod_create( g, sym[ 20 ] /* symbol */,
		sym[ 15 ], /* inline */
		(ppsym*)NULL );

	prod[ 11 ] = pp_prod_create( g, sym[ 21 ] /* modifier */,
		sym[ 20 ], /* symbol */
		sym[ 22 ], /* "*" */
		(ppsym*)NULL );
	prod[ 11 ]->emit = "kle";

	prod[ 12 ] = pp_prod_create( g, sym[ 21 ] /* modifier */,
		sym[ 20 ], /* symbol */
		sym[ 23 ], /* "+" */
		(ppsym*)NULL );
	prod[ 12 ]->emit = "pos";

	prod[ 13 ] = pp_prod_create( g, sym[ 21 ] /* modifier */,
		sym[ 20 ], /* symbol */
		sym[ 24 ], /* "?" */
		(ppsym*)NULL );
	prod[ 13 ]->emit = "opt";

	prod[ 14 ] = pp_prod_create( g, sym[ 21 ] /* modifier */,
		sym[ 20 ], /* symbol */
		(ppsym*)NULL );

	prod[ 15 ] = pp_prod_create( g, sym[ 25 ] /* sequence */,
		sym[ 25 ], /* sequence */
		sym[ 21 ], /* modifier */
		(ppsym*)NULL );

	prod[ 16 ] = pp_prod_create( g, sym[ 25 ] /* sequence */,
		sym[ 21 ], /* modifier */
		(ppsym*)NULL );

	prod[ 17 ] = pp_prod_create( g, sym[ 26 ] /* production */,
		sym[ 25 ], /* sequence */
		(ppsym*)NULL );

	prod[ 18 ] = pp_prod_create( g, sym[ 26 ] /* production */,
		(ppsym*)NULL );

	prod[ 19 ] = pp_prod_create( g, sym[ 27 ] /* alternative */,
		sym[ 26 ], /* production */
		(ppsym*)NULL );

	prod[ 20 ] = pp_prod_create( g, sym[ 18 ] /* alternation */,
		sym[ 18 ], /* alternation */
		sym[ 28 ], /* "|" */
		sym[ 27 ], /* alternative */
		(ppsym*)NULL );

	prod[ 21 ] = pp_prod_create( g, sym[ 18 ] /* alternation */,
		sym[ 27 ], /* alternative */
		(ppsym*)NULL );

	prod[ 22 ] = pp_prod_create( g, sym[ 29 ] /* nontermdef */,
		sym[ 30 ], /* flag_emit? */
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 33 ], /* nontermdef'? */
		sym[ 34 ], /* ":" */
		sym[ 18 ], /* alternation */
		sym[ 35 ], /* ";" */
		(ppsym*)NULL );

	prod[ 23 ] = pp_prod_create( g, sym[ 30 ] /* flag_emit? */,
		sym[ 11 ], /* "@" */
		(ppsym*)NULL );

	prod[ 24 ] = pp_prod_create( g, sym[ 30 ] /* flag_emit? */,
		(ppsym*)NULL );

	prod[ 25 ] = pp_prod_create( g, sym[ 31 ] /* nontermdef' */,
		sym[ 12 ], /* "$" */
		(ppsym*)NULL );

	prod[ 26 ] = pp_prod_create( g, sym[ 31 ] /* nontermdef' */,
		sym[ 13 ], /* "!" */
		(ppsym*)NULL );

	prod[ 27 ] = pp_prod_create( g, sym[ 32 ] /* nontermdef'+ */,
		sym[ 32 ], /* nontermdef'+ */
		sym[ 31 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 28 ] = pp_prod_create( g, sym[ 32 ] /* nontermdef'+ */,
		sym[ 31 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 29 ] = pp_prod_create( g, sym[ 33 ] /* nontermdef'? */,
		sym[ 32 ], /* nontermdef'+ */
		(ppsym*)NULL );

	prod[ 30 ] = pp_prod_create( g, sym[ 33 ] /* nontermdef'? */,
		(ppsym*)NULL );

	prod[ 31 ] = pp_prod_create( g, sym[ 36 ] /* termdef */,
		sym[ 37 ], /* termdef' */
		sym[ 38 ], /* termdef'' */
		sym[ 35 ], /* ";" */
		(ppsym*)NULL );

	prod[ 32 ] = pp_prod_create( g, sym[ 37 ] /* termdef' */,
		sym[ 30 ], /* flag_emit? */
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 33 ] = pp_prod_create( g, sym[ 37 ] /* termdef' */,
		sym[ 14 ], /* /%(ignore|skip)/ */
		(ppsym*)NULL );

	prod[ 34 ] = pp_prod_create( g, sym[ 38 ] /* termdef'' */,
		sym[ 4 ], /* /\\[(\\\\.|[^\\\\\\]])*\\]/ */
		(ppsym*)NULL );

	prod[ 35 ] = pp_prod_create( g, sym[ 38 ] /* termdef'' */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 36 ] = pp_prod_create( g, sym[ 38 ] /* termdef'' */,
		sym[ 7 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 37 ] = pp_prod_create( g, sym[ 38 ] /* termdef'' */,
		sym[ 10 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );

	prod[ 38 ] = pp_prod_create( g, sym[ 39 ] /* definition */,
		sym[ 29 ], /* nontermdef */
		(ppsym*)NULL );

	prod[ 39 ] = pp_prod_create( g, sym[ 39 ] /* definition */,
		sym[ 36 ], /* termdef */
		(ppsym*)NULL );

	prod[ 40 ] = pp_prod_create( g, sym[ 40 ] /* grammar */,
		sym[ 41 ], /* definition+ */
		(ppsym*)NULL );

	prod[ 41 ] = pp_prod_create( g, sym[ 41 ] /* definition+ */,
		sym[ 41 ], /* definition+ */
		sym[ 39 ], /* definition */
		(ppsym*)NULL );

	prod[ 42 ] = pp_prod_create( g, sym[ 41 ] /* definition+ */,
		sym[ 39 ], /* definition */
		(ppsym*)NULL );


}
