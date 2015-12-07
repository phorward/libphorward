/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bnf.c
Usage:	Self-hosted grammar for ppgram, generated from bnf.syn
----------------------------------------------------------------------------- */

/* DO NOT CHANGE THIS FILE MANUALLY. IT WILL GO AWAY! */

#include "phorward.h"

void pp_bnf_define( ppgram* g )
{
	ppsym*	sym[ 64 ];
	ppprod*	prod[ 76 ];
	/* Symbols */

	sym[ 1 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "[\\t\\n\\r ]+" );
	sym[ 1 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 2 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "//[^\\n]*\\n" );
	sym[ 2 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 3 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "/\\*([^*]|\\*[^/])*\\*/" );
	sym[ 3 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 4 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ccl", "'[^']*'" );
	sym[ 4 ]->emit = 1;

	sym[ 5 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "string", "\"[^\"]*\"" );
	sym[ 5 ]->emit = 2;

	sym[ 6 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "regex", "/(\\\\.|[^/\\\\])*/" );
	sym[ 6 ]->emit = 3;

	sym[ 7 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ident", "[A-Z_a-z][0-9A-Z_a-z]*" );
	sym[ 7 ]->emit = 10;

	sym[ 8 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "int", "[0-9]+" );
	sym[ 8 ]->emit = 11;

	sym[ 9 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "function", "[A-Z_a-z][0-9A-Z_a-z]*\\(\\)" );
	sym[ 9 ]->emit = 4;

	sym[ 10 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "(" );

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ")" );

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 14 ]->emit = 20;

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "*" );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "+" );

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "?" );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "production", (char*)NULL );

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emit" );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit'", (char*)NULL );

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit'?", (char*)NULL );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "noemit" );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "%" );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag+", (char*)NULL );

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags?", (char*)NULL );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "|" );

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag", (char*)NULL );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "goal" );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lexeme" );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags", (char*)NULL );

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag+", (char*)NULL );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags?", (char*)NULL );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'", (char*)NULL );

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ":" );

	sym[ 42 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'?", (char*)NULL );

	sym[ 43 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ";" );

	sym[ 44 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag", (char*)NULL );

	sym[ 45 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "ignore" );

	sym[ 46 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "skip" );

	sym[ 47 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags", (char*)NULL );

	sym[ 48 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag+", (char*)NULL );

	sym[ 49 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );

	sym[ 50 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "$" );

	sym[ 51 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 52 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 53 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags?", (char*)NULL );

	sym[ 54 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	sym[ 54 ]->emit = 41;

	sym[ 55 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitall" );

	sym[ 56 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitnone" );

	sym[ 57 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lrec" );

	sym[ 58 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "rrec" );

	sym[ 59 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );

	sym[ 60 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag+", (char*)NULL );

	sym[ 61 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 62 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 62 ];

	sym[ 63 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	prod[ 0 ] = pp_prod_create( g, sym[ 10 ] /* inline */,
		sym[ 11 ], /* '(' */
		sym[ 12 ], /* alternation */
		sym[ 13 ], /* ')' */
		(ppsym*)NULL );
	prod[ 0 ]->emit = 32;

	prod[ 1 ] = pp_prod_create( g, sym[ 14 ] /* symbol */,
		sym[ 7 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );
	prod[ 1 ]->emit = 20;

	prod[ 2 ] = pp_prod_create( g, sym[ 14 ] /* symbol */,
		sym[ 4 ], /* /'[^']*'/ */
		(ppsym*)NULL );
	prod[ 2 ]->emit = 20;

	prod[ 3 ] = pp_prod_create( g, sym[ 14 ] /* symbol */,
		sym[ 5 ], /* /"[^"]*"/ */
		(ppsym*)NULL );
	prod[ 3 ]->emit = 20;

	prod[ 4 ] = pp_prod_create( g, sym[ 14 ] /* symbol */,
		sym[ 6 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );
	prod[ 4 ]->emit = 20;

	prod[ 5 ] = pp_prod_create( g, sym[ 14 ] /* symbol */,
		sym[ 9 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );
	prod[ 5 ]->emit = 20;

	prod[ 6 ] = pp_prod_create( g, sym[ 14 ] /* symbol */,
		sym[ 10 ], /* inline */
		(ppsym*)NULL );
	prod[ 6 ]->emit = 20;

	prod[ 7 ] = pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 14 ], /* symbol */
		sym[ 16 ], /* '*' */
		(ppsym*)NULL );
	prod[ 7 ]->emit = 25;

	prod[ 8 ] = pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 14 ], /* symbol */
		sym[ 17 ], /* '+' */
		(ppsym*)NULL );
	prod[ 8 ]->emit = 26;

	prod[ 9 ] = pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 14 ], /* symbol */
		sym[ 18 ], /* '?' */
		(ppsym*)NULL );
	prod[ 9 ]->emit = 27;

	prod[ 10 ] = pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 14 ], /* symbol */
		(ppsym*)NULL );

	prod[ 11 ] = pp_prod_create( g, sym[ 19 ] /* sequence */,
		sym[ 19 ], /* sequence */
		sym[ 15 ], /* modifier */
		(ppsym*)NULL );

	prod[ 12 ] = pp_prod_create( g, sym[ 19 ] /* sequence */,
		sym[ 15 ], /* modifier */
		(ppsym*)NULL );

	prod[ 13 ] = pp_prod_create( g, sym[ 20 ] /* production */,
		sym[ 19 ], /* sequence */
		(ppsym*)NULL );

	prod[ 14 ] = pp_prod_create( g, sym[ 20 ] /* production */,
		(ppsym*)NULL );

	prod[ 15 ] = pp_prod_create( g, sym[ 23 ] /* emit' */,
		sym[ 8 ], /* /[0-9]+/ */
		(ppsym*)NULL );

	prod[ 16 ] = pp_prod_create( g, sym[ 23 ] /* emit' */,
		sym[ 5 ], /* /"[^"]*"/ */
		(ppsym*)NULL );

	prod[ 17 ] = pp_prod_create( g, sym[ 24 ] /* emit'? */,
		sym[ 23 ], /* emit' */
		(ppsym*)NULL );

	prod[ 18 ] = pp_prod_create( g, sym[ 24 ] /* emit'? */,
		(ppsym*)NULL );

	prod[ 19 ] = pp_prod_create( g, sym[ 21 ] /* emit */,
		sym[ 22 ], /* "emit" */
		sym[ 24 ], /* emit'? */
		(ppsym*)NULL );
	prod[ 19 ]->emit = 42;

	prod[ 20 ] = pp_prod_create( g, sym[ 25 ] /* flag */,
		sym[ 26 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 20 ]->emit = 40;

	prod[ 21 ] = pp_prod_create( g, sym[ 25 ] /* flag */,
		sym[ 21 ], /* emit */
		(ppsym*)NULL );

	prod[ 22 ] = pp_prod_create( g, sym[ 29 ] /* flag+ */,
		sym[ 29 ], /* flag+ */
		sym[ 25 ], /* flag */
		(ppsym*)NULL );

	prod[ 23 ] = pp_prod_create( g, sym[ 29 ] /* flag+ */,
		sym[ 25 ], /* flag */
		(ppsym*)NULL );

	prod[ 24 ] = pp_prod_create( g, sym[ 27 ] /* flags */,
		sym[ 27 ], /* flags */
		sym[ 28 ], /* '%' */
		sym[ 29 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 25 ] = pp_prod_create( g, sym[ 27 ] /* flags */,
		sym[ 28 ], /* '%' */
		sym[ 29 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 26 ] = pp_prod_create( g, sym[ 31 ] /* flags? */,
		sym[ 27 ], /* flags */
		(ppsym*)NULL );

	prod[ 27 ] = pp_prod_create( g, sym[ 31 ] /* flags? */,
		(ppsym*)NULL );

	prod[ 28 ] = pp_prod_create( g, sym[ 30 ] /* alternative */,
		sym[ 20 ], /* production */
		sym[ 31 ], /* flags? */
		(ppsym*)NULL );
	prod[ 28 ]->emit = 30;

	prod[ 29 ] = pp_prod_create( g, sym[ 12 ] /* alternation */,
		sym[ 12 ], /* alternation */
		sym[ 32 ], /* '|' */
		sym[ 30 ], /* alternative */
		(ppsym*)NULL );

	prod[ 30 ] = pp_prod_create( g, sym[ 12 ] /* alternation */,
		sym[ 30 ], /* alternative */
		(ppsym*)NULL );

	prod[ 31 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 26 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 31 ]->emit = 40;

	prod[ 32 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 34 ], /* "goal" */
		(ppsym*)NULL );
	prod[ 32 ]->emit = 40;

	prod[ 33 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 35 ], /* "lexeme" */
		(ppsym*)NULL );
	prod[ 33 ]->emit = 40;

	prod[ 34 ] = pp_prod_create( g, sym[ 33 ] /* nontermflag */,
		sym[ 21 ], /* emit */
		(ppsym*)NULL );

	prod[ 35 ] = pp_prod_create( g, sym[ 37 ] /* nontermflag+ */,
		sym[ 37 ], /* nontermflag+ */
		sym[ 33 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 36 ] = pp_prod_create( g, sym[ 37 ] /* nontermflag+ */,
		sym[ 33 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 37 ] = pp_prod_create( g, sym[ 36 ] /* nontermflags */,
		sym[ 36 ], /* nontermflags */
		sym[ 28 ], /* '%' */
		sym[ 37 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 38 ] = pp_prod_create( g, sym[ 36 ] /* nontermflags */,
		sym[ 28 ], /* '%' */
		sym[ 37 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 39 ] = pp_prod_create( g, sym[ 39 ] /* nontermflags? */,
		sym[ 36 ], /* nontermflags */
		(ppsym*)NULL );

	prod[ 40 ] = pp_prod_create( g, sym[ 39 ] /* nontermflags? */,
		(ppsym*)NULL );

	prod[ 41 ] = pp_prod_create( g, sym[ 40 ] /* nontermdef' */,
		sym[ 41 ], /* ':' */
		sym[ 12 ], /* alternation */
		(ppsym*)NULL );

	prod[ 42 ] = pp_prod_create( g, sym[ 42 ] /* nontermdef'? */,
		sym[ 40 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 43 ] = pp_prod_create( g, sym[ 42 ] /* nontermdef'? */,
		(ppsym*)NULL );

	prod[ 44 ] = pp_prod_create( g, sym[ 38 ] /* nontermdef */,
		sym[ 7 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 39 ], /* nontermflags? */
		sym[ 42 ], /* nontermdef'? */
		sym[ 43 ], /* ';' */
		(ppsym*)NULL );
	prod[ 44 ]->emit = 31;

	prod[ 45 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 26 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 45 ]->emit = 40;

	prod[ 46 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 45 ], /* "ignore" */
		(ppsym*)NULL );
	prod[ 46 ]->emit = 40;

	prod[ 47 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 46 ], /* "skip" */
		(ppsym*)NULL );
	prod[ 47 ]->emit = 40;

	prod[ 48 ] = pp_prod_create( g, sym[ 44 ] /* termflag */,
		sym[ 21 ], /* emit */
		(ppsym*)NULL );

	prod[ 49 ] = pp_prod_create( g, sym[ 48 ] /* termflag+ */,
		sym[ 48 ], /* termflag+ */
		sym[ 44 ], /* termflag */
		(ppsym*)NULL );

	prod[ 50 ] = pp_prod_create( g, sym[ 48 ] /* termflag+ */,
		sym[ 44 ], /* termflag */
		(ppsym*)NULL );

	prod[ 51 ] = pp_prod_create( g, sym[ 47 ] /* termflags */,
		sym[ 47 ], /* termflags */
		sym[ 28 ], /* '%' */
		sym[ 48 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 52 ] = pp_prod_create( g, sym[ 47 ] /* termflags */,
		sym[ 28 ], /* '%' */
		sym[ 48 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 53 ] = pp_prod_create( g, sym[ 51 ] /* ident? */,
		sym[ 7 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 54 ] = pp_prod_create( g, sym[ 51 ] /* ident? */,
		(ppsym*)NULL );

	prod[ 55 ] = pp_prod_create( g, sym[ 52 ] /* termdef' */,
		sym[ 4 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 56 ] = pp_prod_create( g, sym[ 52 ] /* termdef' */,
		sym[ 5 ], /* /"[^"]*"/ */
		(ppsym*)NULL );

	prod[ 57 ] = pp_prod_create( g, sym[ 52 ] /* termdef' */,
		sym[ 6 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 58 ] = pp_prod_create( g, sym[ 52 ] /* termdef' */,
		sym[ 9 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\\(\\)/ */
		(ppsym*)NULL );

	prod[ 59 ] = pp_prod_create( g, sym[ 53 ] /* termflags? */,
		sym[ 47 ], /* termflags */
		(ppsym*)NULL );

	prod[ 60 ] = pp_prod_create( g, sym[ 53 ] /* termflags? */,
		(ppsym*)NULL );

	prod[ 61 ] = pp_prod_create( g, sym[ 49 ] /* termdef */,
		sym[ 50 ], /* '$' */
		sym[ 51 ], /* ident? */
		sym[ 52 ], /* termdef' */
		sym[ 53 ], /* termflags? */
		sym[ 43 ], /* ';' */
		(ppsym*)NULL );
	prod[ 61 ]->emit = 35;

	prod[ 62 ] = pp_prod_create( g, sym[ 54 ] /* gflag */,
		sym[ 55 ], /* "emitall" */
		(ppsym*)NULL );
	prod[ 62 ]->emit = 41;

	prod[ 63 ] = pp_prod_create( g, sym[ 54 ] /* gflag */,
		sym[ 56 ], /* "emitnone" */
		(ppsym*)NULL );
	prod[ 63 ]->emit = 41;

	prod[ 64 ] = pp_prod_create( g, sym[ 54 ] /* gflag */,
		sym[ 57 ], /* "lrec" */
		(ppsym*)NULL );
	prod[ 64 ]->emit = 41;

	prod[ 65 ] = pp_prod_create( g, sym[ 54 ] /* gflag */,
		sym[ 58 ], /* "rrec" */
		(ppsym*)NULL );
	prod[ 65 ]->emit = 41;

	prod[ 66 ] = pp_prod_create( g, sym[ 60 ] /* gflag+ */,
		sym[ 60 ], /* gflag+ */
		sym[ 54 ], /* gflag */
		(ppsym*)NULL );

	prod[ 67 ] = pp_prod_create( g, sym[ 60 ] /* gflag+ */,
		sym[ 54 ], /* gflag */
		(ppsym*)NULL );

	prod[ 68 ] = pp_prod_create( g, sym[ 59 ] /* gflags */,
		sym[ 59 ], /* gflags */
		sym[ 28 ], /* '%' */
		sym[ 60 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 69 ] = pp_prod_create( g, sym[ 59 ] /* gflags */,
		sym[ 28 ], /* '%' */
		sym[ 60 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 70 ] = pp_prod_create( g, sym[ 61 ] /* definition */,
		sym[ 38 ], /* nontermdef */
		(ppsym*)NULL );

	prod[ 71 ] = pp_prod_create( g, sym[ 61 ] /* definition */,
		sym[ 49 ], /* termdef */
		(ppsym*)NULL );

	prod[ 72 ] = pp_prod_create( g, sym[ 61 ] /* definition */,
		sym[ 59 ], /* gflags */
		(ppsym*)NULL );

	prod[ 73 ] = pp_prod_create( g, sym[ 63 ] /* definition+ */,
		sym[ 63 ], /* definition+ */
		sym[ 61 ], /* definition */
		(ppsym*)NULL );

	prod[ 74 ] = pp_prod_create( g, sym[ 63 ] /* definition+ */,
		sym[ 61 ], /* definition */
		(ppsym*)NULL );

	prod[ 75 ] = pp_prod_create( g, sym[ 62 ] /* grammar */,
		sym[ 63 ], /* definition+ */
		(ppsym*)NULL );


}
