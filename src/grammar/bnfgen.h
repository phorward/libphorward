pgterminal*	T_REGEX;
pgterminal*	T_colon;
pgterminal*	T_semicolon;
pgterminal*	T_pipe;
pgterminal*	T_STRING;
pgterminal*	T_IDENT;
pgterminal*	T_at;
pgnonterminal*	N_nonterminal;
pgnonterminal*	N_productions;
pgnonterminal*	N_terminal;
pgnonterminal*	N_production;
pgnonterminal*	N_symbol;
pgnonterminal*	N_grammar;
pgnonterminal*	N_rhs;
pgnonterminal*	N_definitions;
pgnonterminal*	N_definition;
pgproduction*	P_nonterminal_1;
pgproduction*	P_nonterminal_2;
pgproduction*	P_productions_1;
pgproduction*	P_productions_2;
pgproduction*	P_terminal_1;
pgproduction*	P_terminal_2;
pgproduction*	P_terminal_3;
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

/* Terminals */
T_REGEX = pg_terminal_create( g, "REGEX", "/([^\\]|\\/)*/" );
T_colon = pg_terminal_create( g, "colon", ":" );
T_semicolon = pg_terminal_create( g, ";", (char*)NULL );
T_pipe = pg_terminal_create( g, "|", (char*)NULL );
T_STRING = pg_terminal_create( g, "STRING", "\".*\"" );
T_IDENT = pg_terminal_create( g, "IDENT", "[A-Za-z_][A-Za-z0-9_]*" );
T_at = pg_terminal_create( g, "@", (char*)NULL );

/* Nonterminals */
N_nonterminal = pg_nonterminal_create( g, "nonterminal" );
N_productions = pg_nonterminal_create( g, "productions" );
N_terminal = pg_nonterminal_create( g, "terminal" );
N_production = pg_nonterminal_create( g, "production" );
N_symbol = pg_nonterminal_create( g, "symbol" );
N_grammar = pg_nonterminal_create( g, "grammar" );
N_rhs = pg_nonterminal_create( g, "rhs" );
N_definitions = pg_nonterminal_create( g, "definitions" );
N_definition = pg_nonterminal_create( g, "definition" );

/* Productions for nonterminal */
P_nonterminal_1 = pg_production_create( N_nonterminal, T_IDENT, T_colon, N_productions, (pgsymbol*)NULL );
P_nonterminal_2 = pg_production_create( N_nonterminal, T_IDENT, (pgsymbol*)NULL );


/* Productions for productions */
P_productions_1 = pg_production_create( N_productions, N_productions, T_pipe, N_rhs, (pgsymbol*)NULL );
P_productions_2 = pg_production_create( N_productions, N_rhs, (pgsymbol*)NULL );


/* Productions for terminal */
P_terminal_1 = pg_production_create( N_terminal, T_at, T_IDENT, T_REGEX, (pgsymbol*)NULL );
P_terminal_2 = pg_production_create( N_terminal, T_at, T_IDENT, T_STRING, (pgsymbol*)NULL );
P_terminal_3 = pg_production_create( N_terminal, T_at, T_IDENT, (pgsymbol*)NULL );


/* Productions for production */
P_production_1 = pg_production_create( N_production, N_production, N_symbol, (pgsymbol*)NULL );
P_production_2 = pg_production_create( N_production, N_symbol, (pgsymbol*)NULL );


/* Productions for symbol */
P_symbol_1 = pg_production_create( N_symbol, T_at, T_IDENT, (pgsymbol*)NULL );
P_symbol_2 = pg_production_create( N_symbol, T_IDENT, (pgsymbol*)NULL );


/* Productions for grammar */
P_grammar_1 = pg_production_create( N_grammar, N_definitions, (pgsymbol*)NULL );
P_grammar_2 = pg_production_create( N_grammar, (pgsymbol*)NULL );


/* Productions for rhs */
P_rhs_1 = pg_production_create( N_rhs, N_production, (pgsymbol*)NULL );
P_rhs_2 = pg_production_create( N_rhs, (pgsymbol*)NULL );


/* Productions for definitions */
P_definitions_1 = pg_production_create( N_definitions, N_definitions, N_definition, (pgsymbol*)NULL );
P_definitions_2 = pg_production_create( N_definitions, N_definition, (pgsymbol*)NULL );


/* Productions for definition */
P_definition_1 = pg_production_create( N_definition, N_terminal, T_semicolon, (pgsymbol*)NULL );
P_definition_2 = pg_production_create( N_definition, N_nonterminal, T_semicolon, (pgsymbol*)NULL );

