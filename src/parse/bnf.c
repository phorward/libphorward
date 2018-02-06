/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	bnf.c
Usage:	Parsing grammars from BNF definitions
----------------------------------------------------------------------------- */

#include "phorward.h"

#define NAMELEN			72
#define DERIVCHAR		'\''

static pboolean traverse_production( ppgram* g, ppsym* lhs, ppast* node );

/* Derive name from basename */
static char* derive_name( ppgram* g, char* base )
{
	int             i;
	static
	char    deriv   [ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !pp_sym_get_by_name( g, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

#define NODE_IS( n, s ) 	( !strcmp( (n)->emit, s ) )

static ppsym* traverse_symbol( ppgram* g, ppsym* lhs, ppast* node )
{
	ppsym*		sym			= (ppsym*)NULL;
	ppast*		child;
	char		name		[ NAMELEN * 2 + 1 ];

	/* fprintf( stderr, "sym >%s<\n", node->emit ); */

	if( NODE_IS( node, "inline") )
	{
		sym = pp_sym_create( g, derive_name( g, lhs->name ),
				PPFLAG_FREENAME | PPFLAG_DEFINED | PPFLAG_GENERATED );

		for( child = node->child; child; child = child->next )
			if( !traverse_production( g, sym, child->child ) )
				return (ppsym*)NULL;
	}
	else if( NODE_IS( node, "Terminal") || NODE_IS( node, "Nonterminal") )
	{
		sprintf( name, "%.*s", node->len, node->start );

		if( !( sym = pp_sym_get_by_name( g, name ) ) )
			sym = pp_sym_create( g, name, PPFLAG_FREENAME );
	}
	else
		MISSINGCASE;

	if( sym )
		sym->flags |= PPFLAG_CALLED;

	return sym;
}


static pboolean traverse_production( ppgram* g, ppsym* lhs, ppast* node )
{
	ppsym*		sym;
	ppsym*		csym;
	ppprod*		prod;
	ppprod*		popt;
	ppast*		child;
	char*		str;
	char		name		[ NAMELEN * 2 + 1 ];
	int			i;
	plistel*	e;

	prod = pp_prod_create( g, lhs, (ppsym*)NULL );

	for( ; node; node = node->next )
	{
		/* fprintf( stderr, "prod >%s<\n", node->emit ); */

		if( NODE_IS( node, "symbol" ) )
		{
			if( !( sym = traverse_symbol( g, lhs, node->child ) ) )
				return FALSE;

			pp_prod_append( prod, sym );
		}
		else
		{
			sym = traverse_symbol( g, lhs, node->child->child );
			str = sym->name;

			if( NODE_IS( node, "kle" ) || NODE_IS( node, "pos" ) )
			{
				sprintf( name, "%s%c", str, PPMOD_POSITIVE );

				if( !( csym = pp_sym_get_by_name( g, name ) ) )
				{
					csym = pp_sym_create( g, name,
								PPFLAG_FREENAME | PPFLAG_DEFINED
									| PPFLAG_CALLED | PPFLAG_GENERATED );

					if( g->flags & PPFLAG_PREVENTLREC )
						pp_prod_create( g, csym, sym, csym, (ppsym*)NULL );
					else
						pp_prod_create( g, csym, csym, sym, (ppsym*)NULL );

					pp_prod_create( g, csym, sym, (ppsym*)NULL );
				}

				sym = csym;
			}

			if( NODE_IS( node, "opt" ) || NODE_IS( node, "kle" ) )
			{
				sprintf( name, "%s%c", str, PPMOD_OPTIONAL );

				if( !( csym = pp_sym_get_by_name( g, name ) ) )
				{
					csym = pp_sym_create( g, name,
								PPFLAG_FREENAME | PPFLAG_DEFINED
									| PPFLAG_CALLED | PPFLAG_GENERATED );

					pp_prod_create( g, csym, sym, (ppsym*)NULL );
					pp_prod_create( g, csym, (ppsym*)NULL );
				}

				sym = csym;
			}

			pp_prod_append( prod, sym );
		}
	}

	return TRUE;
}

static pboolean ast_to_gram( ppgram* g, ppast* ast )
{
	ppsym*		sym			= (ppsym*)NULL;
	ppast*		child;
	char		name		[ NAMELEN * 2 + 1 ];

	for( ast; ast; ast = ast->next )
	{
		if( NODE_IS( ast, "nonterm" ) )
		{
			/* Get nonterminal's name */
			child = ast->child;
			sprintf( name, "%.*s", child->len, child->start );

			/* Create the non-terminal symbol */
			if( !( sym = pp_sym_get_by_name( g, name ) ) )
				sym = pp_sym_create( g, name,
						PPFLAG_FREENAME | PPFLAG_DEFINED );

			for( child = child->next; child; child = child->next )
			{
				if( NODE_IS( child, "production" ) )
				{
					if( !traverse_production( g, sym, child->child ) )
						return FALSE;
				}
				else
					MISSINGCASE;
			}
		}
		else
			MISSINGCASE;
	}

	/* If there is no goal, then the last defined nonterminal
		becomes the goal symbol */
	if( !g->goal )
		g->goal = sym;

	/* Look for unique goal sequence; if this is not the case, wrap it with
		another, generated nonterminal. */
	if( pp_sym_getprod( g->goal, 1 ) )
	{
		sym = pp_sym_create( g, derive_name( g, g->goal->name ),
				PPFLAG_FREENAME | PPFLAG_DEFINED
					| PPFLAG_CALLED | PPFLAG_GENERATED );

		pp_prod_create( g, sym, g->goal, (ppsym*)NULL );
		g->goal = sym;
	}

	return TRUE;
}

/** Compiles a grammar definition into a grammar.

//g// is the grammar that receives the result of the parse.
//source// is the BNF definition string that defines the grammar.
*/
pboolean pp_gram_from_bnf( ppgram* g, char* source )
{
	pparser*	par;
	ppgram*		bnf;
	char*		s = source;
	char*		e;
	ppast*		ast;

	ppsym*		terminal;
	ppsym*		nonterminal;
	ppsym*		colon;
	ppsym*		semi;
	ppsym*		pipe;
	ppsym*		brop;
	ppsym*		brcl;
	ppsym*		star;
	ppsym*		quest;
	ppsym*		plus;

	ppsym*		n_symbol;
	ppsym*		n_mod;
	ppsym*		n_seq;
	ppsym*		n_prod;
	ppsym*		n_alt;
	ppsym*		n_nonterm;
	ppsym*		n_defs;
	ppsym*		n_grammar;

	ppprod*		p;

	if( !( g && source ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Define a grammar for BNF */

	bnf = pp_gram_create();

	/*
		Terminal : [^a-z_][a-zA-Z_]*;
		Nonterminal : [a-z_]+;

		symbol : '(' alternation ')' | Terminal | Nonterminal ;
		modifier : symbol | symbol '*' | symbol '+' | symbol '?' ;
		sequence : sequence modifier | modifier ;
		production : sequence | ;
		alternation : alternation '|' production | production ;

		nonterm : Nonterminal ':' alternation ';';
		defs : defs nonterm | nonterm ;

		grammar$ : defs ;
	*/

	/* Terminals */
	terminal = pp_sym_create( bnf, "Terminal", PPFLAG_NONE );
	terminal->emit = "Terminal";

	nonterminal = pp_sym_create( bnf, "Nonterminal", PPFLAG_NONE );
	nonterminal->emit = "Nonterminal";

	colon = pp_sym_create( bnf, ":", PPFLAG_NONE );
	semi = pp_sym_create( bnf, ";", PPFLAG_NONE );
	pipe = pp_sym_create( bnf, "|", PPFLAG_NONE );
	brop = pp_sym_create( bnf, "(", PPFLAG_NONE );
	brcl = pp_sym_create( bnf, ")", PPFLAG_NONE );
	star = pp_sym_create( bnf, "*", PPFLAG_NONE );
	quest = pp_sym_create( bnf, "?", PPFLAG_NONE );
	plus = pp_sym_create( bnf, "+", PPFLAG_NONE );

	/* Nonterminals */
	n_symbol = pp_sym_create( bnf, "symbol", PPFLAG_NONE );
	n_symbol->emit = "symbol";

	n_mod = pp_sym_create( bnf, "modifier", PPFLAG_NONE );
	n_seq = pp_sym_create( bnf, "sequence", PPFLAG_NONE );

	n_prod = pp_sym_create( bnf, "production", PPFLAG_NONE );
	n_prod->emit = "production";

	n_alt = pp_sym_create( bnf, "alternation", PPFLAG_NONE );

	n_nonterm = pp_sym_create( bnf, "nonterm", PPFLAG_NONE );
	n_nonterm->emit = "nonterm";

	n_defs = pp_sym_create( bnf, "defs", PPFLAG_NONE );

	n_grammar = pp_sym_create( bnf, "grammar", PPFLAG_NONE );
	bnf->goal = n_grammar;

	/* Productions */
	p = pp_prod_create( bnf, n_symbol, brop, n_alt, brcl, (ppsym*)NULL );
	p->emit = "inline";
	p = pp_prod_create( bnf, n_symbol, terminal, (ppsym*)NULL );
	p = pp_prod_create( bnf, n_symbol, nonterminal, (ppsym*)NULL );

	p = pp_prod_create( bnf, n_mod, n_symbol, (ppsym*)NULL );
	p = pp_prod_create( bnf, n_mod, n_symbol, star, (ppsym*)NULL );
	p->emit = "star";
	p = pp_prod_create( bnf, n_mod, n_symbol, plus, (ppsym*)NULL );
	p->emit = "pos";
	p = pp_prod_create( bnf, n_mod, n_symbol, quest, (ppsym*)NULL );
	p->emit = "opt";

	pp_prod_create( bnf, n_seq, n_seq, n_mod, (ppsym*)NULL );
	pp_prod_create( bnf, n_seq, n_mod, (ppsym*)NULL );

	pp_prod_create( bnf, n_prod, n_seq, (ppsym*)NULL );
	pp_prod_create( bnf, n_prod, (ppsym*)NULL );

	pp_prod_create( bnf, n_alt, n_alt, pipe, n_prod, (ppsym*)NULL );
	pp_prod_create( bnf, n_alt, n_prod, (ppsym*)NULL );

	pp_prod_create( bnf, n_nonterm, nonterminal, colon,
							n_alt, semi, (ppsym*)NULL );

	pp_prod_create( bnf, n_defs, n_defs, n_nonterm, (ppsym*)NULL );
	pp_prod_create( bnf, n_defs, n_nonterm, (ppsym*)NULL );

	pp_prod_create( bnf, n_grammar, n_defs, (ppsym*)NULL );

	/* Setup a parser */
	par = pp_parser_create( bnf );

	/* Lexer */

	pp_parser_define_token( par, terminal,
		"[^a-z_:;|()*?+ \t\r\n][^:;|()*?+ \t\r\n]*" 	/* ident */
		"|/(\\.|[^\\/])*/(@\\w*)?"						/* /regular
																expression/ */
		#if 0
		"|\"[^\"]*\"(@\\w*)?"							/* "double-quoted
																string" */
		#endif
		"|'[^']*'(@\\w*)?",								/* 'single-quoted
																string' */
		0 );
	pp_parser_define_token( par, nonterminal,
		"[a-z_][^:;|()*?+ \t\r\n]*",					/* Ident */
		0 );

	pp_parser_auto_token( par );

	if( !pp_parser_parse( &ast, par, s, &e ) )
	{
		pp_parser_free( par );
		pp_gram_free( bnf );
		return FALSE;
	}

	/* pp_ast_dump_short( stdout, ast ); */ /* DEBUG! */

	if( !ast_to_gram( g, ast ) )
		return FALSE;

	pp_parser_free( par );
	pp_gram_free( bnf );
	pp_ast_free( ast );

	return TRUE;
}
