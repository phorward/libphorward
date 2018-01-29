/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bnf.c
Usage:	Self-hosted grammar for ppgram.
----------------------------------------------------------------------------- */

#include "phorward.h"

void pp_bnf_define( ppgram* g, plex* l )
{
	ppsym*	ident;
	ppsym*	colon;
	ppsym*	semi;
	ppsym*	pipe;
	ppsym*	brop;
	ppsym*	brcl;
	ppsym*	star;
	ppsym*	quest;
	ppsym*	plus;

	ppsym*	n_symbol;
	ppsym*	n_mod;
	ppsym*	n_seq;
	ppsym*	n_prod;
	ppsym*	n_alt;
	ppsym*	n_nonterm;
	ppsym*	n_defs;
	ppsym*	n_grammar;

	ppprod*	p;

	/*
		Ident : [a-z]+;

		symbol : '(' alternation ')' | Ident ;
		modifier : symbol | symbol '*' | symbol '+' | symbol '?' ;
		sequence : sequence modifier | modifier ;
		production : sequence | ;
		alternation : alternation '|' production | production ;

		nonterm : Ident ':' alternation ';';
		defs : defs nonterm | nonterm ;

		grammar$ : defs ;
	*/

	/* Terminals */
	ident = pp_sym_create( g, "Ident", PPFLAG_NONE );
	ident->emit = "Ident";

	colon = pp_sym_create( g, ":", PPFLAG_NONE );
	semi = pp_sym_create( g, ";", PPFLAG_NONE );
	pipe = pp_sym_create( g, "|", PPFLAG_NONE );
	brop = pp_sym_create( g, "(", PPFLAG_NONE );
	brcl = pp_sym_create( g, ")", PPFLAG_NONE );
	star = pp_sym_create( g, "*", PPFLAG_NONE );
	quest = pp_sym_create( g, "?", PPFLAG_NONE );
	plus = pp_sym_create( g, "+", PPFLAG_NONE );

	/* Nonterminals */
	n_symbol = pp_sym_create( g, "symbol", PPFLAG_NONE );
	n_symbol->emit = "symbol";

	n_mod = pp_sym_create( g, "modifier", PPFLAG_NONE );
	n_seq = pp_sym_create( g, "sequence", PPFLAG_NONE );

	n_prod = pp_sym_create( g, "production", PPFLAG_NONE );
	n_prod->emit = "production";

	n_alt = pp_sym_create( g, "alternation", PPFLAG_NONE );

	n_nonterm = pp_sym_create( g, "nonterm", PPFLAG_NONE );
	n_nonterm->emit = "nonterm";

	n_defs = pp_sym_create( g, "defs", PPFLAG_NONE );

	n_grammar = pp_sym_create( g, "grammar", PPFLAG_NONE );
	g->goal = n_grammar;

	/* Productions */

	p = pp_prod_create( g, n_symbol, brop, n_alt, brcl, (ppsym*)NULL );
	p->emit = "inline";
	p = pp_prod_create( g, n_symbol, ident, (ppsym*)NULL );

	p = pp_prod_create( g, n_mod, n_symbol, (ppsym*)NULL );
	p = pp_prod_create( g, n_mod, n_symbol, star, (ppsym*)NULL );
	p->emit = "star";
	p = pp_prod_create( g, n_mod, n_symbol, plus, (ppsym*)NULL );
	p->emit = "pos";
	p = pp_prod_create( g, n_mod, n_symbol, quest, (ppsym*)NULL );
	p->emit = "opt";

	pp_prod_create( g, n_seq, n_seq, n_mod, (ppsym*)NULL );
	pp_prod_create( g, n_seq, n_mod, (ppsym*)NULL );

	pp_prod_create( g, n_prod, n_seq, (ppsym*)NULL );
	pp_prod_create( g, n_prod, (ppsym*)NULL );

	pp_prod_create( g, n_alt, n_alt, pipe, n_prod, (ppsym*)NULL );
	pp_prod_create( g, n_alt, n_prod, (ppsym*)NULL );

	pp_prod_create( g, n_nonterm, ident, colon, n_alt, semi, (ppsym*)NULL );

	pp_prod_create( g, n_defs, n_defs, n_nonterm, (ppsym*)NULL );
	pp_prod_create( g, n_defs, n_nonterm, (ppsym*)NULL );

	pp_prod_create( g, n_grammar, n_defs, (ppsym*)NULL );

	/* Prepare grammar */
	pp_gram_prepare( g );

	/* Lexer */
	plex_define( l, "[A-Za-z_][A-Za-z0-9_]*", ident->idx + 1, 0 );
	plex_define( l, colon->name, colon->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, semi->name, semi->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, pipe->name, pipe->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, brop->name, brop->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, brcl->name, brcl->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, star->name, star->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, quest->name, quest->idx + 1, PREGEX_COMP_STATIC );
	plex_define( l, plus->name, plus->idx + 1, PREGEX_COMP_STATIC );

	plex_prepare( l );
}
