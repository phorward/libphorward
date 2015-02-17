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
	ppsym*	sym[ 52 ];

	/* Symbols */

	sym[ 1 ] = pp_sym_create( g, PPSYMTYPE_REGEX, (char*)NULL, "[\\t\\n\\r ]+" );
	sym[ 1 ]->flags |= PPFLAG_WHITESPACE;

	sym[ 2 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ccl", "'.*'" );
	sym[ 2 ]->emit = 1;

	sym[ 3 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "string", "\".*\"" );
	sym[ 3 ]->emit = 2;

	sym[ 4 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "regex", "/(\\\\.|[^/\\\\])*/" );
	sym[ 4 ]->emit = 3;

	sym[ 5 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "ident", "[A-Z_a-z][0-9A-Z_a-z]*" );
	sym[ 5 ]->emit = 10;

	sym[ 6 ] = pp_sym_create( g, PPSYMTYPE_REGEX, "int", "[0-9]+" );
	sym[ 6 ]->emit = 11;

	sym[ 7 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "symbol", (char*)NULL );
	sym[ 7 ]->emit = 20;

	sym[ 8 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "inline", (char*)NULL );
	sym[ 8 ]->emit = 32;

	sym[ 9 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "mod_kleene", (char*)NULL );
	sym[ 9 ]->emit = 25;

	sym[ 10 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "*" );

	sym[ 11 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "mod_positive", (char*)NULL );
	sym[ 11 ]->emit = 26;

	sym[ 12 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "+" );

	sym[ 13 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "mod_optional", (char*)NULL );
	sym[ 13 ]->emit = 27;

	sym[ 14 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "?" );

	sym[ 15 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "modifier", (char*)NULL );

	sym[ 16 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "sequence", (char*)NULL );

	sym[ 17 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternative", (char*)NULL );
	sym[ 17 ]->emit = 30;

	sym[ 18 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "alternation", (char*)NULL );

	sym[ 19 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "|" );

	sym[ 20 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emit", (char*)NULL );
	sym[ 20 ]->emit = 42;

	sym[ 21 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emit" );

	sym[ 22 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "int?", (char*)NULL );

	sym[ 23 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flag", (char*)NULL );
	sym[ 23 ]->emit = 40;

	sym[ 24 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "noemit" );

	sym[ 25 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "goal" );

	sym[ 26 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "ignore" );

	sym[ 27 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lexem" );

	sym[ 28 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emitflag", (char*)NULL );

	sym[ 29 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags", (char*)NULL );

	sym[ 30 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "%" );

	sym[ 31 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "emitflag+", (char*)NULL );

	sym[ 32 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "nontermdef", (char*)NULL );
	sym[ 32 ]->emit = 31;

	sym[ 33 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ":" );

	sym[ 34 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "flags?", (char*)NULL );

	sym[ 35 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ";" );

	sym[ 36 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "(" );

	sym[ 37 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, ")" );

	sym[ 38 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef", (char*)NULL );
	sym[ 38 ]->emit = 35;

	sym[ 39 ] = pp_sym_create( g, PPSYMTYPE_CCL, (char*)NULL, "$" );

	sym[ 40 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "ident?", (char*)NULL );

	sym[ 41 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "termdef'", (char*)NULL );

	sym[ 42 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag", (char*)NULL );
	sym[ 42 ]->emit = 41;

	sym[ 43 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitall" );

	sym[ 44 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "emitnone" );

	sym[ 45 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "lrec" );

	sym[ 46 ] = pp_sym_create( g, PPSYMTYPE_STRING, (char*)NULL, "rrec" );

	sym[ 47 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflags", (char*)NULL );

	sym[ 48 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "gflag+", (char*)NULL );

	sym[ 49 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition", (char*)NULL );

	sym[ 50 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "grammar", (char*)NULL );
	g->goal = sym[ 50 ];

	sym[ 51 ] = pp_sym_create( g, PPSYMTYPE_NONTERM, "definition+", (char*)NULL );


	/* Productions */

	pp_prod_create( g, sym[ 7 ] /* symbol */,
		sym[ 8 ], /* inline */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 7 ] /* symbol */,
		sym[ 4 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 7 ] /* symbol */,
		sym[ 3 ], /* /".*"/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 7 ] /* symbol */,
		sym[ 2 ], /* /'.*'/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 7 ] /* symbol */,
		sym[ 5 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 9 ] /* mod_kleene */,
		sym[ 7 ], /* symbol */
		sym[ 10 ], /* '*' */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 11 ] /* mod_positive */,
		sym[ 7 ], /* symbol */
		sym[ 12 ], /* '+' */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 13 ] /* mod_optional */,
		sym[ 7 ], /* symbol */
		sym[ 14 ], /* '?' */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 7 ], /* symbol */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 13 ], /* mod_optional */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 11 ], /* mod_positive */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 15 ] /* modifier */,
		sym[ 9 ], /* mod_kleene */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 16 ] /* sequence */,
		sym[ 15 ], /* modifier */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 16 ] /* sequence */,
		sym[ 16 ], /* sequence */
		sym[ 15 ], /* modifier */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 17 ] /* alternative */,
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 17 ] /* alternative */,
		sym[ 16 ], /* sequence */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 18 ] /* alternation */,
		sym[ 17 ], /* alternative */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 18 ] /* alternation */,
		sym[ 18 ], /* alternation */
		sym[ 19 ], /* '|' */
		sym[ 17 ], /* alternative */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 22 ] /* int? */,
		sym[ 6 ], /* /[0-9]+/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 22 ] /* int? */,
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 20 ] /* emit */,
		sym[ 21 ], /* "emit" */
		sym[ 22 ], /* int? */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 23 ] /* flag */,
		sym[ 27 ], /* "lexem" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 23 ] /* flag */,
		sym[ 26 ], /* "ignore" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 23 ] /* flag */,
		sym[ 25 ], /* "goal" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 23 ] /* flag */,
		sym[ 24 ], /* "noemit" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 28 ] /* emitflag */,
		sym[ 23 ], /* flag */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 28 ] /* emitflag */,
		sym[ 20 ], /* emit */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 31 ] /* emitflag+ */,
		sym[ 31 ], /* emitflag+ */
		sym[ 28 ], /* emitflag */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 31 ] /* emitflag+ */,
		sym[ 28 ], /* emitflag */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 29 ] /* flags */,
		sym[ 30 ], /* '%' */
		sym[ 31 ], /* emitflag+ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 29 ] /* flags */,
		sym[ 29 ], /* flags */
		sym[ 30 ], /* '%' */
		sym[ 31 ], /* emitflag+ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 34 ] /* flags? */,
		sym[ 29 ], /* flags */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 34 ] /* flags? */,
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 32 ] /* nontermdef */,
		sym[ 5 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		sym[ 33 ], /* ':' */
		sym[ 18 ], /* alternation */
		sym[ 34 ], /* flags? */
		sym[ 35 ], /* ';' */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 8 ] /* inline */,
		sym[ 36 ], /* '(' */
		sym[ 18 ], /* alternation */
		sym[ 37 ], /* ')' */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 40 ] /* ident? */,
		sym[ 5 ], /* /[A-Z_a-z][0-9A-Z_a-z]*\/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 40 ] /* ident? */,
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 41 ] /* termdef' */,
		sym[ 4 ], /* /\/(\\\\.|[^\/\\\\])*\// */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 41 ] /* termdef' */,
		sym[ 3 ], /* /".*"/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 41 ] /* termdef' */,
		sym[ 2 ], /* /'.*'/ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 38 ] /* termdef */,
		sym[ 39 ], /* '$' */
		sym[ 40 ], /* ident? */
		sym[ 41 ], /* termdef' */
		sym[ 34 ], /* flags? */
		sym[ 35 ], /* ';' */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 42 ] /* gflag */,
		sym[ 46 ], /* "rrec" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 42 ] /* gflag */,
		sym[ 45 ], /* "lrec" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 42 ] /* gflag */,
		sym[ 44 ], /* "emitnone" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 42 ] /* gflag */,
		sym[ 43 ], /* "emitall" */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 48 ] /* gflag+ */,
		sym[ 48 ], /* gflag+ */
		sym[ 42 ], /* gflag */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 48 ] /* gflag+ */,
		sym[ 42 ], /* gflag */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 47 ] /* gflags */,
		sym[ 30 ], /* '%' */
		sym[ 48 ], /* gflag+ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 47 ] /* gflags */,
		sym[ 47 ], /* gflags */
		sym[ 30 ], /* '%' */
		sym[ 48 ], /* gflag+ */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 49 ] /* definition */,
		sym[ 47 ], /* gflags */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 49 ] /* definition */,
		sym[ 38 ], /* termdef */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 49 ] /* definition */,
		sym[ 32 ], /* nontermdef */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 51 ] /* definition+ */,
		sym[ 51 ], /* definition+ */
		sym[ 49 ], /* definition */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 51 ] /* definition+ */,
		sym[ 49 ], /* definition */
		(ppsym*)NULL );

	pp_prod_create( g, sym[ 50 ] /* grammar */,
		sym[ 51 ], /* definition+ */
		(ppsym*)NULL );


}
