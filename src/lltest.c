#include <phorward.h>

int main()
{
	pggrammar*		g;
	pgparser*		p;

	pgterm*		i;
	pgterm*		op_a;
	pgterm*		op_m;
	pgterm*		br_op;
	pgterm*		br_cl;

	pgnonterm*	start;
	pgnonterm*	expr;
	pgnonterm*	exprl;
	pgnonterm*	term;
	pgnonterm*	terml;
	pgnonterm*	factor;

	pgprod*	prod;

	pgtoken*		tok;
	pgast*			ast;

	g = pg_grammar_create();

	i = pg_term_create( g, "@INTEGER", "[0-9]+" );
	op_a = pg_term_create( g, "+", "\\+" );
	/* op_s = pg_term_create( g, "-", "-" ); */
	op_m = pg_term_create( g, "*", "\\*" );
	/* op_d = pg_term_create( g, "/", "/" ); */
	br_op = pg_term_create( g, "(", "\\(" );
	br_cl = pg_term_create( g, ")", "\\)" );

	start = pg_nonterm_create( g, "start" );
	expr = pg_nonterm_create( g, "expr" );
	pg_nonterm_set_emit( expr, TRUE );
	exprl = pg_nonterm_create( g, "expr'" );
	term = pg_nonterm_create( g, "term" );
	pg_nonterm_set_emit( term, TRUE );
	terml = pg_nonterm_create( g, "term'" );
	factor = pg_nonterm_create( g, "factor" );

	/* start */
	pg_prod_create( start, expr, (pgsymbol*)NULL );
	pg_prod_create( start, (pgsymbol*)NULL );

	/* expr and expr' */
	pg_prod_create( expr, term, exprl, (pgsymbol*)NULL );
	pg_prod_create( exprl, op_a, term, exprl, (pgsymbol*)NULL );
	pg_prod_create( exprl, (pgsymbol*)NULL );

	/* term and term' */
	pg_prod_create( term, factor, terml, (pgsymbol*)NULL );
	pg_prod_create( terml, op_m, factor, terml, (pgsymbol*)NULL );
	pg_prod_create( terml, (pgsymbol*)NULL );

	/* factor */
	pg_prod_create( factor, br_op, expr, br_cl, (pgsymbol*)NULL );
	pg_prod_create( factor, i, (pgsymbol*)NULL );

	pg_grammar_print( g );

	p = pg_parser_create( g, PGPARADIGM_LL1 );

	pg_lexer_set_source( p->lexer, PG_LEX_SRCTYPE_STRING, "1+2*3" );
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
