#include <phorward.h>

int main()
{
	pggrammar*		g;
	pgparser*		p_lr0;
	pgparser*		p_lr1;
	pgparser*		p_lalr1;

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

	g = pg_grammar_create();

	i = pg_terminal_create( g, "INTEGER", "[0-9]+" );
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

	pg_production_create( start, expr, (pgsymbol*)NULL );
	pg_production_create( expr, expr, op_a, term, (pgsymbol*)NULL );
	pg_production_create( expr, term, (pgsymbol*)NULL );
	pg_production_create( term, term, op_m, factor, (pgsymbol*)NULL );
	pg_production_create( term, factor, (pgsymbol*)NULL );
	pg_production_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	pg_production_create( factor, i, (pgsymbol*)NULL );

	pg_grammar_print( g );

	/* LR(0) */
	p_lr0 = pg_parser_create( g, PGPARADIGM_LR0 );
	/* pg_parser_lr_closure( p_lr0 ); */

	/* LR(1) */
	p_lr1 = pg_parser_create( g, PGPARADIGM_LR1 );
	/* pg_parser_lr_closure( p_lr1 ); */

	/* LALR(1) */
	p_lalr1 = pg_parser_create( g, PGPARADIGM_LALR1 );
	pg_parser_lr_closure( p_lalr1 );

	return 0;
}
