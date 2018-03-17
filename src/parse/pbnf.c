/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pbnf.c
Usage:	Parsing full parser definitions from Phorward BNF (PBNF).
----------------------------------------------------------------------------- */

#include "phorward.h"

#define NAMELEN			72
#define DERIVCHAR		'\''

static pboolean traverse_production( ppgram* gram, ppsym* lhs, ppast* node );

/* Derive name from basename */
static char* derive_name( ppgram* gram, char* base )
{
	int             i;
	static char		deriv   [ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !pp_sym_get_by_name( gram, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

#define NODE_IS( n, s ) 	( !strcmp( (n)->emit, s ) )

static ppsym* traverse_symbol( ppgram* gram, ppsym* lhs, ppast* node )
{
	pregex_ptn*		ptn;
	ppsym*			sym			= (ppsym*)NULL;
	ppast*			child;
	char			name		[ NAMELEN * 2 + 1 ];
	char			ch;

	/* fprintf( stderr, "sym >%s<\n", node->emit ); */

	if( NODE_IS( node, "inline") )
	{
		sym = pp_sym_create( gram, derive_name( gram, lhs->name ),
									PPFLAG_FREENAME | PPFLAG_DEFINED
										| PPFLAG_GENERATED );

		for( child = node->child; child; child = child->next )
			if( !traverse_production( gram, sym, child->child ) )
				return (ppsym*)NULL;
	}
	else
	{
		sprintf( name, "%.*s", (int)node->len, node->start );

		if( NODE_IS( node, "Nonterminal") )
		{
			if( !( sym = pp_sym_get_by_name( gram, name ) ) )
				sym = pp_sym_create( gram, name, PPFLAG_FREENAME );
		}
		else
		{
			if( !( sym = pp_sym_get_by_name( gram, name ) ) )
			{
				if( NODE_IS( node, "CCL" ) )
					pregex_ptn_parse( &ptn, name, PREGEX_COMP_NOERRORS );
				else if( !NODE_IS( node, "Terminal" ) )
				{
					ch = name[ pstrlen( name ) - 1 ];
					name[ pstrlen( name ) - 1 ] = '\0';

					if( NODE_IS( node, "Token") || NODE_IS( node, "String" ) )
						pregex_ptn_parse( &ptn, name + 1, PREGEX_COMP_STATIC );
					else if( NODE_IS( node, "Regex" ) )
						pregex_ptn_parse( &ptn, name + 1,
							PREGEX_COMP_NOERRORS );
					else
						MISSINGCASE;

					name[ pstrlen( name ) ] = ch;
				}

				sym = pp_sym_create( gram, name,
						PPFLAG_FREENAME | PPFLAG_DEFINED );
				sym->ptn = ptn;

				if( NODE_IS( node, "Token") )
				{
					sym->emit = pstrndup( sym->name + 1,
									strlen( sym->name ) - 2 );
					sym->flags |= PPFLAG_FREEEMIT;
				}
			}
		}
	}

	if( sym )
		sym->flags |= PPFLAG_CALLED;

	return sym;
}


static pboolean traverse_production( ppgram* gram, ppsym* lhs, ppast* node )
{
	ppsym*			sym;
	ppsym*			csym;
	ppprod*			prod;
	ppprod*			popt;
	char*			str;
	char			name		[ NAMELEN * 2 + 1 ];
	int				i;

	prod = pp_prod_create( gram, lhs, (ppsym*)NULL );

	for( ; node; node = node->next )
	{
		/* fprintf( stderr, "prod >%s<\n", node->emit ); */

		if( NODE_IS( node, "symbol" ) )
		{
			if( !( sym = traverse_symbol( gram, lhs, node->child ) ) )
				return FALSE;

			pp_prod_append( prod, sym );
		}
		else if( NODE_IS( node, "emits" ) )
		{
			if( NODE_IS( node->child, "Code" ) )
				prod->emit = pstrndup( node->child->start + 2,
										node->child->len - 4 );
			else
				prod->emit = pstrndup( node->child->start, node->child->len );

			prod->flags |= PPFLAG_FREEEMIT;
		}
		else
		{
			sym = traverse_symbol( gram, lhs, node->child->child );
			str = sym->name;

			if( NODE_IS( node, "kle" ) || NODE_IS( node, "pos" ) )
			{
				sprintf( name, "pos_%s", str );

				if( !( csym = pp_sym_get_by_name( gram, name ) ) )
				{
					csym = pp_sym_create( gram, name,
							PPFLAG_FREENAME | PPFLAG_DEFINED
								| PPFLAG_CALLED | PPFLAG_GENERATED );

					if( gram->flags & PPFLAG_PREVENTLREC )
						pp_prod_create( gram, csym, sym, csym, (ppsym*)NULL );
					else
						pp_prod_create( gram, csym, csym, sym, (ppsym*)NULL );

					pp_prod_create( gram, csym, sym, (ppsym*)NULL );
				}

				sym = csym;
			}

			if( NODE_IS( node, "opt" ) || NODE_IS( node, "kle" ) )
			{
				sprintf( name, "opt_%s", str );

				if( !( csym = pp_sym_get_by_name( gram, name ) ) )
				{
					csym = pp_sym_create( gram, name,
							PPFLAG_FREENAME | PPFLAG_DEFINED
								| PPFLAG_CALLED | PPFLAG_GENERATED );

					pp_prod_create( gram, csym, sym, (ppsym*)NULL );
					pp_prod_create( gram, csym, (ppsym*)NULL );
				}

				sym = csym;
			}

			pp_prod_append( prod, sym );
		}
	}

	/*
		Optimize productions with one generated symbol
		that was introduced via 'inline'.
	*/
	if( ( sym = pp_prod_getfromrhs( prod, 0 ) )
			&& !pp_prod_getfromrhs( prod, 1 ) )
	{
		if( PPSYM_IS_NONTERMINAL( sym )
				&& sym->flags & PPFLAG_GENERATED
					&& sym->emit && !pp_sym_getprod( sym, 1 ) )
		{
			if( sym->flags & PPFLAG_FREEEMIT )
			{
				prod->emit = pstrdup( sym->emit );
				prod->flags |= PPFLAG_FREEEMIT;
			}
			else
				prod->emit = sym->emit;

			pp_prod_remove( prod, sym );

			popt = pp_sym_getprod( sym, 0 );

			for( i = 0; ( csym = pp_prod_getfromrhs( popt, i ) ); i++ )
				pp_prod_append( prod, csym );

			pp_sym_drop( sym );
		}
	}

	return TRUE;
}

static pboolean ast_to_gram( ppgram* gram, ppast* ast )
{
	ppsym*			sym;
	ppsym*			nonterm		= (ppsym*)NULL;
	ppast* 			node;
	ppast*			child;
	char			name		[ NAMELEN * 2 + 1 ];
	char			def			[ NAMELEN * 2 + 1 ];
	char*			emit;
	pregex_ptn*		ptn;
	pboolean		flag_ignore;

	/* pp_ast_dump_short( stderr, ast ); */

	for( node = ast; node; node = node->next )
	{
		emit = (char*)NULL;
		flag_ignore = FALSE;

		/* fprintf( stderr, "gram >%s<\n", node->emit ); */

		if( NODE_IS( node, "nontermdef" ) )
		{
			child = node->child;

			sprintf( name, "%.*s", (int)child->len, child->start );

			/* Create the terminal symbol */
			if( !( nonterm = sym = pp_sym_get_by_name( gram, name ) ) )
				nonterm = sym = pp_sym_create( gram, name, PPFLAG_FREENAME );

			sym->flags |= PPFLAG_DEFINED;

			for( child = node->child->next; child; child = child->next )
			{
				if( NODE_IS( child, "flag_goal" ) )
				{
					if( !gram->goal ) /* fixme */
					{
						gram->goal = sym;
						sym->flags |= PPFLAG_CALLED;
					}
				}
				else if( NODE_IS( child, "flag_lexem" ) )
					sym->flags |= PPFLAG_LEXEM;
				else if( NODE_IS( child, "emitsdef" ) )
					sym->emit = sym->name;
				else if( NODE_IS( child, "production" ) )
				{
					if( !traverse_production( gram, sym, child->child ) )
						return FALSE;
				}
			}
		}
		else if( NODE_IS( node, "termdef" ) )
		{
			child = node->child;

			if( NODE_IS( child, "flag_ignore" ) )
			{
				flag_ignore = TRUE;
				child = child->next;
			}
			else if( NODE_IS( child, "Terminal" ) )
			{
				sprintf( name, "%.*s", (int)child->len, child->start );
				child = child->next;
			}
			else
				*name = '\0';

			if( NODE_IS( child, "emitsdef" ) )
			{
				emit = name;
				child = child->next;
			}

			if( NODE_IS( child, "CCL" ) )
			{
				sprintf( def, "%.*s", (int)child->len, child->start );
				pregex_ptn_parse( &ptn, def, PREGEX_COMP_NOERRORS );
			}
			else
			{
				sprintf( def, "%.*s", (int)child->len - 2, child->start + 1 );

				if( ! *name )
					sprintf( name, "%.*s", (int)child->len, child->start );

				if( NODE_IS( child, "Token") || NODE_IS( child, "String" ) )
					pregex_ptn_parse( &ptn, def, PREGEX_COMP_STATIC );
				else if( NODE_IS( child, "Regex" ) )
					pregex_ptn_parse( &ptn, def, PREGEX_COMP_NOERRORS );
				else
					MISSINGCASE;
			}

			child = child->next;

			if( child && NODE_IS( child, "emits" ) )
			{
				if( NODE_IS( child->child, "Code" ) )
					emit = pstrndup( child->child->start + 2,
											child->child->len - 4 );
				else
					emit = pstrndup( child->child->start, child->child->len );
			}

			/* Create the terminal symbol */
			sym = pp_sym_create( gram, name, PPFLAG_FREENAME | PPFLAG_DEFINED );
			sym->ptn = ptn;

			/* Set emitting & flags */
			if( emit == name )
				sym->emit = sym->name;
			else if( ( sym->emit = emit ) )
				sym->flags |= PPFLAG_FREEEMIT;

			if( flag_ignore )
				sym->flags |= PPFLAG_WHITESPACE;
		}
	}

	/* If there is no goal, then the last defined nonterm becomes goal symbol */
	if( !gram->goal && !( gram->goal = nonterm ) )
	{
		fprintf( stderr, "There is not any nonterminal symbol defined!\n" );
		return FALSE;
	}

	/* Look for unique goal sequence */
	if( pp_sym_getprod( gram->goal, 1 ) )
	{
		nonterm = pp_sym_create( gram,
					derive_name( gram, gram->goal->name ),
						PPFLAG_FREENAME );

		pp_prod_create( gram, nonterm, gram->goal, (ppsym*)NULL );
		gram->goal = nonterm;
	}

	/* pp_gram_dump( stderr, gram ); */

	return TRUE;
}


/** Compiles an Phorward Backus-Naur-Format definition into a parser.

//p// is the parser that receives the result of the parse.

In difference to pp_gram_from_bnf() and pp_gram_from_ebnf(),
pp_par_from_pbnf() allows for a full-fledged parser definition with
lexical analyzer-specific definitions, grammar and AST construction features.


**Grammar:**
```
// Whitespace ------------------------------------------------------------------

%skip			/[ \t\r\n]+/ ;
%skip			/\/\/[^\n]*\n/ ;
%skip			/\/\*([^*]|\*[^\/])*\*\// ;

// Terminals -------------------------------------------------------------------

@Terminal		/[A-Z][A-Za-z0-9_]*\ ;
@Nonterminal	/[a-z_][A-Za-z0-9_]*\ ;

@CCL 			/\[(\\.|[^\\\]])*\]/ ;
@String 		/'[^']*'/ ;
@Token			/"[^"]*"/ ;
@Regex 			/\/(\\.|[^\\\/])*\// ;

@Int			/[0-9]+/ ;
@Function		/[A-Za-z_][A-Za-z0-9_]*\(\)/ ;

@Flag_emit		'@([A-Za-z0-9_]+)?' ;
@Flag_goal		'$' ;
@Flag_lexem		'!' ;
@Flag_ignore	/%(ignore|skip)/ ;

// Nonterminals ----------------------------------------------------------------

@inline			: Flag_emit '(' alternation ')'
				|  '(' alternation ')'
				;

@symbol 		: Terminal | Nonterminal | CCL | String | Token
					| Regex | Function | inline ;

modifier		: ( symbol '*' )=kle
				| ( symbol '+' )=pos
				| ( symbol '?' )=opt
				| symbol
				;

sequence		: sequence modifier | modifier ;

@production	 	: sequence | ;

alternation		: alternation '|' production | production ;

@nontermdef		: Nonterminal ':' alternation ';'
				;

@termdef		: Terminal ':' ( CCL | String | Regex | Function ) ';'
				;

definition		: nontermdef
				| termdef
				;

grammar	$		: definition+ ;
```
*/
pboolean pp_gram_from_pbnf( ppgram* g, char* src )
{
	pppar*			ppar;
	ppgram*			pbnf;
	ppast*			ast;

	ppsym*			whitespace;
	ppsym*			comment;

	ppsym*			terminal;
	ppsym*			nonterminal;
	ppsym*			colon;
	ppsym*			colonequal;
	ppsym*			semi;
	ppsym*			pipe;
	ppsym*			brop;
	ppsym*			brcl;
	ppsym*			star;
	ppsym*			quest;
	ppsym*			plus;
	ppsym*			equal;

	ppsym*			code;
	ppsym*			t_ccl;
	ppsym*			t_string;
	ppsym*			t_token;
	ppsym*			t_regex;
	ppsym*			flag_goal;
	ppsym*			flag_lexem;
	ppsym*			flag_ignore;

	ppsym*			n_emit;
	ppsym*			n_opt_emit;
	ppsym*			n_inline;
	ppsym*			n_symbol;
	ppsym*			n_mod;
	ppsym*			n_seq;
	ppsym*			n_opt_seq;
	ppsym*			n_prod;
	ppsym*			n_alt;

	ppsym*			n_nonterm_flags;
	ppsym*			n_nonterm;
	ppsym*			n_term_def;
	ppsym*			n_term;

	ppsym*			n_def;
	ppsym*			n_defs;
	ppsym*			n_grammar;

	ppprod*			p;

	PROC( "pp_gram_from_pbnf" );

	if( !( g && src ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	PARMS( "g", "%p", g );
	PARMS( "src", "%s", src );

	/* Define a grammar for pbnf */

	pbnf = pp_gram_create();

	/* Terminals */

	MSG( "Defining terminals" );

	terminal = pp_sym_create( pbnf, "Terminal", PPFLAG_NONE );
	terminal->emit = "Terminal";

	nonterminal = pp_sym_create( pbnf, "Nonterminal", PPFLAG_NONE );
	nonterminal->emit = "Nonterminal";

	code = pp_sym_create( pbnf, "Code", PPFLAG_NONE );
	code->emit = "Code";

	colonequal = pp_sym_create( pbnf, ":=", PPFLAG_NONE );
	colonequal->emit = "emitsdef";

	colon = pp_sym_create( pbnf, ":", PPFLAG_NONE );
	semi = pp_sym_create( pbnf, ";", PPFLAG_NONE );
	pipe = pp_sym_create( pbnf, "|", PPFLAG_NONE );
	brop = pp_sym_create( pbnf, "(", PPFLAG_NONE );
	brcl = pp_sym_create( pbnf, ")", PPFLAG_NONE );
	star = pp_sym_create( pbnf, "*", PPFLAG_NONE );
	quest = pp_sym_create( pbnf, "?", PPFLAG_NONE );
	plus = pp_sym_create( pbnf, "+", PPFLAG_NONE );

	equal = pp_sym_create( pbnf, "=", PPFLAG_NONE );

	flag_goal = pp_sym_create( pbnf, "$", PPFLAG_NONE );
	flag_goal->emit = "flag_goal";

	flag_lexem = pp_sym_create( pbnf, "!", PPFLAG_NONE );
	flag_lexem->emit = "flag_lexem";

	flag_ignore = pp_sym_create( pbnf, "%ignore", PPFLAG_NONE );
	flag_ignore->emit = "flag_ignore";

	t_ccl = pp_sym_create( pbnf, "CCL", PPFLAG_NONE );
	t_ccl->emit = "CCL";
	t_string = pp_sym_create( pbnf, "String", PPFLAG_NONE );
	t_string->emit = "String";
	t_token = pp_sym_create( pbnf, "Token", PPFLAG_NONE );
	t_token->emit = "Token";
	t_regex = pp_sym_create( pbnf, "Regex", PPFLAG_NONE );
	t_regex->emit = "Regex";

	whitespace = pp_sym_create( pbnf, (char*)NULL, PPFLAG_WHITESPACE );
	comment = pp_sym_create( pbnf, (char*)NULL, PPFLAG_WHITESPACE );

	/* Nonterminals */
	MSG( "Nonterminals" );

	n_emit = pp_sym_create( pbnf, "emit", PPFLAG_NONE );
	n_emit->emit = "emits";

	n_opt_emit = pp_sym_create( pbnf, "opt_emit", PPFLAG_NONE );

	n_inline = pp_sym_create( pbnf, "inline", PPFLAG_NONE );
	n_inline->emit = "inline";

	n_symbol = pp_sym_create( pbnf, "symbol", PPFLAG_NONE );
	n_symbol->emit = "symbol";

	n_mod = pp_sym_create( pbnf, "modifier", PPFLAG_NONE );
	n_seq = pp_sym_create( pbnf, "sequence", PPFLAG_NONE );
	n_opt_seq = pp_sym_create( pbnf, "opt_sequence", PPFLAG_NONE );

	n_prod = pp_sym_create( pbnf, "production", PPFLAG_NONE );
	n_prod->emit = "production";

	n_alt = pp_sym_create( pbnf, "alternation", PPFLAG_NONE );

	n_nonterm_flags = pp_sym_create( pbnf, "nontermdef_flags", PPFLAG_NONE );

	n_nonterm = pp_sym_create( pbnf, "nontermdef", PPFLAG_NONE );
	n_nonterm->emit = "nontermdef";

	n_term_def = pp_sym_create( pbnf, "termdef_def", PPFLAG_NONE );

	n_term = pp_sym_create( pbnf, "termdef", PPFLAG_NONE );
	n_term->emit = "termdef";

	n_def = pp_sym_create( pbnf, "def", PPFLAG_NONE );

	n_defs = pp_sym_create( pbnf, "defs", PPFLAG_NONE );

	n_grammar = pp_sym_create( pbnf, "grammar", PPFLAG_NONE );
	pbnf->goal = n_grammar;

	/* Productions */
	MSG( "Productions" );

	pp_prod_create( pbnf, n_emit, equal, nonterminal, (ppsym*)NULL );
	pp_prod_create( pbnf, n_emit, equal, terminal, (ppsym*)NULL );
	pp_prod_create( pbnf, n_emit, equal, code, (ppsym*)NULL );

	pp_prod_create( pbnf, n_opt_emit, n_emit, (ppsym*)NULL );
	pp_prod_create( pbnf, n_opt_emit, (ppsym*)NULL );

	pp_prod_create( pbnf, n_inline, brop, n_alt, brcl, (ppsym*)NULL );

	pp_prod_create( pbnf, n_symbol, terminal, (ppsym*)NULL );
	pp_prod_create( pbnf, n_symbol, nonterminal, (ppsym*)NULL );
	pp_prod_create( pbnf, n_symbol, t_ccl, (ppsym*)NULL );
	pp_prod_create( pbnf, n_symbol, t_string, (ppsym*)NULL );
	pp_prod_create( pbnf, n_symbol, t_token, (ppsym*)NULL );
	pp_prod_create( pbnf, n_symbol, t_regex, (ppsym*)NULL );

	pp_prod_create( pbnf, n_mod, n_symbol, (ppsym*)NULL );

	p = pp_prod_create( pbnf, n_mod, n_symbol, star, (ppsym*)NULL );
	p->emit = "kle";

	p = pp_prod_create( pbnf, n_mod, n_symbol, plus, (ppsym*)NULL );
	p->emit = "pos";

	p = pp_prod_create( pbnf, n_mod, n_symbol, quest, (ppsym*)NULL );
	p->emit = "opt";

	pp_prod_create( pbnf, n_seq, n_seq, n_mod, (ppsym*)NULL );
	pp_prod_create( pbnf, n_seq, n_mod, (ppsym*)NULL );

	pp_prod_create( pbnf, n_opt_seq, n_seq, (ppsym*)NULL );
	pp_prod_create( pbnf, n_opt_seq, (ppsym*)NULL );

	pp_prod_create( pbnf, n_prod, n_opt_seq, n_opt_emit, (ppsym*)NULL );

	pp_prod_create( pbnf, n_alt, n_alt, pipe, n_prod, (ppsym*)NULL );
	pp_prod_create( pbnf, n_alt, n_prod, (ppsym*)NULL );

		/* nontermdef */

	pp_prod_create( pbnf, n_nonterm_flags,
						n_nonterm_flags, flag_goal, (ppsym*)NULL );
	pp_prod_create( pbnf, n_nonterm_flags,
						n_nonterm_flags, flag_lexem, (ppsym*)NULL );
	pp_prod_create( pbnf, n_nonterm_flags, (ppsym*)NULL );

	pp_prod_create( pbnf, n_nonterm, nonterminal,
						n_nonterm_flags, colon,
							n_alt, semi, (ppsym*)NULL );
	pp_prod_create( pbnf, n_nonterm, nonterminal,
						n_nonterm_flags, colonequal,
							n_alt, semi, (ppsym*)NULL );

		/* termdef */

	pp_prod_create( pbnf, n_term_def, t_ccl, (ppsym*)NULL );
	pp_prod_create( pbnf, n_term_def, t_string, (ppsym*)NULL );
	pp_prod_create( pbnf, n_term_def, t_token, (ppsym*)NULL );
	pp_prod_create( pbnf, n_term_def, t_regex, (ppsym*)NULL );

	pp_prod_create( pbnf, n_term, terminal, colon, n_term_def,
						n_opt_emit, semi, (ppsym*)NULL );
	pp_prod_create( pbnf, n_term, terminal, colonequal, n_term_def, semi,
						(ppsym*)NULL );
	pp_prod_create( pbnf, n_term, flag_ignore, n_term_def, semi,
						(ppsym*)NULL );

	pp_prod_create( pbnf, n_def, n_term, (ppsym*)NULL );
	pp_prod_create( pbnf, n_def, n_nonterm, (ppsym*)NULL );

	pp_prod_create( pbnf, n_defs, n_defs, n_def, (ppsym*)NULL );
	pp_prod_create( pbnf, n_defs, n_def, (ppsym*)NULL );

	pp_prod_create( pbnf, n_grammar, n_defs, (ppsym*)NULL );

	/* Setup a parser */
	ppar = pp_par_create( pbnf );
	PP_GRAM_DUMP( pbnf );

	/* Lexer */
	pp_par_lex( ppar, whitespace, "[ \t\r\n]+", 0 );
	pp_par_lex( ppar, comment, "/\\*([^*]|\\*[^/])*\\*/|//[^\n]*\n", 0 );

	pp_par_lex( ppar, terminal, "[A-Z_][A-Za-z0-9_]*", 0 );
	pp_par_lex( ppar, nonterminal, "[a-z_][A-Za-z0-9_]*", 0 );
	pp_par_lex( ppar, code, "{{.*}}", 0 );

	pp_par_lex( ppar, t_ccl, "\\[(\\.|[^\\\\\\]])*\\]", 0 );
	pp_par_lex( ppar, t_string, "'[^']*'", 0 );
	pp_par_lex( ppar, t_token, "\"[^\"]*\"", 0 );
	pp_par_lex( ppar, t_regex, "/(\\.|[^\\/])*/", 0 );

	pp_par_lex( ppar, flag_ignore, "%(ignore|skip)", 0 );

	pp_par_autolex( ppar );

	if( !pp_par_parse( &ast, ppar, src ) )
	{
		pp_par_free( ppar );
		pp_gram_free( pbnf );
		return FALSE;
	}

	PP_AST_DUMP( ast );

	if( !ast_to_gram( g, ast ) )
		return FALSE;

	PP_GRAM_DUMP( g );

	pp_par_free( ppar );
	pp_gram_free( pbnf );
	pp_ast_free( ast );

	return TRUE;
}


/* cc -o pbnf -I .. pbnf.c ../libphorward.a */
#if 0
int main( int argc, char** argv )
{
	ppgram*	g;
	char*	s;

	if( argc < 2 )
	{
		printf( "%s FILE-OR-GRAMMAR\n", *argv );
		return 0;
	}

	g = pp_gram_create();

	s = argv[1];
	pfiletostr( &s, s );

	pp_gram_from_pbnf( g, s );

	pp_gram_prepare( g );
	pp_gram_dump( stderr, g );

	if( s != argv[1] )
		pfree( s );

	return 0;
}
#endif
