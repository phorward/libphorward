pgterminal*	T_COLON;
pgterminal*	T_REGEX;
pgterminal*	T_semicolon;
pgterminal*	T_pipe;
pgterminal*	T_STRING;
pgterminal*	T_IDENT;
pgterminal*	T_at;
pgnonterminal*	N_termdef;
pgnonterminal*	N_productions;
pgnonterminal*	N_nontermdef;
pgnonterminal*	N_production;
pgnonterminal*	N_symbol;
pgnonterminal*	N_grammar;
pgnonterminal*	N_rhs;
pgnonterminal*	N_definitions;
pgnonterminal*	N_definition;
pgproduction*	P_termdef_1;
pgproduction*	P_termdef_2;
pgproduction*	P_termdef_3;
pgproduction*	P_productions_1;
pgproduction*	P_productions_2;
pgproduction*	P_nontermdef_1;
pgproduction*	P_nontermdef_2;
pgproduction*	P_production_1;
pgproduction*	P_production_2;
pgproduction*	P_symbol_1;
pgproduction*	P_symbol_2;
pgproduction*	P_grammar_1;
pgproduction*	P_grammar_2;
pgproduction*	P_rhs_1;
pgproduction*	P_rhs_2;
pgproduction*	P_definitions_1;
pgproduction*	P_definitions_2;
pgproduction*	P_definition_1;
pgproduction*	P_definition_2;
pgasttype*	A_termdef;
pgasttype*	A_nontermdef;
pgasttype*	A_symbol;
pgasttype*	A_rhs;

/* Terminals */
T_COLON = pg_terminal_create( g, "COLON", ":" );
T_REGEX = pg_terminal_create( g, "REGEX", "/([^\\]|\\/)*/" );
T_semicolon = pg_terminal_create( g, ";", (char*)NULL );
T_pipe = pg_terminal_create( g, "|", (char*)NULL );
T_STRING = pg_terminal_create( g, "STRING", "\".*\"" );
T_IDENT = pg_terminal_create( g, "IDENT", "[A-Za-z_][A-Za-z0-9_]*" );
T_at = pg_terminal_create( g, "@", (char*)NULL );

/* Nonterminals */
N_termdef = pg_nonterminal_create( g, "termdef" );
N_productions = pg_nonterminal_create( g, "productions" );
N_nontermdef = pg_nonterminal_create( g, "nontermdef" );
N_production = pg_nonterminal_create( g, "production" );
N_symbol = pg_nonterminal_create( g, "symbol" );
N_grammar = pg_nonterminal_create( g, "grammar" );
N_rhs = pg_nonterminal_create( g, "rhs" );
N_definitions = pg_nonterminal_create( g, "definitions" );
N_definition = pg_nonterminal_create( g, "definition" );

/* AST-Types */
A_termdef = pg_asttype_create( g, "termdef" );
A_nontermdef = pg_asttype_create( g, "nontermdef" );
A_symbol = pg_asttype_create( g, "symbol" );
A_rhs = pg_asttype_create( g, "rhs" );

/* Productions for termdef */
P_termdef_1 = pg_production_create( N_termdef, T_at, T_IDENT, T_REGEX, (pgsymbol*)NULL );
P_termdef_2 = pg_production_create( N_termdef, T_at, T_IDENT, T_STRING, (pgsymbol*)NULL );
P_termdef_3 = pg_production_create( N_termdef, T_at, T_IDENT, (pgsymbol*)NULL );

pg_production_set_asttype( P_termdef_1, A_termdef );
pg_production_set_asttype( P_termdef_2, A_termdef );
pg_production_set_asttype( P_termdef_3, A_termdef );

/* Productions for productions */
P_productions_1 = pg_production_create( N_productions, N_productions, T_pipe, N_rhs, (pgsymbol*)NULL );
P_productions_2 = pg_production_create( N_productions, N_rhs, (pgsymbol*)NULL );


/* Productions for nontermdef */
P_nontermdef_1 = pg_production_create( N_nontermdef, T_IDENT, T_COLON, N_productions, (pgsymbol*)NULL );
P_nontermdef_2 = pg_production_create( N_nontermdef, T_IDENT, (pgsymbol*)NULL );

pg_production_set_asttype( P_nontermdef_1, A_nontermdef );
pg_production_set_asttype( P_nontermdef_2, A_nontermdef );

/* Productions for production */
P_production_1 = pg_production_create( N_production, N_production, N_symbol, (pgsymbol*)NULL );
P_production_2 = pg_production_create( N_production, N_symbol, (pgsymbol*)NULL );


/* Productions for symbol */
P_symbol_1 = pg_production_create( N_symbol, T_at, T_IDENT, (pgsymbol*)NULL );
P_symbol_2 = pg_production_create( N_symbol, T_IDENT, (pgsymbol*)NULL );

pg_production_set_asttype( P_symbol_1, A_symbol );
pg_production_set_asttype( P_symbol_2, A_symbol );

/* Productions for grammar */
P_grammar_1 = pg_production_create( N_grammar, N_definitions, (pgsymbol*)NULL );
P_grammar_2 = pg_production_create( N_grammar, (pgsymbol*)NULL );


/* Productions for rhs */
P_rhs_1 = pg_production_create( N_rhs, N_production, (pgsymbol*)NULL );
P_rhs_2 = pg_production_create( N_rhs, (pgsymbol*)NULL );

pg_production_set_asttype( P_rhs_1, A_rhs );
pg_production_set_asttype( P_rhs_2, A_rhs );

/* Productions for definitions */
P_definitions_1 = pg_production_create( N_definitions, N_definitions, N_definition, (pgsymbol*)NULL );
P_definitions_2 = pg_production_create( N_definitions, N_definition, (pgsymbol*)NULL );


/* Productions for definition */
P_definition_1 = pg_production_create( N_definition, N_termdef, T_semicolon, (pgsymbol*)NULL );
P_definition_2 = pg_production_create( N_definition, N_nontermdef, T_semicolon, (pgsymbol*)NULL );

