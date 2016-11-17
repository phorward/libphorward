/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bnf.c
Usage:	Self-hosted grammar for ppgram, generated from bnf.syn
----------------------------------------------------------------------------- */

/* DO NOT CHANGE THIS FILE MANUALLY. IT WILL GO AWAY! */

#include "phorward.h"

void pp_bnf_define( ppgram* g )
{
	ppsym*	sym[ 63 ];
	ppprod*	prod[ 73 ];
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

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "(" );

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ")" );

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 15 ]->emit = sym[ 15 ]->name;

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "*" );

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "+" );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "?" );

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "production", (char*)NULL );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emit" );

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "noemit" );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "%" );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag+", (char*)NULL );

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags?", (char*)NULL );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "|" );

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag", (char*)NULL );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "goal" );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lexeme" );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags", (char*)NULL );

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag+", (char*)NULL );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags?", (char*)NULL );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'", (char*)NULL );

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ":" );

	sym[ 42 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'?", (char*)NULL );

	sym[ 43 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ";" );

	sym[ 44 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag", (char*)NULL );

	sym[ 45 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "ignore" );

	sym[ 46 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "skip" );

	sym[ 47 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags", (char*)NULL );

	sym[ 48 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag+", (char*)NULL );

	sym[ 49 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );

	sym[ 50 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "$" );

	sym[ 51 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 52 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags?", (char*)NULL );

	sym[ 53 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	sym[ 53 ]->emit = sym[ 53 ]->name;

	sym[ 54 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitall" );

	sym[ 55 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitnone" );

	sym[ 56 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lrec" );

	sym[ 57 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "rrec" );

	sym[ 58 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );

	sym[ 59 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag+", (char*)NULL );

	sym[ 60 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 61 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 61 ];

	sym[ 62 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	prod[ 0 ] = pp_prod_create( g, sym[ 11 ] /* inline */,
		sym[ 12 ], /* "(" */
		sym[ 13 ], /* alternation */
		sym[ 14 ], /* ")" */
		(ppsym*)NULL );
	prod[ 0 ]->emit = sym[ 11 ]->name;

	prod[ 1 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 2 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 4 ], /* /\\[(\\\\.|[^\\\\\\]])*\\]/ */
		(ppsym*)NULL );

	prod[ 3 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 4 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 6 ], /* /"[^"]*"/ */
		(ppsym*)NULL );

	prod[ 5 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 7 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 6 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 10 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );

	prod[ 7 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 11 ], /* inline */
		(ppsym*)NULL );

	prod[ 8 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		sym[ 17 ], /* "*" */
		(ppsym*)NULL );
	prod[ 8 ]->emit = "kle";

	prod[ 9 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		sym[ 18 ], /* "+" */
		(ppsym*)NULL );
	prod[ 9 ]->emit = "pos";

	prod[ 10 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		sym[ 19 ], /* "?" */
		(ppsym*)NULL );
	prod[ 10 ]->emit = "opt";

	prod[ 11 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		(ppsym*)NULL );

	prod[ 12 ] = pp_prod_create( g, sym[ 20 ] /* sequence */,
		sym[ 20 ], /* sequence */
		sym[ 16 ], /* modifier */
		(ppsym*)NULL );

	prod[ 13 ] = pp_prod_create( g, sym[ 20 ] /* sequence */,
		sym[ 16 ], /* modifier */
		(ppsym*)NULL );

	prod[ 14 ] = pp_prod_create( g, sym[ 21 ] /* production */,
		sym[ 20 ], /* sequence */
		(ppsym*)NULL );

	prod[ 15 ] = pp_prod_create( g, sym[ 21 ] /* production */,
		(ppsym*)NULL );

	prod[ 16 ] = pp_prod_create( g, sym[ 22 ] /* emit */,
		sym[ 23 ], /* "emit" */
		sym[ 24 ], /* ident? */
		(ppsym*)NULL );
	prod[ 16 ]->emit = sym[ 22 ]->name;

	prod[ 17 ] = pp_prod_create( g, sym[ 24 ] /* ident? */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 18 ] = pp_prod_create( g, sym[ 24 ] /* ident? */,
		(ppsym*)NULL );

	prod[ 19 ] = pp_prod_create( g, sym[ 25 ] /* flag */,
		sym[ 26 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 19 ]->emit = sym[ 25 ]->name;

	prod[ 20 ] = pp_prod_create( g, sym[ 25 ] /* flag */,
		sym[ 22 ], /* emit */
		(ppsym*)NULL );

	prod[ 21 ] = pp_prod_create( g, sym[ 27 ] /* flags */,
		sym[ 27 ], /* flags */
		sym[ 28 ], /* "%" */
		sym[ 29 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 22 ] = pp_prod_create( g, sym[ 29 ] /* flag+ */,
		sym[ 29 ], /* flag+ */
		sym[ 25 ], /* flag */
		(ppsym*)NULL );

	prod[ 23 ] = pp_prod_create( g, sym[ 29 ] /* flag+ */,
		sym[ 25 ], /* flag */
		(ppsym*)NULL );

	prod[ 24 ] = pp_prod_create( g, sym[ 27 ] /* flags */,
		sym[ 28 ], /* "%" */
		sym[ 29 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 25 ] = pp_prod_create( g, sym[ 30 ] /* alternative */,
		sym[ 21 ], /* production */
		sym[ 31 ], /* flags? */
		(ppsym*)NULL );
	prod[ 25 ]->emit = sym[ 30 ]->name;

	prod[ 26 ] = pp_prod_create( g, sym[ 31 ] /* flags? */,
		sym[ 27 ], /* flags */
		(ppsym*)NULL );

	prod[ 27 ] = pp_prod_create( g, sym[ 31 ] /* flags? */,
		(ppsym*)NULL );

	prod[ 28 ] = pp_prod_create( g, sym[ 13 ] /* alternation */,
		sym[ 13 ], /* alternation */
		sym[ 32 ], /* "|" */
		sym[ 30 ], /* alternative */
		(ppsym*)NULL );

	prod[ 29 ] = pp_prod_create( g, sym[ 13 ] /* alternation */,
		sym[ 30 ], /* alternative */
		(ppsym*)NULL );

	prod[ 30 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 26 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 30 ]->emit = sym[ 33 ]->name;

	prod[ 31 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 34 ], /* "goal" */
		(ppsym*)NULL );
	prod[ 31 ]->emit = sym[ 33 ]->name;

	prod[ 32 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 35 ], /* "lexeme" */
		(ppsym*)NULL );
	prod[ 32 ]->emit = sym[ 33 ]->name;

	prod[ 33 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 22 ], /* emit */
		(ppsym*)NULL );

	prod[ 34 ] = pp_prod_create( g, sym[ 36 ] /* nontermflags */,
		sym[ 36 ], /* nontermflags */
		sym[ 28 ], /* "%" */
		sym[ 37 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 35 ] = pp_prod_create( g, sym[ 37 ] /* nontermflag+ */,
		sym[ 37 ], /* nontermflag+ */
		sym[ 33 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 36 ] = pp_prod_create( g, sym[ 37 ] /* nontermflag+ */,
		sym[ 33 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 37 ] = pp_prod_create( g, sym[ 36 ] /* nontermflags */,
		sym[ 28 ], /* "%" */
		sym[ 37 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 38 ] = pp_prod_create( g, sym[ 38 ] /* nontermdef */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 39 ], /* nontermflags? */
		sym[ 42 ], /* nontermdef'? */
		sym[ 43 ], /* ";" */
		(ppsym*)NULL );
	prod[ 38 ]->emit = sym[ 38 ]->name;

	prod[ 39 ] = pp_prod_create( g, sym[ 39 ] /* nontermflags? */,
		sym[ 36 ], /* nontermflags */
		(ppsym*)NULL );

	prod[ 40 ] = pp_prod_create( g, sym[ 39 ] /* nontermflags? */,
		(ppsym*)NULL );

	prod[ 41 ] = pp_prod_create( g, sym[ 40 ] /* nontermdef' */,
		sym[ 41 ], /* ":" */
		sym[ 13 ], /* alternation */
		(ppsym*)NULL );

	prod[ 42 ] = pp_prod_create( g, sym[ 42 ] /* nontermdef'? */,
		sym[ 40 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 43 ] = pp_prod_create( g, sym[ 42 ] /* nontermdef'? */,
		(ppsym*)NULL );

	prod[ 44 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 26 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 44 ]->emit = sym[ 44 ]->name;

	prod[ 45 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 45 ], /* "ignore" */
		(ppsym*)NULL );
	prod[ 45 ]->emit = sym[ 44 ]->name;

	prod[ 46 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 46 ], /* "skip" */
		(ppsym*)NULL );
	prod[ 46 ]->emit = sym[ 44 ]->name;

	prod[ 47 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 22 ], /* emit */
		(ppsym*)NULL );

	prod[ 48 ] = pp_prod_create( g, sym[ 47 ] /* termflags */,
		sym[ 47 ], /* termflags */
		sym[ 28 ], /* "%" */
		sym[ 48 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 49 ] = pp_prod_create( g, sym[ 48 ] /* termflag+ */,
		sym[ 48 ], /* termflag+ */
		sym[ 44 ], /* termflag */
		(ppsym*)NULL );

	prod[ 50 ] = pp_prod_create( g, sym[ 48 ] /* termflag+ */,
		sym[ 44 ], /* termflag */
		(ppsym*)NULL );

	prod[ 51 ] = pp_prod_create( g, sym[ 47 ] /* termflags */,
		sym[ 28 ], /* "%" */
		sym[ 48 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 52 ] = pp_prod_create( g, sym[ 49 ] /* termdef */,
		sym[ 50 ], /* "$" */
		sym[ 24 ], /* ident? */
		sym[ 51 ], /* termdef' */
		sym[ 52 ], /* termflags? */
		sym[ 43 ], /* ";" */
		(ppsym*)NULL );
	prod[ 52 ]->emit = sym[ 49 ]->name;

	prod[ 53 ] = pp_prod_create( g, sym[ 51 ] /* termdef' */,
		sym[ 4 ], /* /\\[(\\\\.|[^\\\\\\]])*\\]/ */
		(ppsym*)NULL );

	prod[ 54 ] = pp_prod_create( g, sym[ 51 ] /* termdef' */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 55 ] = pp_prod_create( g, sym[ 51 ] /* termdef' */,
		sym[ 7 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 56 ] = pp_prod_create( g, sym[ 51 ] /* termdef' */,
		sym[ 10 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );

	prod[ 57 ] = pp_prod_create( g, sym[ 52 ] /* termflags? */,
		sym[ 47 ], /* termflags */
		(ppsym*)NULL );

	prod[ 58 ] = pp_prod_create( g, sym[ 52 ] /* termflags? */,
		(ppsym*)NULL );

	prod[ 59 ] = pp_prod_create( g, sym[ 53 ] /* gflag */,
		sym[ 54 ], /* "emitall" */
		(ppsym*)NULL );

	prod[ 60 ] = pp_prod_create( g, sym[ 53 ] /* gflag */,
		sym[ 55 ], /* "emitnone" */
		(ppsym*)NULL );

	prod[ 61 ] = pp_prod_create( g, sym[ 53 ] /* gflag */,
		sym[ 56 ], /* "lrec" */
		(ppsym*)NULL );

	prod[ 62 ] = pp_prod_create( g, sym[ 53 ] /* gflag */,
		sym[ 57 ], /* "rrec" */
		(ppsym*)NULL );

	prod[ 63 ] = pp_prod_create( g, sym[ 58 ] /* gflags */,
		sym[ 58 ], /* gflags */
		sym[ 28 ], /* "%" */
		sym[ 59 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 64 ] = pp_prod_create( g, sym[ 59 ] /* gflag+ */,
		sym[ 59 ], /* gflag+ */
		sym[ 53 ], /* gflag */
		(ppsym*)NULL );

	prod[ 65 ] = pp_prod_create( g, sym[ 59 ] /* gflag+ */,
		sym[ 53 ], /* gflag */
		(ppsym*)NULL );

	prod[ 66 ] = pp_prod_create( g, sym[ 58 ] /* gflags */,
		sym[ 28 ], /* "%" */
		sym[ 59 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 67 ] = pp_prod_create( g, sym[ 60 ] /* definition */,
		sym[ 38 ], /* nontermdef */
		(ppsym*)NULL );

	prod[ 68 ] = pp_prod_create( g, sym[ 60 ] /* definition */,
		sym[ 49 ], /* termdef */
		(ppsym*)NULL );

	prod[ 69 ] = pp_prod_create( g, sym[ 60 ] /* definition */,
		sym[ 58 ], /* gflags */
		(ppsym*)NULL );

	prod[ 70 ] = pp_prod_create( g, sym[ 61 ] /* grammar */,
		sym[ 62 ], /* definition+ */
		(ppsym*)NULL );

	prod[ 71 ] = pp_prod_create( g, sym[ 62 ] /* definition+ */,
		sym[ 62 ], /* definition+ */
		sym[ 60 ], /* definition */
		(ppsym*)NULL );

	prod[ 72 ] = pp_prod_create( g, sym[ 62 ] /* definition+ */,
		sym[ 60 ], /* definition */
		(ppsym*)NULL );


}
