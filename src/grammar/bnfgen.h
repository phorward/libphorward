pgterm*	T_star;
pgterm*	T_COLON;
pgterm*	T_plus;
pgterm*	T_REGEX;
pgterm*	T_semicolon;
pgterm*	T_pipe;
pgterm*	T_atIDENT;
pgterm*	T_equal;
pgterm*	T_STRING;
pgterm*	T_IDENT;
pgterm*	T_quest;
pgterm*	T_at;
pgnonterm*	N_termdef;
pgnonterm*	N_productions;
pgnonterm*	N_nontermdef;
pgnonterm*	N_production;
pgnonterm*	N_term;
pgnonterm*	N_nonterm;
pgnonterm*	N_named_symbol;
pgnonterm*	N_symbol;
pgnonterm*	N_grammar;
pgnonterm*	N_rhs;
pgnonterm*	N_definitions;
pgnonterm*	N_definition;
pgnonterm*	N_mod_symbol;
pgprod*	P_termdef_1;
pgprod*	P_termdef_2;
pgprod*	P_termdef_3;
pgprod*	P_productions_1;
pgprod*	P_productions_2;
pgprod*	P_nontermdef_1;
pgprod*	P_nontermdef_2;
pgprod*	P_production_1;
pgprod*	P_production_2;
pgprod*	P_term_1;
pgprod*	P_nonterm_1;
pgprod*	P_named_symbol_1;
pgprod*	P_named_symbol_2;
pgprod*	P_symbol_1;
pgprod*	P_symbol_2;
pgprod*	P_grammar_1;
pgprod*	P_grammar_2;
pgprod*	P_rhs_1;
pgprod*	P_rhs_2;
pgprod*	P_definitions_1;
pgprod*	P_definitions_2;
pgprod*	P_definition_1;
pgprod*	P_definition_2;
pgprod*	P_mod_symbol_1;
pgprod*	P_mod_symbol_2;
pgprod*	P_mod_symbol_3;
pgprod*	P_mod_symbol_4;

/* Terminals */
T_star = pg_term_create( g, "*", (char*)NULL );
T_COLON = pg_term_create( g, "COLON", ":" );
T_plus = pg_term_create( g, "+", (char*)NULL );
T_REGEX = pg_term_create( g, "REGEX", "/([^\\]|\\/)*/" );
T_semicolon = pg_term_create( g, ";", (char*)NULL );
T_pipe = pg_term_create( g, "|", (char*)NULL );
T_atIDENT = pg_term_create( g, "@IDENT", (char*)NULL );
T_equal = pg_term_create( g, "=", (char*)NULL );
T_STRING = pg_term_create( g, "STRING", "\".*\"" );
T_IDENT = pg_term_create( g, "IDENT", "[A-Za-z_][A-Za-z0-9_]*" );
T_quest = pg_term_create( g, "?", (char*)NULL );
T_at = pg_term_create( g, "@", (char*)NULL );

/* Nonterminals */
N_termdef = pg_nonterm_create( g, "termdef" );
pg_nonterm_set_emit( N_termdef, TRUE );N_productions = pg_nonterm_create( g, "productions" );
N_nontermdef = pg_nonterm_create( g, "nontermdef" );
pg_nonterm_set_emit( N_nontermdef, TRUE );N_production = pg_nonterm_create( g, "production" );
N_term = pg_nonterm_create( g, "term" );
pg_nonterm_set_emit( N_term, TRUE );N_nonterm = pg_nonterm_create( g, "nonterm" );
pg_nonterm_set_emit( N_nonterm, TRUE );N_named_symbol = pg_nonterm_create( g, "named_symbol" );
N_symbol = pg_nonterm_create( g, "symbol" );
N_grammar = pg_nonterm_create( g, "grammar" );
N_rhs = pg_nonterm_create( g, "rhs" );
pg_nonterm_set_emit( N_rhs, TRUE );N_definitions = pg_nonterm_create( g, "definitions" );
N_definition = pg_nonterm_create( g, "definition" );
N_mod_symbol = pg_nonterm_create( g, "mod_symbol" );
pg_nonterm_set_emit( N_mod_symbol, TRUE );
/* Productions for termdef */
P_termdef_1 = pg_prod_create( g, N_termdef, T_at, T_IDENT, T_REGEX, (pgsymbol*)NULL );
P_termdef_2 = pg_prod_create( g, N_termdef, T_at, T_IDENT, T_STRING, (pgsymbol*)NULL );
P_termdef_3 = pg_prod_create( g, N_termdef, T_at, T_IDENT, (pgsymbol*)NULL );

/* Productions for productions */
P_productions_1 = pg_prod_create( g, N_productions, N_productions, T_pipe, N_rhs, (pgsymbol*)NULL );
P_productions_2 = pg_prod_create( g, N_productions, N_rhs, (pgsymbol*)NULL );

/* Productions for nontermdef */
P_nontermdef_1 = pg_prod_create( g, N_nontermdef, T_IDENT, T_COLON, N_productions, (pgsymbol*)NULL );
P_nontermdef_2 = pg_prod_create( g, N_nontermdef, T_IDENT, (pgsymbol*)NULL );

/* Productions for production */
P_production_1 = pg_prod_create( g, N_production, N_production, N_named_symbol, (pgsymbol*)NULL );
P_production_2 = pg_prod_create( g, N_production, N_named_symbol, (pgsymbol*)NULL );

/* Productions for term */
P_term_1 = pg_prod_create( g, N_term, T_at, T_IDENT, (pgsymbol*)NULL );

/* Productions for nonterm */
P_nonterm_1 = pg_prod_create( g, N_nonterm, T_IDENT, (pgsymbol*)NULL );

/* Productions for named_symbol */
P_named_symbol_1 = pg_prod_create( g, N_named_symbol, N_mod_symbol, T_equal, T_atIDENT, (pgsymbol*)NULL );
P_named_symbol_2 = pg_prod_create( g, N_named_symbol, N_mod_symbol, (pgsymbol*)NULL );

/* Productions for symbol */
P_symbol_1 = pg_prod_create( g, N_symbol, N_term, (pgsymbol*)NULL );
P_symbol_2 = pg_prod_create( g, N_symbol, N_nonterm, (pgsymbol*)NULL );

/* Productions for grammar */
P_grammar_1 = pg_prod_create( g, N_grammar, N_definitions, (pgsymbol*)NULL );
P_grammar_2 = pg_prod_create( g, N_grammar, (pgsymbol*)NULL );

/* Productions for rhs */
P_rhs_1 = pg_prod_create( g, N_rhs, N_production, (pgsymbol*)NULL );
P_rhs_2 = pg_prod_create( g, N_rhs, (pgsymbol*)NULL );

/* Productions for definitions */
P_definitions_1 = pg_prod_create( g, N_definitions, N_definitions, N_definition, (pgsymbol*)NULL );
P_definitions_2 = pg_prod_create( g, N_definitions, N_definition, (pgsymbol*)NULL );

/* Productions for definition */
P_definition_1 = pg_prod_create( g, N_definition, N_termdef, T_semicolon, (pgsymbol*)NULL );
P_definition_2 = pg_prod_create( g, N_definition, N_nontermdef, T_semicolon, (pgsymbol*)NULL );

/* Productions for mod_symbol */
P_mod_symbol_1 = pg_prod_create( g, N_mod_symbol, N_symbol, T_plus, (pgsymbol*)NULL );
P_mod_symbol_2 = pg_prod_create( g, N_mod_symbol, N_symbol, T_star, (pgsymbol*)NULL );
P_mod_symbol_3 = pg_prod_create( g, N_mod_symbol, N_symbol, T_quest, (pgsymbol*)NULL );
P_mod_symbol_4 = pg_prod_create( g, N_mod_symbol, N_symbol, (pgsymbol*)NULL );
