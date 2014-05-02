#include <phorward.h>

/*
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
*/

int main()
{
	pggrammar*		g;
	pgparser*		p;
	pgparser*		p2;

	pgterm*			i;
	pgterm*			op_a;
	pgterm*			op_s;
	pgterm*			op_d;
	pgterm*			op_m;
	pgterm*			br_op;
	pgterm*			br_cl;

	pgnonterm*		start;
	pgnonterm*		expr;
	pgnonterm*		term;
	pgnonterm*		factor;

	pgprod*			prod;

	pgtoken*		tok;
	pgast*			ast;

	pg_grammar_from_bnf();
	return 0;

	g = pg_grammar_create();

	i = pg_term_create( g, "@INTEGER", "[0-9]+" );
	op_a = pg_term_create( g, "+", "\\+" );
	op_s = pg_term_create( g, "-", "-" );
	op_d = pg_term_create( g, "/", "/" );
	op_m = pg_term_create( g, "*", "\\*" );
	br_op = pg_term_create( g, "(", "\\(" );
	br_cl = pg_term_create( g, ")", "\\)" );

	start = pg_nonterm_create( g, "start" );
	expr = pg_nonterm_create( g, "expr" );
	term = pg_nonterm_create( g, "term" );
	factor = pg_nonterm_create( g, "factor" );

	/*
	pg_prod_create( start, expr, (pgsymbol*)NULL );
	pg_prod_create( expr, expr, op_a, term, (pgsymbol*)NULL );
	pg_prod_create( expr, expr, op_s, term, (pgsymbol*)NULL );
	pg_prod_create( expr, term, (pgsymbol*)NULL );
	pg_prod_create( term, term, op_m, factor, (pgsymbol*)NULL );
	pg_prod_create( term, term, op_d, factor, (pgsymbol*)NULL );
	pg_prod_create( term, factor, (pgsymbol*)NULL );
	pg_prod_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	pg_prod_create( factor, i, (pgsymbol*)NULL );
	*/

	/* start */
	prod = pg_prod_create( start, expr, (pgsymbol*)NULL );

	/* --- expr --- */
	prod = pg_prod_create( expr, expr, op_a, term, (pgsymbol*)NULL );
	prod = pg_prod_create( expr, expr, op_s, term, (pgsymbol*)NULL );

	prod = pg_prod_create( expr, term, (pgsymbol*)NULL );
	prod = pg_prod_create( term, term, op_m, factor, (pgsymbol*)NULL );

	prod = pg_prod_create( term, term, op_d, factor, (pgsymbol*)NULL );
	prod = pg_prod_create( term, factor, (pgsymbol*)NULL );

	/* --- factor --- */
	prod = pg_prod_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	prod = pg_prod_create( factor, i, (pgsymbol*)NULL );

	pg_grammar_print( g );

	/*
	pgterm*		test;
	pgterm*		test2;

	g = pg_grammar_create();
	test = pg_term_create( g, "INTEGER", "[0-9]+" );
	test2 = pg_term_create( g, "NAME", "[A-Za-z_!]+" );
	start = pg_nonterm_create( g, "start" );
	pg_prod_create( start, test, test2, (pgsymbol*)NULL );
	pg_grammar_print( g );
	*/

	p = pg_parser_create( g, PGPARADIGM_LALR1 );

	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING,
		"1+2*3/4-5" );
	if( ( ast = pg_parser_parse_to_ast( p ) ) )
		pg_ast_print( ast );

	/*
	getchar();
	fprintf( stderr, "------------------------------\n" );
	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING, "(7+3)*2-5" );
	pg_parser_parse( p );
	*/

	return 0;
}
