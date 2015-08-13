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
	ppsym*	sym[ 72 ];
	ppprod*	prod[ 88 ];
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

	sym[ 8 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "float", "[0-9]+\\.[0-9]*|[0-9]*\\.[0-9]+" );
	sym[ 8 ]->emit = 12;

	sym[ 9 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );

	sym[ 10 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "(" );

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ")" );

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 13 ]->emit = 20;

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "*" );

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "+" );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "?" );

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "do_cmd", (char*)NULL );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "do_param", (char*)NULL );
	sym[ 19 ]->emit = 51;

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "do_param_list", (char*)NULL );

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "," );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "do_params", (char*)NULL );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "do_param_list?", (char*)NULL );

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "production", (char*)NULL );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emit" );

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "int?", (char*)NULL );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "before" );

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "do_params?", (char*)NULL );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "within" );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "behind" );

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "behind?", (char*)NULL );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "noemit" );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "%" );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag+", (char*)NULL );

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags?", (char*)NULL );

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "|" );

	sym[ 42 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag", (char*)NULL );

	sym[ 43 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "goal" );

	sym[ 44 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lexeme" );

	sym[ 45 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags", (char*)NULL );

	sym[ 46 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflag+", (char*)NULL );

	sym[ 47 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );

	sym[ 48 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermflags?", (char*)NULL );

	sym[ 49 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'", (char*)NULL );

	sym[ 50 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ":" );

	sym[ 51 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef'?", (char*)NULL );

	sym[ 52 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ";" );

	sym[ 53 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag", (char*)NULL );

	sym[ 54 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "ignore" );

	sym[ 55 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags", (char*)NULL );

	sym[ 56 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflag+", (char*)NULL );

	sym[ 57 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );

	sym[ 58 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "$" );

	sym[ 59 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 60 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 61 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termflags?", (char*)NULL );

	sym[ 62 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	sym[ 62 ]->emit = 41;

	sym[ 63 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitall" );

	sym[ 64 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitnone" );

	sym[ 65 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lrec" );

	sym[ 66 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "rrec" );

	sym[ 67 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );

	sym[ 68 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag+", (char*)NULL );

	sym[ 69 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 70 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 70 ];

	sym[ 71 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	prod[ 0 ] = pp_prod_create( g, sym[ 9 ] /* inline */,
		sym[ 10 ], /* '(' */
		sym[ 11 ], /* alternation */
		sym[ 12 ], /* ')' */
		(ppsym*)NULL );
	prod[ 0 ]->emit = 32;

	prod[ 1 ] = pp_prod_create( g, sym[ 13 ] /* symbol */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );
	prod[ 1 ]->emit = 20;

	prod[ 2 ] = pp_prod_create( g, sym[ 13 ] /* symbol */,
		sym[ 3 ], /* /'[^']*'/ */
		(ppsym*)NULL );
	prod[ 2 ]->emit = 20;

	prod[ 3 ] = pp_prod_create( g, sym[ 13 ] /* symbol */,
		sym[ 4 ], /* /"[^"]*"/ */
		(ppsym*)NULL );
	prod[ 3 ]->emit = 20;

	prod[ 4 ] = pp_prod_create( g, sym[ 13 ] /* symbol */,
		sym[ 5 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );
	prod[ 4 ]->emit = 20;

	prod[ 5 ] = pp_prod_create( g, sym[ 13 ] /* symbol */,
		sym[ 9 ], /* inline */
		(ppsym*)NULL );
	prod[ 5 ]->emit = 20;

	prod[ 6 ] = pp_prod_create( g, sym[ 14 ] /* modifier */,
		sym[ 13 ], /* symbol */
		sym[ 15 ], /* '*' */
		(ppsym*)NULL );
	prod[ 6 ]->emit = 25;

	prod[ 7 ] = pp_prod_create( g, sym[ 14 ] /* modifier */,
		sym[ 13 ], /* symbol */
		sym[ 16 ], /* '+' */
		(ppsym*)NULL );
	prod[ 7 ]->emit = 26;

	prod[ 8 ] = pp_prod_create( g, sym[ 14 ] /* modifier */,
		sym[ 13 ], /* symbol */
		sym[ 17 ], /* '?' */
		(ppsym*)NULL );
	prod[ 8 ]->emit = 27;

	prod[ 9 ] = pp_prod_create( g, sym[ 14 ] /* modifier */,
		sym[ 13 ], /* symbol */
		(ppsym*)NULL );

	prod[ 10 ] = pp_prod_create( g, sym[ 18 ] /* do_cmd */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );
	prod[ 10 ]->emit = 50;

	prod[ 11 ] = pp_prod_create( g, sym[ 19 ] /* do_param */,
		sym[ 4 ], /* /"[^"]*"/ */
		(ppsym*)NULL );
	prod[ 11 ]->emit = 51;

	prod[ 12 ] = pp_prod_create( g, sym[ 19 ] /* do_param */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );
	prod[ 12 ]->emit = 51;

	prod[ 13 ] = pp_prod_create( g, sym[ 19 ] /* do_param */,
		sym[ 7 ], /* /[0-9]+/ */
		(ppsym*)NULL );
	prod[ 13 ]->emit = 51;

	prod[ 14 ] = pp_prod_create( g, sym[ 19 ] /* do_param */,
		sym[ 8 ], /* /[0-9]+\\.[0-9]*|[0-9]*\\.[0-9]+/ */
		(ppsym*)NULL );
	prod[ 14 ]->emit = 51;

	prod[ 15 ] = pp_prod_create( g, sym[ 20 ] /* do_param_list */,
		sym[ 20 ], /* do_param_list */
		sym[ 21 ], /* ',' */
		sym[ 19 ], /* do_param */
		(ppsym*)NULL );

	prod[ 16 ] = pp_prod_create( g, sym[ 20 ] /* do_param_list */,
		sym[ 19 ], /* do_param */
		(ppsym*)NULL );

	prod[ 17 ] = pp_prod_create( g, sym[ 23 ] /* do_param_list? */,
		sym[ 20 ], /* do_param_list */
		(ppsym*)NULL );

	prod[ 18 ] = pp_prod_create( g, sym[ 23 ] /* do_param_list? */,
		(ppsym*)NULL );

	prod[ 19 ] = pp_prod_create( g, sym[ 22 ] /* do_params */,
		sym[ 10 ], /* '(' */
		sym[ 23 ], /* do_param_list? */
		sym[ 12 ], /* ')' */
		(ppsym*)NULL );

	prod[ 20 ] = pp_prod_create( g, sym[ 24 ] /* sequence */,
		sym[ 24 ], /* sequence */
		sym[ 14 ], /* modifier */
		(ppsym*)NULL );

	prod[ 21 ] = pp_prod_create( g, sym[ 24 ] /* sequence */,
		sym[ 14 ], /* modifier */
		(ppsym*)NULL );

	prod[ 22 ] = pp_prod_create( g, sym[ 25 ] /* production */,
		sym[ 24 ], /* sequence */
		(ppsym*)NULL );

	prod[ 23 ] = pp_prod_create( g, sym[ 25 ] /* production */,
		(ppsym*)NULL );

	prod[ 24 ] = pp_prod_create( g, sym[ 28 ] /* int? */,
		sym[ 7 ], /* /[0-9]+/ */
		(ppsym*)NULL );

	prod[ 25 ] = pp_prod_create( g, sym[ 28 ] /* int? */,
		(ppsym*)NULL );

	prod[ 26 ] = pp_prod_create( g, sym[ 26 ] /* emit */,
		sym[ 27 ], /* "emit" */
		sym[ 28 ], /* int? */
		(ppsym*)NULL );
	prod[ 26 ]->emit = 42;

	prod[ 27 ] = pp_prod_create( g, sym[ 30 ] /* do_params? */,
		sym[ 22 ], /* do_params */
		(ppsym*)NULL );

	prod[ 28 ] = pp_prod_create( g, sym[ 30 ] /* do_params? */,
		(ppsym*)NULL );

	prod[ 29 ] = pp_prod_create( g, sym[ 26 ] /* emit */,
		sym[ 29 ], /* "before" */
		sym[ 18 ], /* do_cmd */
		sym[ 30 ], /* do_params? */
		(ppsym*)NULL );
	prod[ 29 ]->emit = 60;

	prod[ 30 ] = pp_prod_create( g, sym[ 26 ] /* emit */,
		sym[ 31 ], /* "within" */
		sym[ 18 ], /* do_cmd */
		sym[ 30 ], /* do_params? */
		(ppsym*)NULL );
	prod[ 30 ]->emit = 61;

	prod[ 31 ] = pp_prod_create( g, sym[ 33 ] /* behind? */,
		sym[ 32 ], /* "behind" */
		(ppsym*)NULL );

	prod[ 32 ] = pp_prod_create( g, sym[ 33 ] /* behind? */,
		(ppsym*)NULL );

	prod[ 33 ] = pp_prod_create( g, sym[ 26 ] /* emit */,
		sym[ 33 ], /* behind? */
		sym[ 18 ], /* do_cmd */
		sym[ 30 ], /* do_params? */
		(ppsym*)NULL );
	prod[ 33 ]->emit = 62;

	prod[ 34 ] = pp_prod_create( g, sym[ 34 ] /* flag */,
		sym[ 35 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 34 ]->emit = 40;

	prod[ 35 ] = pp_prod_create( g, sym[ 34 ] /* flag */,
		sym[ 26 ], /* emit */
		(ppsym*)NULL );

	prod[ 36 ] = pp_prod_create( g, sym[ 38 ] /* flag+ */,
		sym[ 38 ], /* flag+ */
		sym[ 34 ], /* flag */
		(ppsym*)NULL );

	prod[ 37 ] = pp_prod_create( g, sym[ 38 ] /* flag+ */,
		sym[ 34 ], /* flag */
		(ppsym*)NULL );

	prod[ 38 ] = pp_prod_create( g, sym[ 36 ] /* flags */,
		sym[ 36 ], /* flags */
		sym[ 37 ], /* '%' */
		sym[ 38 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 39 ] = pp_prod_create( g, sym[ 36 ] /* flags */,
		sym[ 37 ], /* '%' */
		sym[ 38 ], /* flag+ */
		(ppsym*)NULL );

	prod[ 40 ] = pp_prod_create( g, sym[ 40 ] /* flags? */,
		sym[ 36 ], /* flags */
		(ppsym*)NULL );

	prod[ 41 ] = pp_prod_create( g, sym[ 40 ] /* flags? */,
		(ppsym*)NULL );

	prod[ 42 ] = pp_prod_create( g, sym[ 39 ] /* alternative */,
		sym[ 25 ], /* production */
		sym[ 40 ], /* flags? */
		(ppsym*)NULL );
	prod[ 42 ]->emit = 30;

	prod[ 43 ] = pp_prod_create( g, sym[ 11 ] /* alternation */,
		sym[ 11 ], /* alternation */
		sym[ 41 ], /* '|' */
		sym[ 39 ], /* alternative */
		(ppsym*)NULL );

	prod[ 44 ] = pp_prod_create( g, sym[ 11 ] /* alternation */,
		sym[ 39 ], /* alternative */
		(ppsym*)NULL );

	prod[ 45 ] = pp_prod_create( g, sym[ 42 ] /* nontermflag */,
		sym[ 35 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 45 ]->emit = 40;

	prod[ 46 ] = pp_prod_create( g, sym[ 42 ] /* nontermflag */,
		sym[ 43 ], /* "goal" */
		(ppsym*)NULL );
	prod[ 46 ]->emit = 40;

	prod[ 47 ] = pp_prod_create( g, sym[ 42 ] /* nontermflag */,
		sym[ 44 ], /* "lexeme" */
		(ppsym*)NULL );
	prod[ 47 ]->emit = 40;

	prod[ 48 ] = pp_prod_create( g, sym[ 42 ] /* nontermflag */,
		sym[ 26 ], /* emit */
		(ppsym*)NULL );

	prod[ 49 ] = pp_prod_create( g, sym[ 46 ] /* nontermflag+ */,
		sym[ 46 ], /* nontermflag+ */
		sym[ 42 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 50 ] = pp_prod_create( g, sym[ 46 ] /* nontermflag+ */,
		sym[ 42 ], /* nontermflag */
		(ppsym*)NULL );

	prod[ 51 ] = pp_prod_create( g, sym[ 45 ] /* nontermflags */,
		sym[ 45 ], /* nontermflags */
		sym[ 37 ], /* '%' */
		sym[ 46 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 52 ] = pp_prod_create( g, sym[ 45 ] /* nontermflags */,
		sym[ 37 ], /* '%' */
		sym[ 46 ], /* nontermflag+ */
		(ppsym*)NULL );

	prod[ 53 ] = pp_prod_create( g, sym[ 48 ] /* nontermflags? */,
		sym[ 45 ], /* nontermflags */
		(ppsym*)NULL );

	prod[ 54 ] = pp_prod_create( g, sym[ 48 ] /* nontermflags? */,
		(ppsym*)NULL );

	prod[ 55 ] = pp_prod_create( g, sym[ 49 ] /* nontermdef' */,
		sym[ 50 ], /* ':' */
		sym[ 11 ], /* alternation */
		(ppsym*)NULL );

	prod[ 56 ] = pp_prod_create( g, sym[ 51 ] /* nontermdef'? */,
		sym[ 49 ], /* nontermdef' */
		(ppsym*)NULL );

	prod[ 57 ] = pp_prod_create( g, sym[ 51 ] /* nontermdef'? */,
		(ppsym*)NULL );

	prod[ 58 ] = pp_prod_create( g, sym[ 47 ] /* nontermdef */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 48 ], /* nontermflags? */
		sym[ 51 ], /* nontermdef'? */
		sym[ 52 ], /* ';' */
		(ppsym*)NULL );
	prod[ 58 ]->emit = 31;

	prod[ 59 ] = pp_prod_create( g, sym[ 53 ] /* termflag */,
		sym[ 35 ], /* "noemit" */
		(ppsym*)NULL );
	prod[ 59 ]->emit = 40;

	prod[ 60 ] = pp_prod_create( g, sym[ 53 ] /* termflag */,
		sym[ 54 ], /* "ignore" */
		(ppsym*)NULL );
	prod[ 60 ]->emit = 40;

	prod[ 61 ] = pp_prod_create( g, sym[ 53 ] /* termflag */,
		sym[ 26 ], /* emit */
		(ppsym*)NULL );

	prod[ 62 ] = pp_prod_create( g, sym[ 56 ] /* termflag+ */,
		sym[ 56 ], /* termflag+ */
		sym[ 53 ], /* termflag */
		(ppsym*)NULL );

	prod[ 63 ] = pp_prod_create( g, sym[ 56 ] /* termflag+ */,
		sym[ 53 ], /* termflag */
		(ppsym*)NULL );

	prod[ 64 ] = pp_prod_create( g, sym[ 55 ] /* termflags */,
		sym[ 55 ], /* termflags */
		sym[ 37 ], /* '%' */
		sym[ 56 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 65 ] = pp_prod_create( g, sym[ 55 ] /* termflags */,
		sym[ 37 ], /* '%' */
		sym[ 56 ], /* termflag+ */
		(ppsym*)NULL );

	prod[ 66 ] = pp_prod_create( g, sym[ 59 ] /* ident? */,
		sym[ 6 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	prod[ 67 ] = pp_prod_create( g, sym[ 59 ] /* ident? */,
		(ppsym*)NULL );

	prod[ 68 ] = pp_prod_create( g, sym[ 60 ] /* termdef' */,
		sym[ 3 ], /* /'[^']*'/ */
		(ppsym*)NULL );

	prod[ 69 ] = pp_prod_create( g, sym[ 60 ] /* termdef' */,
		sym[ 4 ], /* /"[^"]*"/ */
		(ppsym*)NULL );

	prod[ 70 ] = pp_prod_create( g, sym[ 60 ] /* termdef' */,
		sym[ 5 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	prod[ 71 ] = pp_prod_create( g, sym[ 61 ] /* termflags? */,
		sym[ 55 ], /* termflags */
		(ppsym*)NULL );

	prod[ 72 ] = pp_prod_create( g, sym[ 61 ] /* termflags? */,
		(ppsym*)NULL );

	prod[ 73 ] = pp_prod_create( g, sym[ 57 ] /* termdef */,
		sym[ 58 ], /* '$' */
		sym[ 59 ], /* ident? */
		sym[ 60 ], /* termdef' */
		sym[ 61 ], /* termflags? */
		sym[ 52 ], /* ';' */
		(ppsym*)NULL );
	prod[ 73 ]->emit = 35;

	prod[ 74 ] = pp_prod_create( g, sym[ 62 ] /* gflag */,
		sym[ 63 ], /* "emitall" */
		(ppsym*)NULL );
	prod[ 74 ]->emit = 41;

	prod[ 75 ] = pp_prod_create( g, sym[ 62 ] /* gflag */,
		sym[ 64 ], /* "emitnone" */
		(ppsym*)NULL );
	prod[ 75 ]->emit = 41;

	prod[ 76 ] = pp_prod_create( g, sym[ 62 ] /* gflag */,
		sym[ 65 ], /* "lrec" */
		(ppsym*)NULL );
	prod[ 76 ]->emit = 41;

	prod[ 77 ] = pp_prod_create( g, sym[ 62 ] /* gflag */,
		sym[ 66 ], /* "rrec" */
		(ppsym*)NULL );
	prod[ 77 ]->emit = 41;

	prod[ 78 ] = pp_prod_create( g, sym[ 68 ] /* gflag+ */,
		sym[ 68 ], /* gflag+ */
		sym[ 62 ], /* gflag */
		(ppsym*)NULL );

	prod[ 79 ] = pp_prod_create( g, sym[ 68 ] /* gflag+ */,
		sym[ 62 ], /* gflag */
		(ppsym*)NULL );

	prod[ 80 ] = pp_prod_create( g, sym[ 67 ] /* gflags */,
		sym[ 67 ], /* gflags */
		sym[ 37 ], /* '%' */
		sym[ 68 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 81 ] = pp_prod_create( g, sym[ 67 ] /* gflags */,
		sym[ 37 ], /* '%' */
		sym[ 68 ], /* gflag+ */
		(ppsym*)NULL );

	prod[ 82 ] = pp_prod_create( g, sym[ 69 ] /* definition */,
		sym[ 47 ], /* nontermdef */
		(ppsym*)NULL );

	prod[ 83 ] = pp_prod_create( g, sym[ 69 ] /* definition */,
		sym[ 57 ], /* termdef */
		(ppsym*)NULL );

	prod[ 84 ] = pp_prod_create( g, sym[ 69 ] /* definition */,
		sym[ 67 ], /* gflags */
		(ppsym*)NULL );

	prod[ 85 ] = pp_prod_create( g, sym[ 71 ] /* definition+ */,
		sym[ 71 ], /* definition+ */
		sym[ 69 ], /* definition */
		(ppsym*)NULL );

	prod[ 86 ] = pp_prod_create( g, sym[ 71 ] /* definition+ */,
		sym[ 69 ], /* definition */
		(ppsym*)NULL );

	prod[ 87 ] = pp_prod_create( g, sym[ 70 ] /* grammar */,
		sym[ 71 ], /* definition+ */
		(ppsym*)NULL );


}
