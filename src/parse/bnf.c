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
	ppsym*	sym[ 65 ];
	ppprod*	prod[ 77 ];
	/* Symbols */

	sym[ 1 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "[\\t\\n\\r ]+" );
	sym[ 1 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 2 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "//[^\\n]*\\n" );
	sym[ 2 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 3 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "/\\*([^*]|\\*[^/])*\\*/" );
	sym[ 3 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 4 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ccl", "\\[(\\\\.|[^\\\\\\]])*\\]" );
	sym[ 4 ]->emit = 1;

	sym[ 5 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "string", "'[^']*'" );
	sym[ 5 ]->emit = 2;

	sym[ 6 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "token", "\"[^\"]*\"" );
	sym[ 6 ]->emit = 9;

	sym[ 7 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "regex", "/(\\\\.|[^/\\\\])*/" );
	sym[ 7 ]->emit = 3;

	sym[ 8 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ident", "[A-Z_a-z][0-9A-Z_a-z]*" );
	sym[ 8 ]->emit = 10;

	sym[ 9 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "int", "[0-9]+" );
	sym[ 9 ]->emit = 11;

	sym[ 10 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "function", "[A-Z_a-z][0-9A-Z_a-z]*\\(\\)" );
	sym[ 10 ]->emit = 4;

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "(" );

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ")" );

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 15 ]->emit = 20;

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "*" );

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "+" );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "?" );

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "production", (char*)NULL );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emit" );

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit'", (char*)NULL );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit'?", (char*)NULL );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "noemit" );

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "%" );

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag+", (char*)NULL );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags?", (char*)NULL );

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "|" );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag", (char*)NULL );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "goal" );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lexeme" );

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags", (char*)NULL );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag+", (char*)NULL );

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags?", (char*)NULL );

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'", (char*)NULL );

	sym[ 42 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ":" );

	sym[ 43 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'?", (char*)NULL );

	sym[ 44 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, ";" );

	sym[ 45 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag", (char*)NULL );

	sym[ 46 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "ignore" );

	sym[ 47 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "skip" );

	sym[ 48 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags", (char*)NULL );

	sym[ 49 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag+", (char*)NULL );

	sym[ 50 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );

	sym[ 51 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "$" );

	sym[ 52 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 53 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 54 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags?", (char*)NULL );

	sym[ 55 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	sym[ 55 ]->emit = 41;

	sym[ 56 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitall" );

	sym[ 57 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitnone" );

	sym[ 58 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lrec" );

	sym[ 59 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "rrec" );

	sym[ 60 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );

	sym[ 61 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag+", (char*)NULL );

	sym[ 62 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 63 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 63 ];

	sym[ 64 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	prod[ 0 ] = pp_prod_create( g, sym[ 11 ] /* inline */,
		sym[ 12 ], /* "(" */
		sym[ 13 ], /* alternation */
		sym[ 14 ], /* ")" */
		(ppsym*)NULL );
	prod[ 0 ]->emit = 32;

	prod[ 1 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );
	prod[ 1 ]->emit = 20;

	prod[ 2 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 4 ], /* /\\[(\\\\.|[^\\\\\\]])*\\]/ */
		(ppsym*)NULL );
	prod[ 2 ]->emit = 20;

	prod[ 3 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );
	prod[ 3 ]->emit = 20;

	prod[ 4 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 6 ], /* /"[^"]*"/ */
		(ppsym*)NULL );
	prod[ 4 ]->emit = 20;

	prod[ 5 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 7 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );
	prod[ 5 ]->emit = 20;

	prod[ 6 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 10 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );
	prod[ 6 ]->emit = 20;

	prod[ 7 ] = pp_prod_create( g, sym[ 15 ] /* symbol */,
		sym[ 11 ], /* inline */
		(ppsym*)NULL );
	prod[ 7 ]->emit = 20;

	prod[ 8 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		sym[ 17 ], /* "*" */
		(ppsym*)NULL );
	prod[ 8 ]->emit = 25;

	prod[ 9 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		sym[ 18 ], /* "+" */
		(ppsym*)NULL );
	prod[ 9 ]->emit = 26;

	prod[ 10 ] = pp_prod_create( g, sym[ 16 ] /* modifier */,
		sym[ 15 ], /* symbol */
		sym[ 19 ], /* "?" */
		(ppsym*)NULL );
	prod[ 10 ]->emit = 27;

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

	prod[ 16 ] = pp_prod_create( g, sym[ 24 ] /* emit' */,
		sym[ 9 ], /* /[0-9]+/ */
		(ppsym*)NULL );

	prod[ 17 ] = pp_prod_create( g, sym[ 24 ] /* emit' */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 18 ] = pp_prod_create( g, sym[ 25 ] /* emit'? */,
		sym[ 24 ], /* emit' */
		(ppsym*)NULL );

	prod[ 19 ] = pp_prod_create( g, sym[ 25 ] /* emit'? */,
		(ppsym*)NULL );

	prod[ 20 ] = pp_prod_create( g, sym[ 22 ] /* emit */,
		sym[ 23 ], /* "emit" */
		sym[ 25 ], /* emit'? */
		(ppsym*)NULL );
	prod[ 20 ]->emit = 42;

	prod[ 21 ] = pp_prod_create( g, sym[ 26 ] /* flag */,
		sym[ 27 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 21 ]->emit = 40;

	prod[ 22 ] = pp_prod_create( g, sym[ 26 ] /* flag */,
		sym[ 22 ], /* emit */
		(ppsym*)NULL );

	prod[ 23 ] = pp_prod_create( g, sym[ 30 ] /* flag+ */,
		sym[ 30 ], /* flag+ */
		sym[ 26 ], /* flag */
		(ppsym*)NULL );

	prod[ 24 ] = pp_prod_create( g, sym[ 30 ] /* flag+ */,
		sym[ 26 ], /* flag */
		(ppsym*)NULL );

	prod[ 25 ] = pp_prod_create( g, sym[ 28 ] /* flags */,
		sym[ 28 ], /* flags */
		sym[ 29 ], /* "%" */
		sym[ 30 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 26 ] = pp_prod_create( g, sym[ 28 ] /* flags */,
		sym[ 29 ], /* "%" */
		sym[ 30 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 27 ] = pp_prod_create( g, sym[ 32 ] /* flags? */,
		sym[ 28 ], /* flags */
		(ppsym*)NULL );

	prod[ 28 ] = pp_prod_create( g, sym[ 32 ] /* flags? */,
		(ppsym*)NULL );

	prod[ 29 ] = pp_prod_create( g, sym[ 31 ] /* alternative */,
		sym[ 21 ], /* production */
		sym[ 32 ], /* flags? */
		(ppsym*)NULL );
	prod[ 29 ]->emit = 30;

	prod[ 30 ] = pp_prod_create( g, sym[ 13 ] /* alternation */,
		sym[ 13 ], /* alternation */
		sym[ 33 ], /* "|" */
		sym[ 31 ], /* alternative */
		(ppsym*)NULL );

	prod[ 31 ] = pp_prod_create( g, sym[ 13 ] /* alternation */,
		sym[ 31 ], /* alternative */
		(ppsym*)NULL );

	prod[ 32 ] = pp_prod_create( g, sym[ 34 ] /* nontermflag */,
		sym[ 27 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 32 ]->emit = 40;

	prod[ 33 ] = pp_prod_create( g, sym[ 34 ] /* nontermflag */,
		sym[ 35 ], /* "goal" */
		(ppsym*)NULL );
	prod[ 33 ]->emit = 40;

	prod[ 34 ] = pp_prod_create( g, sym[ 34 ] /* nontermflag */,
		sym[ 36 ], /* "lexeme" */
		(ppsym*)NULL );
	prod[ 34 ]->emit = 40;

	prod[ 35 ] = pp_prod_create( g, sym[ 34 ] /* nontermflag */,
		sym[ 22 ], /* emit */
		(ppsym*)NULL );

	prod[ 36 ] = pp_prod_create( g, sym[ 38 ] /* nontermflag+ */,
		sym[ 38 ], /* nontermflag+ */
		sym[ 34 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 37 ] = pp_prod_create( g, sym[ 38 ] /* nontermflag+ */,
		sym[ 34 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 38 ] = pp_prod_create( g, sym[ 37 ] /* nontermflags */,
		sym[ 37 ], /* nontermflags */
		sym[ 29 ], /* "%" */
		sym[ 38 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 39 ] = pp_prod_create( g, sym[ 37 ] /* nontermflags */,
		sym[ 29 ], /* "%" */
		sym[ 38 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 40 ] = pp_prod_create( g, sym[ 40 ] /* nontermflags? */,
		sym[ 37 ], /* nontermflags */
		(ppsym*)NULL );

	prod[ 41 ] = pp_prod_create( g, sym[ 40 ] /* nontermflags? */,
		(ppsym*)NULL );

	prod[ 42 ] = pp_prod_create( g, sym[ 41 ] /* nontermdef' */,
		sym[ 42 ], /* ":" */
		sym[ 13 ], /* alternation */
		(ppsym*)NULL );

	prod[ 43 ] = pp_prod_create( g, sym[ 43 ] /* nontermdef'? */,
		sym[ 41 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 44 ] = pp_prod_create( g, sym[ 43 ] /* nontermdef'? */,
		(ppsym*)NULL );

	prod[ 45 ] = pp_prod_create( g, sym[ 39 ] /* nontermdef */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 40 ], /* nontermflags? */
		sym[ 43 ], /* nontermdef'? */
		sym[ 44 ], /* ";" */
		(ppsym*)NULL );
	prod[ 45 ]->emit = 31;

	prod[ 46 ] = pp_prod_create( g, sym[ 45 ] /* termflag */,
		sym[ 27 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 46 ]->emit = 40;

	prod[ 47 ] = pp_prod_create( g, sym[ 45 ] /* termflag */,
		sym[ 46 ], /* "ignore" */
		(ppsym*)NULL );
	prod[ 47 ]->emit = 40;

	prod[ 48 ] = pp_prod_create( g, sym[ 45 ] /* termflag */,
		sym[ 47 ], /* "skip" */
		(ppsym*)NULL );
	prod[ 48 ]->emit = 40;

	prod[ 49 ] = pp_prod_create( g, sym[ 45 ] /* termflag */,
		sym[ 22 ], /* emit */
		(ppsym*)NULL );

	prod[ 50 ] = pp_prod_create( g, sym[ 49 ] /* termflag+ */,
		sym[ 49 ], /* termflag+ */
		sym[ 45 ], /* termflag */
		(ppsym*)NULL );

	prod[ 51 ] = pp_prod_create( g, sym[ 49 ] /* termflag+ */,
		sym[ 45 ], /* termflag */
		(ppsym*)NULL );

	prod[ 52 ] = pp_prod_create( g, sym[ 48 ] /* termflags */,
		sym[ 48 ], /* termflags */
		sym[ 29 ], /* "%" */
		sym[ 49 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 53 ] = pp_prod_create( g, sym[ 48 ] /* termflags */,
		sym[ 29 ], /* "%" */
		sym[ 49 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 54 ] = pp_prod_create( g, sym[ 52 ] /* ident? */,
		sym[ 8 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 55 ] = pp_prod_create( g, sym[ 52 ] /* ident? */,
		(ppsym*)NULL );

	prod[ 56 ] = pp_prod_create( g, sym[ 53 ] /* termdef' */,
		sym[ 4 ], /* /\\[(\\\\.|[^\\\\\\]])*\\]/ */
		(ppsym*)NULL );

	prod[ 57 ] = pp_prod_create( g, sym[ 53 ] /* termdef' */,
		sym[ 5 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 58 ] = pp_prod_create( g, sym[ 53 ] /* termdef' */,
		sym[ 7 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 59 ] = pp_prod_create( g, sym[ 53 ] /* termdef' */,
		sym[ 10 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );

	prod[ 60 ] = pp_prod_create( g, sym[ 54 ] /* termflags? */,
		sym[ 48 ], /* termflags */
		(ppsym*)NULL );

	prod[ 61 ] = pp_prod_create( g, sym[ 54 ] /* termflags? */,
		(ppsym*)NULL );

	prod[ 62 ] = pp_prod_create( g, sym[ 50 ] /* termdef */,
		sym[ 51 ], /* "$" */
		sym[ 52 ], /* ident? */
		sym[ 53 ], /* termdef' */
		sym[ 54 ], /* termflags? */
		sym[ 44 ], /* ";" */
		(ppsym*)NULL );
	prod[ 62 ]->emit = 35;

	prod[ 63 ] = pp_prod_create( g, sym[ 55 ] /* gflag */,
		sym[ 56 ], /* "emitall" */
		(ppsym*)NULL );
	prod[ 63 ]->emit = 41;

	prod[ 64 ] = pp_prod_create( g, sym[ 55 ] /* gflag */,
		sym[ 57 ], /* "emitnone" */
		(ppsym*)NULL );
	prod[ 64 ]->emit = 41;

	prod[ 65 ] = pp_prod_create( g, sym[ 55 ] /* gflag */,
		sym[ 58 ], /* "lrec" */
		(ppsym*)NULL );
	prod[ 65 ]->emit = 41;

	prod[ 66 ] = pp_prod_create( g, sym[ 55 ] /* gflag */,
		sym[ 59 ], /* "rrec" */
		(ppsym*)NULL );
	prod[ 66 ]->emit = 41;

	prod[ 67 ] = pp_prod_create( g, sym[ 61 ] /* gflag+ */,
		sym[ 61 ], /* gflag+ */
		sym[ 55 ], /* gflag */
		(ppsym*)NULL );

	prod[ 68 ] = pp_prod_create( g, sym[ 61 ] /* gflag+ */,
		sym[ 55 ], /* gflag */
		(ppsym*)NULL );

	prod[ 69 ] = pp_prod_create( g, sym[ 60 ] /* gflags */,
		sym[ 60 ], /* gflags */
		sym[ 29 ], /* "%" */
		sym[ 61 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 70 ] = pp_prod_create( g, sym[ 60 ] /* gflags */,
		sym[ 29 ], /* "%" */
		sym[ 61 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 71 ] = pp_prod_create( g, sym[ 62 ] /* definition */,
		sym[ 39 ], /* nontermdef */
		(ppsym*)NULL );

	prod[ 72 ] = pp_prod_create( g, sym[ 62 ] /* definition */,
		sym[ 50 ], /* termdef */
		(ppsym*)NULL );

	prod[ 73 ] = pp_prod_create( g, sym[ 62 ] /* definition */,
		sym[ 60 ], /* gflags */
		(ppsym*)NULL );

	prod[ 74 ] = pp_prod_create( g, sym[ 64 ] /* definition+ */,
		sym[ 64 ], /* definition+ */
		sym[ 62 ], /* definition */
		(ppsym*)NULL );

	prod[ 75 ] = pp_prod_create( g, sym[ 64 ] /* definition+ */,
		sym[ 62 ], /* definition */
		(ppsym*)NULL );

	prod[ 76 ] = pp_prod_create( g, sym[ 63 ] /* grammar */,
		sym[ 64 ], /* definition+ */
		(ppsym*)NULL );


}
