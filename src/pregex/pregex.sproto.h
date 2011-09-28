/*
 * This is a generated file - manual editing is not recommended!
 */

/* comp.c */

/* dfa.c */
PRIVATE pregex_dfa_st* pregex_dfa_create_state( pregex_dfa* dfa );
PRIVATE void pregex_dfa_delete_state( pregex_dfa_st* st );
PRIVATE pregex_dfa_st* pregex_dfa_get_undone_state( pregex_dfa* dfa );
PRIVATE int pregex_dfa_same_transitions( pregex_dfa* dfa, LIST* trans );
PRIVATE void pregex_dfa_default_trans( pregex_dfa* dfa );
PRIVATE int pregex_dfa_collect_ref( pregex_dfa_st* st );
PRIVATE pboolean pregex_dfa_equal_states( pregex_dfa* dfa, LIST* groups, pregex_dfa_st* first, pregex_dfa_st* second );

/* direct.c */

/* nfa.c */
PRIVATE int parse_char( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );
PRIVATE int parse_factor( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );
PRIVATE int parse_sequence( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );
PRIVATE int parse_alter( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );

/* ref.c */

/* util.c */

