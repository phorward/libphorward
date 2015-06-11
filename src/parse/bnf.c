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
	ppsym*	sym[ 60 ];
	ppprod*	prod[ 71 ];
	/* Symbols */

	sym[ 1 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "[\\t\\n\\r ]+" );
	sym[ 1 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 2 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "//[^\\n]*\\n" );
	sym[ 2 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 3 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ccl", "'[^']*'" );
	sym[ 3 ]->emit = 1;

	sym[ 4 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "string", "\"[^\"]*\"" );
	sym[ 4 ]->emit = 2;

	sym[ 5 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "regex", "/(\\\\.|[^/\\\\])*/" );
	sym[ 5 ]->emit = 3;

	sym[ 6 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ident", "[A-Z_a-z][0-9A-Z_a-z]*" );
	sym[ 6 ]->emit = 10;

	sym[ 7 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "int", "[0-9]+" );
	sym[ 7 ]->emit = 11;

	sym[ 8 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );

	sym[ 9 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "(" );

	sym[ 10 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ")" );

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 12 ]->emit = 20;

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "*" );

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "+" );

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "?" );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "production", (char*)NULL );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emit" );

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "int?", (char*)NULL );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "noemit" );

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "%" );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag+", (char*)NULL );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags?", (char*)NULL );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "|" );

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag", (char*)NULL );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "goal" );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lexeme" );

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags", (char*)NULL );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag+", (char*)NULL );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags?", (char*)NULL );

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'", (char*)NULL );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ":" );

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'?", (char*)NULL );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ";" );

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag", (char*)NULL );

	sym[ 42 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "ignore" );

	sym[ 43 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags", (char*)NULL );

	sym[ 44 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag+", (char*)NULL );

	sym[ 45 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );

	sym[ 46 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "$" );

	sym[ 47 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 48 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 49 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags?", (char*)NULL );

	sym[ 50 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	sym[ 50 ]->emit = 41;

	sym[ 51 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitall" );

	sym[ 52 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitnone" );

	sym[ 53 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lrec" );

	sym[ 54 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "rrec" );

	sym[ 55 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );

	sym[ 56 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag+", (char*)NULL );

	sym[ 57 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 58 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 58 ];

	sym[ 59 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	prod[ 0 ] = pp_prod_create( g, sym[ 8 ] /* inline */,
		sym[ 9 ], /* '(' */
		sym[ 10 ], /* alternation */
		sym[ 11 ], /* ')' */
		(ppsym*)NULL );
	prod[ 0 ]->emit = 32;

	prod[ 1 ] = pp_prod_create( g, sym[ 12 ] /* symbol */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );
	prod[ 1 ]->emit = 20;

	prod[ 2 ] = pp_prod_create( g, sym[ 12 ] /* symbol */,
		sym[ 3 ], /* /'[^']*'/ */
		(ppsym*)NULL );
	prod[ 2 ]->emit = 20;

	prod[ 3 ] = pp_prod_create( g, sym[ 12 ] /* symbol */,
		sym[ 4 ], /* /"[^"]*"/ */
		(ppsym*)NULL );
	prod[ 3 ]->emit = 20;

	prod[ 4 ] = pp_prod_create( g, sym[ 12 ] /* symbol */,
		sym[ 5 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );
	prod[ 4 ]->emit = 20;

	prod[ 5 ] = pp_prod_create( g, sym[ 12 ] /* symbol */,
		sym[ 8 ], /* inline */
		(ppsym*)NULL );
	prod[ 5 ]->emit = 20;

	prod[ 6 ] = pp_prod_create( g, sym[ 13 ] /* modifier */,
		sym[ 12 ], /* symbol */
		sym[ 14 ], /* '*' */
		(ppsym*)NULL );
	prod[ 6 ]->emit = 25;

	prod[ 7 ] = pp_prod_create( g, sym[ 13 ] /* modifier */,
		sym[ 12 ], /* symbol */
		sym[ 15 ], /* '+' */
		(ppsym*)NULL );
	prod[ 7 ]->emit = 26;

	prod[ 8 ] = pp_prod_create( g, sym[ 13 ] /* modifier */,
		sym[ 12 ], /* symbol */
		sym[ 16 ], /* '?' */
		(ppsym*)NULL );
	prod[ 8 ]->emit = 27;

	prod[ 9 ] = pp_prod_create( g, sym[ 13 ] /* modifier */,
		sym[ 12 ], /* symbol */
		(ppsym*)NULL );

	prod[ 10 ] = pp_prod_create( g, sym[ 17 ] /* sequence */,
		sym[ 17 ], /* sequence */
		sym[ 13 ], /* modifier */
		(ppsym*)NULL );

	prod[ 11 ] = pp_prod_create( g, sym[ 17 ] /* sequence */,
		sym[ 13 ], /* modifier */
		(ppsym*)NULL );

	prod[ 12 ] = pp_prod_create( g, sym[ 18 ] /* production */,
		sym[ 17 ], /* sequence */
		(ppsym*)NULL );

	prod[ 13 ] = pp_prod_create( g, sym[ 18 ] /* production */,
		(ppsym*)NULL );

	prod[ 14 ] = pp_prod_create( g, sym[ 21 ] /* int? */,
		sym[ 7 ], /* /[0-9]+/ */
		(ppsym*)NULL );

	prod[ 15 ] = pp_prod_create( g, sym[ 21 ] /* int? */,
		(ppsym*)NULL );

	prod[ 16 ] = pp_prod_create( g, sym[ 19 ] /* emit */,
		sym[ 20 ], /* "emit" */
		sym[ 21 ], /* int? */
		(ppsym*)NULL );
	prod[ 16 ]->emit = 42;

	prod[ 17 ] = pp_prod_create( g, sym[ 22 ] /* flag */,
		sym[ 23 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 17 ]->emit = 40;

	prod[ 18 ] = pp_prod_create( g, sym[ 22 ] /* flag */,
		sym[ 19 ], /* emit */
		(ppsym*)NULL );

	prod[ 19 ] = pp_prod_create( g, sym[ 26 ] /* flag+ */,
		sym[ 26 ], /* flag+ */
		sym[ 22 ], /* flag */
		(ppsym*)NULL );

	prod[ 20 ] = pp_prod_create( g, sym[ 26 ] /* flag+ */,
		sym[ 22 ], /* flag */
		(ppsym*)NULL );

	prod[ 21 ] = pp_prod_create( g, sym[ 24 ] /* flags */,
		sym[ 24 ], /* flags */
		sym[ 25 ], /* '%' */
		sym[ 26 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 22 ] = pp_prod_create( g, sym[ 24 ] /* flags */,
		sym[ 25 ], /* '%' */
		sym[ 26 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 23 ] = pp_prod_create( g, sym[ 28 ] /* flags? */,
		sym[ 24 ], /* flags */
		(ppsym*)NULL );

	prod[ 24 ] = pp_prod_create( g, sym[ 28 ] /* flags? */,
		(ppsym*)NULL );

	prod[ 25 ] = pp_prod_create( g, sym[ 27 ] /* alternative */,
		sym[ 18 ], /* production */
		sym[ 28 ], /* flags? */
		(ppsym*)NULL );
	prod[ 25 ]->emit = 30;

	prod[ 26 ] = pp_prod_create( g, sym[ 10 ] /* alternation */,
		sym[ 10 ], /* alternation */
		sym[ 29 ], /* '|' */
		sym[ 27 ], /* alternative */
		(ppsym*)NULL );

	prod[ 27 ] = pp_prod_create( g, sym[ 10 ] /* alternation */,
		sym[ 27 ], /* alternative */
		(ppsym*)NULL );

	prod[ 28 ] = pp_prod_create( g, sym[ 30 ] /* nontermflag */,
		sym[ 23 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 28 ]->emit = 40;

	prod[ 29 ] = pp_prod_create( g, sym[ 30 ] /* nontermflag */,
		sym[ 31 ], /* "goal" */
		(ppsym*)NULL );
	prod[ 29 ]->emit = 40;

	prod[ 30 ] = pp_prod_create( g, sym[ 30 ] /* nontermflag */,
		sym[ 32 ], /* "lexeme" */
		(ppsym*)NULL );
	prod[ 30 ]->emit = 40;

	prod[ 31 ] = pp_prod_create( g, sym[ 30 ] /* nontermflag */,
		sym[ 19 ], /* emit */
		(ppsym*)NULL );

	prod[ 32 ] = pp_prod_create( g, sym[ 34 ] /* nontermflag+ */,
		sym[ 34 ], /* nontermflag+ */
		sym[ 30 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 33 ] = pp_prod_create( g, sym[ 34 ] /* nontermflag+ */,
		sym[ 30 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 34 ] = pp_prod_create( g, sym[ 33 ] /* nontermflags */,
		sym[ 33 ], /* nontermflags */
		sym[ 25 ], /* '%' */
		sym[ 34 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 35 ] = pp_prod_create( g, sym[ 33 ] /* nontermflags */,
		sym[ 25 ], /* '%' */
		sym[ 34 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 36 ] = pp_prod_create( g, sym[ 36 ] /* nontermflags? */,
		sym[ 33 ], /* nontermflags */
		(ppsym*)NULL );

	prod[ 37 ] = pp_prod_create( g, sym[ 36 ] /* nontermflags? */,
		(ppsym*)NULL );

	prod[ 38 ] = pp_prod_create( g, sym[ 37 ] /* nontermdef' */,
		sym[ 38 ], /* ':' */
		sym[ 10 ], /* alternation */
		(ppsym*)NULL );

	prod[ 39 ] = pp_prod_create( g, sym[ 39 ] /* nontermdef'? */,
		sym[ 37 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 40 ] = pp_prod_create( g, sym[ 39 ] /* nontermdef'? */,
		(ppsym*)NULL );

	prod[ 41 ] = pp_prod_create( g, sym[ 35 ] /* nontermdef */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 36 ], /* nontermflags? */
		sym[ 39 ], /* nontermdef'? */
		sym[ 40 ], /* ';' */
		(ppsym*)NULL );
	prod[ 41 ]->emit = 31;

	prod[ 42 ] = pp_prod_create( g, sym[ 41 ] /* termflag */,
		sym[ 23 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 42 ]->emit = 40;

	prod[ 43 ] = pp_prod_create( g, sym[ 41 ] /* termflag */,
		sym[ 42 ], /* "ignore" */
		(ppsym*)NULL );
	prod[ 43 ]->emit = 40;

	prod[ 44 ] = pp_prod_create( g, sym[ 41 ] /* termflag */,
		sym[ 19 ], /* emit */
		(ppsym*)NULL );

	prod[ 45 ] = pp_prod_create( g, sym[ 44 ] /* termflag+ */,
		sym[ 44 ], /* termflag+ */
		sym[ 41 ], /* termflag */
		(ppsym*)NULL );

	prod[ 46 ] = pp_prod_create( g, sym[ 44 ] /* termflag+ */,
		sym[ 41 ], /* termflag */
		(ppsym*)NULL );

	prod[ 47 ] = pp_prod_create( g, sym[ 43 ] /* termflags */,
		sym[ 43 ], /* termflags */
		sym[ 25 ], /* '%' */
		sym[ 44 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 48 ] = pp_prod_create( g, sym[ 43 ] /* termflags */,
		sym[ 25 ], /* '%' */
		sym[ 44 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 49 ] = pp_prod_create( g, sym[ 47 ] /* ident? */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 50 ] = pp_prod_create( g, sym[ 47 ] /* ident? */,
		(ppsym*)NULL );

	prod[ 51 ] = pp_prod_create( g, sym[ 48 ] /* termdef' */,
		sym[ 3 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 52 ] = pp_prod_create( g, sym[ 48 ] /* termdef' */,
		sym[ 4 ], /* /"[^"]*"/ */
		(ppsym*)NULL );

	prod[ 53 ] = pp_prod_create( g, sym[ 48 ] /* termdef' */,
		sym[ 5 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 54 ] = pp_prod_create( g, sym[ 49 ] /* termflags? */,
		sym[ 43 ], /* termflags */
		(ppsym*)NULL );

	prod[ 55 ] = pp_prod_create( g, sym[ 49 ] /* termflags? */,
		(ppsym*)NULL );

	prod[ 56 ] = pp_prod_create( g, sym[ 45 ] /* termdef */,
		sym[ 46 ], /* '$' */
		sym[ 47 ], /* ident? */
		sym[ 48 ], /* termdef' */
		sym[ 49 ], /* termflags? */
		sym[ 40 ], /* ';' */
		(ppsym*)NULL );
	prod[ 56 ]->emit = 35;

	prod[ 57 ] = pp_prod_create( g, sym[ 50 ] /* gflag */,
		sym[ 51 ], /* "emitall" */
		(ppsym*)NULL );
	prod[ 57 ]->emit = 41;

	prod[ 58 ] = pp_prod_create( g, sym[ 50 ] /* gflag */,
		sym[ 52 ], /* "emitnone" */
		(ppsym*)NULL );
	prod[ 58 ]->emit = 41;

	prod[ 59 ] = pp_prod_create( g, sym[ 50 ] /* gflag */,
		sym[ 53 ], /* "lrec" */
		(ppsym*)NULL );
	prod[ 59 ]->emit = 41;

	prod[ 60 ] = pp_prod_create( g, sym[ 50 ] /* gflag */,
		sym[ 54 ], /* "rrec" */
		(ppsym*)NULL );
	prod[ 60 ]->emit = 41;

	prod[ 61 ] = pp_prod_create( g, sym[ 56 ] /* gflag+ */,
		sym[ 56 ], /* gflag+ */
		sym[ 50 ], /* gflag */
		(ppsym*)NULL );

	prod[ 62 ] = pp_prod_create( g, sym[ 56 ] /* gflag+ */,
		sym[ 50 ], /* gflag */
		(ppsym*)NULL );

	prod[ 63 ] = pp_prod_create( g, sym[ 55 ] /* gflags */,
		sym[ 55 ], /* gflags */
		sym[ 25 ], /* '%' */
		sym[ 56 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 64 ] = pp_prod_create( g, sym[ 55 ] /* gflags */,
		sym[ 25 ], /* '%' */
		sym[ 56 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 65 ] = pp_prod_create( g, sym[ 57 ] /* definition */,
		sym[ 35 ], /* nontermdef */
		(ppsym*)NULL );

	prod[ 66 ] = pp_prod_create( g, sym[ 57 ] /* definition */,
		sym[ 45 ], /* termdef */
		(ppsym*)NULL );

	prod[ 67 ] = pp_prod_create( g, sym[ 57 ] /* definition */,
		sym[ 55 ], /* gflags */
		(ppsym*)NULL );

	prod[ 68 ] = pp_prod_create( g, sym[ 59 ] /* definition+ */,
		sym[ 59 ], /* definition+ */
		sym[ 57 ], /* definition */
		(ppsym*)NULL );

	prod[ 69 ] = pp_prod_create( g, sym[ 59 ] /* definition+ */,
		sym[ 57 ], /* definition */
		(ppsym*)NULL );

	prod[ 70 ] = pp_prod_create( g, sym[ 58 ] /* grammar */,
		sym[ 59 ], /* definition+ */
		(ppsym*)NULL );


}
