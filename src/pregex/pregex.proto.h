/*
 * This is a generated file - manual editing is not recommended!
 */

/* comp.c */
void pregex_comp_init( pregex* machine, int flags );
int pregex_comp_compile( pregex* machine, uchar* pattern, int accept );
int pregex_comp_finalize( pregex* machine );
void pregex_comp_free( pregex* machine );
int pregex_comp_match( pregex* machine, uchar* str, pregex_callback fn, pregex_result** results );
int pregex_comp_split( pregex* machine, uchar* str, pregex_callback fn, pregex_result** results );
int pregex_comp_replace( pregex* machine, uchar* str, uchar* replacement, pregex_callback fn, uchar** result );

/* dfa.c */
void pregex_dfa_print( FILE* stream, pregex_dfa* dfa );
void pregex_dfa_free( pregex_dfa* dfa );
int pregex_dfa_from_nfa( pregex_dfa* dfa, pregex_nfa* nfa );
int pregex_dfa_minimize( pregex_dfa* dfa );
int pregex_dfa_match( pregex_dfa* dfa, uchar* str, size_t* len, int* anchors, pregex_result** ref, int* ref_count, int flags );

/* direct.c */
int pregex_match( uchar* regex, uchar* str, int flags, pregex_result** results );
int pregex_split( uchar* regex, uchar* str, int flags, pregex_result** results );
int pregex_replace( uchar* regex, uchar* str, uchar* replacement, int flags, uchar** result );

/* nfa.c */
pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, uchar* chardef, int flags );
void pregex_nfa_print( pregex_nfa* nfa );
void pregex_nfa_free( pregex_nfa* nfa );
LIST* pregex_nfa_move( pregex_nfa* nfa, LIST* input, pchar from, pchar to );
LIST* pregex_nfa_epsilon_closure( pregex_nfa* nfa, LIST* input, int* accept, BOOLEAN* greedy, int* anchors );
int pregex_nfa_match( pregex_nfa* nfa, uchar* str, psize* len, int* anchors, pregex_result** ref, int* ref_count, int flags );
int pregex_compile_to_nfa( uchar* str, pregex_nfa* nfa, int flags, int accept );

/* ref.c */
int pregex_ref_init( pregex_result** ref, int* ref_count, int ref_all, int flags );
void pregex_ref_update( pregex_result* ref, uchar* strp, psize off );

/* util.c */
int pregex_nfa_from_string( pregex_nfa* nfa, uchar* str, int flags, int acc );
pboolean pregex_check_anchors( uchar* all, uchar* str, psize len, int anchors, int flags );

