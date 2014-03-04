#include <phorward.h>

int main()
{
	pggrammar*		g;
	pgparser*		p;

	pgterminal*		i;
	pgterminal*		op_a;
	pgterminal*		op_m;
	pgterminal*		br_op;
	pgterminal*		br_cl;

	pgnonterminal*	start;
	pgnonterminal*	expr;
	pgnonterminal*	exprl;
	pgnonterminal*	term;
	pgnonterminal*	terml;
	pgnonterminal*	factor;

	pgproduction*	prod;
	pgasttype*		at;

	pgtoken*		tok;

	g = pg_grammar_create();

	i = pg_terminal_create( g, "@INTEGER", "[0-9]+" );
	op_a = pg_terminal_create( g, "+", "\\+" );
	/* op_s = pg_terminal_create( g, "-", "-" ); */
	op_m = pg_terminal_create( g, "*", "\\*" );
	/* op_d = pg_terminal_create( g, "/", "/" ); */
	br_op = pg_terminal_create( g, "(", "\\(" );
	br_cl = pg_terminal_create( g, ")", "\\)" );

	start = pg_nonterminal_create( g, "start" );
	expr = pg_nonterminal_create( g, "expr" );
	exprl = pg_nonterminal_create( g, "expr'" );
	term = pg_nonterminal_create( g, "term" );
	terml = pg_nonterminal_create( g, "term'" );
	factor = pg_nonterminal_create( g, "factor" );

	/* start */
	pg_production_create( start, expr, (pgsymbol*)NULL );
	pg_production_create( start, (pgsymbol*)NULL );

	/* expr and expr' */
	pg_production_create( expr, term, exprl, (pgsymbol*)NULL );
	pg_production_create( exprl, op_a, term, exprl, (pgsymbol*)NULL );
	pg_production_create( exprl, (pgsymbol*)NULL );

	/* term and term' */
	pg_production_create( term, factor, terml, (pgsymbol*)NULL );
	pg_production_create( terml, op_m, factor, terml, (pgsymbol*)NULL );
	pg_production_create( terml, (pgsymbol*)NULL );

	/* factor */
	pg_production_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	pg_production_create( factor, i, (pgsymbol*)NULL );


	pg_grammar_print( g );

	p = pg_parser_create( g, PGPARADIGM_LL1 );

	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING, "1+2*3+456" );
	pg_parser_parse( p );

	/*
	getchar();
	fprintf( stderr, "------------------------------\n" );
	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING, "(7+3)*2-5" );
	pg_parser_parse( p );
	*/

	return 0;
}
