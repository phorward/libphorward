#include <phorward.h>

PG_ASTFUNC( tadd )
{
	fprintf( stderr, "ADD\n" );
}

PG_ASTFUNC( tsub )
{
	fprintf( stderr, "SUB\n" );
}

PG_ASTFUNC( tmul )
{
	fprintf( stderr, "MUL\n" );
}

PG_ASTFUNC( tdiv )
{
	fprintf( stderr, "DIV\n" );
}

int main()
{
	pggrammar*		g;
	pgparser*		p;
	pgparser*		p2;

	pgterminal*		i;
	pgterminal*		op_a;
	pgterminal*		op_s;
	pgterminal*		op_d;
	pgterminal*		op_m;
	pgterminal*		br_op;
	pgterminal*		br_cl;

	pgnonterminal*	start;
	pgnonterminal*	expr;
	pgnonterminal*	term;
	pgnonterminal*	factor;

	pgproduction*	prod;
	pgasttype*		at;

	pgtoken*		tok;

	pg_grammar_from_bnf();
	return 0;

	g = pg_grammar_create();

	i = pg_terminal_create( g, "@INTEGER", "[0-9]+" );
	op_a = pg_terminal_create( g, "+", "\\+" );
	op_s = pg_terminal_create( g, "-", "-" );
	op_d = pg_terminal_create( g, "/", "/" );
	op_m = pg_terminal_create( g, "*", "\\*" );
	br_op = pg_terminal_create( g, "(", "\\(" );
	br_cl = pg_terminal_create( g, ")", "\\)" );

	start = pg_nonterminal_create( g, "start" );
	expr = pg_nonterminal_create( g, "expr" );
	term = pg_nonterminal_create( g, "term" );
	factor = pg_nonterminal_create( g, "factor" );

	/*
	pg_production_create( start, expr, (pgsymbol*)NULL );
	pg_production_create( expr, expr, op_a, term, (pgsymbol*)NULL );
	pg_production_create( expr, expr, op_s, term, (pgsymbol*)NULL );
	pg_production_create( expr, term, (pgsymbol*)NULL );
	pg_production_create( term, term, op_m, factor, (pgsymbol*)NULL );
	pg_production_create( term, term, op_d, factor, (pgsymbol*)NULL );
	pg_production_create( term, factor, (pgsymbol*)NULL );
	pg_production_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	pg_production_create( factor, i, (pgsymbol*)NULL );
	*/

	/* start */
	prod = pg_production_create( start, expr, (pgsymbol*)NULL );

	/* --- expr --- */
	prod = pg_production_create( expr, expr, op_a, term, (pgsymbol*)NULL );

	/* AST type: add */
	at = pg_asttype_create( g, "add" );
	pg_asttype_set_bottomup( at, tadd );
	pg_production_set_asttype( prod, at );

	prod = pg_production_create( expr, expr, op_s, term, (pgsymbol*)NULL );

	/* AST type: sub */
	at = pg_asttype_create( g, "sub" );
	pg_asttype_set_bottomup( at, tsub );
	pg_production_set_asttype( prod, at );

	prod = pg_production_create( expr, term, (pgsymbol*)NULL );

	/* --- term --- */
	prod = pg_production_create( term, term, op_m, factor, (pgsymbol*)NULL );

	/* AST type: mul */
	at = pg_asttype_create( g, "mul" );
	pg_asttype_set_bottomup( at, tmul );
	pg_production_set_asttype( prod, at );

	prod = pg_production_create( term, term, op_d, factor, (pgsymbol*)NULL );

	/* AST type: div */
	at = pg_asttype_create( g, "div" );
	pg_asttype_set_bottomup( at, tdiv );
	pg_production_set_asttype( prod, at );

	prod = pg_production_create( term, factor, (pgsymbol*)NULL );

	/* --- factor --- */
	prod = pg_production_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	prod = pg_production_create( factor, i, (pgsymbol*)NULL );

	pg_grammar_print( g );

	/*
	pgterminal*		test;
	pgterminal*		test2;

	g = pg_grammar_create();
	test = pg_terminal_create( g, "INTEGER", "[0-9]+" );
	test2 = pg_terminal_create( g, "NAME", "[A-Za-z_!]+" );
	start = pg_nonterminal_create( g, "start" );
	pg_production_create( start, test, test2, (pgsymbol*)NULL );
	pg_grammar_print( g );
	*/

	p = pg_parser_create( g, PGPARADIGM_LALR1 );

	/* pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING, "1*2+3" ); */
	pg_parser_parse( p );

	/*
	getchar();
	fprintf( stderr, "------------------------------\n" );

	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING, "(7+3)*2-5" );
	pg_parser_parse( p );
	*/

	return 0;
}
