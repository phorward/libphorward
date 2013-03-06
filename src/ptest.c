#include <phorward.h>

int main()
{
	pggrammar*		g;

	pgterminal*		i;
	pgterminal*		op_a;
	pgterminal*		op_s;
	pgterminal*		op_d;
	pgterminal*		op_m;
	pgterminal*		br_op;
	pgterminal*		br_cl;

	pgnonterminal*	expr;
	pgnonterminal*	term;
	pgnonterminal*	factor;

	g = pg_grammar_create();

	i = pg_terminal_create( g, "INTEGER", "[0-9]+" );
	op_a = pg_terminal_create( g, "+", "\\+" );
	op_s = pg_terminal_create( g, "-", "-" );
	op_d = pg_terminal_create( g, "/", "/" );
	op_m = pg_terminal_create( g, "*", "\\*" );
	br_op = pg_terminal_create( g, "(", "\\(" );
	br_cl = pg_terminal_create( g, ")", "\\)" );

	expr = pg_nonterminal_create( g, "expr" );
	term = pg_nonterminal_create( g, "term" );
	factor = pg_nonterminal_create( g, "factor" );

	pg_production_create( expr, term, op_a, term, (pgsymbol*)NULL );
	pg_production_create( expr, term, op_s, term, (pgsymbol*)NULL );

	pg_production_create( term, factor, op_d, factor, (pgsymbol*)NULL );
	pg_production_create( term, factor, op_m, factor, (pgsymbol*)NULL );

	pg_production_create( factor, i, (pgsymbol*)NULL );
	pg_production_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );

	pg_grammar_print( g );
	pg_grammar_compute_first( g, PGPARADIGM_LALR1 );
	pg_grammar_print( g );

	return 0;
}
