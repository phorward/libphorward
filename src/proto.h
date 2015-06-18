/* base/array.c */
pboolean parray_init( parray* array, size_t size, size_t chunk );
parray* parray_create( size_t size, size_t chunk );
pboolean parray_erase( parray* array );
parray* parray_free( parray* array );
void* parray_push( parray* array, void* item );
pboolean parray_reserve( parray* array, size_t n );
void* parray_malloc( parray* array );
void* parray_rmalloc( parray* array );
void* parray_insert( parray* array, size_t offset, void* item );
void* parray_remove( parray* array, size_t offset, void** item );
void* parray_pop( parray* array );
void* parray_unshift( parray* array, void* item );
void* parray_shift( parray* array );
void* parray_get( parray* array, size_t offset );
void* parray_put( parray* array, size_t offset, void* item );
void* parray_rget( parray* array, size_t offset );
void* parray_rput( parray* array, size_t offset, void* item );
void* parray_last( parray* array );
void* parray_first( parray* array );
void* parray_swap( parray* array, size_t pos1, size_t pos2 );
size_t parray_count( parray* array );
pboolean parray_partof( parray* array, void* ptr );
size_t parray_offset( parray* array, void* ptr );

/* base/ccl.c */
pccl* p_ccl_create( int min, int max, char* ccldef );
pboolean p_ccl_compat( pccl* l, pccl* r );
int p_ccl_size( pccl* ccl );
int p_ccl_count( pccl* ccl );
pccl* p_ccl_dup( pccl* ccl );
pboolean p_ccl_testrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_test( pccl* ccl, wchar_t ch );
pboolean p_ccl_instest( pccl* ccl, wchar_t ch );
pboolean p_ccl_addrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_add( pccl* ccl, wchar_t ch );
pboolean p_ccl_delrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_del( pccl* ccl, wchar_t ch );
pccl* p_ccl_negate( pccl* ccl );
pccl* p_ccl_union( pccl* ccl, pccl* add );
pccl* p_ccl_diff( pccl* ccl, pccl* rem );
int p_ccl_compare( pccl* left, pccl* right );
pccl* p_ccl_intersect( pccl* ccl, pccl* within );
pboolean p_ccl_get( wchar_t* from, wchar_t* to, pccl* ccl, int offset );
pboolean p_ccl_parse( pccl* ccl, char* ccldef, pboolean extend );
pboolean p_ccl_erase( pccl* ccl );
pccl* p_ccl_free( pccl* ccl );
char* p_ccl_to_str( pccl* ccl, pboolean escape );
void p_ccl_print( FILE* stream, pccl* ccl, int break_after );

/* base/dbg.c */
void _dbg_trace( char* file, int line, char* type, char* format, ... );

/* base/list.c */
pboolean plist_init( plist* list, size_t size, int flags );
plist* plist_create( size_t size, int flags );
plist* plist_dup( plist* list );
pboolean plist_erase( plist* list );
pboolean plist_clear( plist* list );
plist* plist_free( plist* list );
plistel* plist_insert( plist* list, plistel* pos, char* key, void* src );
plistel* plist_push( plist* list, void* src );
plistel* plist_shift( plist* list, void* src );
void* plist_malloc( plist* list );
void* plist_rmalloc( plist* list );
pboolean plist_remove( plist* list, plistel* e );
pboolean plist_pop( plist* list, void* dest );
pboolean plist_unshift( plist* list, void* dest );
plistel* plist_get( plist* list, size_t n );
plistel* plist_rget( plist* list, size_t n );
plistel* plist_get_by_key( plist* list, char* key );
plistel* plist_get_by_ptr( plist* list, void* ptr );
int plist_union( plist* all, plist* from );
int plist_diff( plist* left, plist* right );
pboolean plist_subsort( plist* list, plistel* from, plistel* to );
pboolean plist_sort( plist* list );
pboolean plist_set_comparefn( plist* list, int (*comparefn)( plist*, plistel*, plistel* ) );
pboolean plist_set_sortfn( plist* list, int (*sortfn)( plist*, plistel*, plistel* ) );
pboolean plist_set_printfn( plist* list, void (*printfn)( plist* ) );
void* plist_access( plistel* e );
char* plist_key( plistel* e );
plistel* plist_next( plistel* u );
plistel* plist_prev( plistel* u );
plistel* plist_hashnext( plistel* u );
plistel* plist_hashprev( plistel* u );
int plist_offset( plistel* u );
pboolean plist_swap( plistel* a, plistel* b );
plistel* plist_first( plist* l );
plistel* plist_last( plist* l );
int plist_size( plist* l );
int plist_count( plist* l );

/* base/memory.c */
void* pmalloc( size_t size );
void* prealloc( void* oldptr, size_t size );
void* pfree( void* ptr );
void* pmemdup( void* ptr, size_t size );

/* parse/ast.c */
ppmatch* pp_ast_get( parray* ast, ppmatch* from, size_t offset );
ppmatch* pp_ast_query( parray* ast, ppmatch* start, int count, int emit, int depth );
ppmatch* pp_ast_pendant( parray* ast, ppmatch* match );
void pp_ast_print( parray* ast );
void pp_ast_simplify( parray* ast );
void pp_ast_tree2svg( parray* ast );

/* parse/bnf.c */
void pp_bnf_define( ppgram* g );

/* parse/gram.c */
pboolean pp_gram_prepare( ppgram* g );
ppgram* pp_gram_create( void );
pboolean pp_gram_from_bnf( ppgram* g, char* bnf );
void pp_gram_print( ppgram* g );
ppgram* pp_gram_free( ppgram* g );

/* parse/ll.c */
pboolean pp_ll_parse( parray** ast, ppgram* grm, char* start, char** end );

/* parse/lr.c */
plist* pp_lr_closure( ppgram* gram, pboolean optimize );
pboolean pp_lr_parse( parray** ast, ppgram* grm, char* start, char** end );

/* parse/parse.c */
pparse* pp_create( int flags, char* bnf );
pparse* pp_free( pparse* par );
pboolean pp_parse_to_ast( parray** ast, pparse* par, char* start, char** end );

/* parse/prod.c */
ppprod* pp_prod_create( ppgram* g, ppsym* lhs, ... );
ppprod* pp_prod_get( ppgram* g, int n );
pboolean pp_prod_append( ppprod* p, ppsym* sym );
ppsym* pp_prod_getfromrhs( ppprod* p, int off );
char* pp_prod_to_str( ppprod* p );

/* parse/scan.c */
pboolean pp_sym_in_input( ppsym* sym, char* start, char** end );
pboolean pp_white_in_input( ppgram* grm, char* start, char** end );
size_t pp_pos_in_input( int* row, int* col, char* start, char* end );

/* parse/sym.c */
ppsym* pp_sym_create( ppgram* g, ppsymtype type, char* name, char* def );
ppsym* pp_sym_get( ppgram* g, int n );
ppsym* pp_sym_get_by_name( ppgram* g, char* name );
ppsym* pp_sym_get_nameless_term_by_def( ppgram* g, char* name );
char* pp_sym_to_str( ppsym* sym );

/* regex/dfa.c */
void pregex_dfa_print( pregex_dfa* dfa );
pregex_dfa* pregex_dfa_create( void );
pboolean pregex_dfa_reset( pregex_dfa* dfa );
pregex_dfa* pregex_dfa_free( pregex_dfa* dfa );
int pregex_dfa_from_nfa( pregex_dfa* dfa, pregex_nfa* nfa );
int pregex_dfa_minimize( pregex_dfa* dfa );
int pregex_dfa_match( pregex_dfa* dfa, char* str, size_t* len, int* mflags, prange** ref, int* ref_count, int flags );
int pregex_dfa_to_dfatab( wchar_t*** dfatab, pregex_dfa* dfa );

/* regex/direct.c */
int pregex_qmatch( char* regex, char* str, int flags, parray** matches );
int pregex_qsplit( char* regex, char* str, int flags, parray** matches );
char* pregex_qreplace( char* regex, char* str, char* replace, int flags );

/* regex/lex.c */
plex* plex_create( int flags );
plex* plex_free( plex* lex );
pboolean plex_reset( plex* lex );
pboolean plex_prepare( plex* lex );
pboolean plex_define( plex* lex, char* pat, int match_id, int flags );
int plex_lex( plex* lex, char* start, char** end );
char* plex_next( plex* lex, char* start, int* id, char** end );
int plex_tokenize( plex* lex, char* start, parray** matches );

/* regex/misc.c */
pboolean pregex_check_anchors( char* all, char* str, size_t len, int anchors, int flags );

/* regex/nfa.c */
pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, char* chardef, int flags );
void pregex_nfa_print( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_create( void );
pboolean pregex_nfa_reset( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_free( pregex_nfa* nfa );
int pregex_nfa_move( pregex_nfa* nfa, plist* hits, wchar_t from, wchar_t to );
int pregex_nfa_epsilon_closure( pregex_nfa* nfa, plist* closure, pregex_accept* accept );
int pregex_nfa_match( pregex_nfa* nfa, char* str, size_t* len, int* mflags, prange** ref, int* ref_count, int flags );
pboolean pregex_nfa_from_string( pregex_nfa* nfa, char* str, int flags, int acc );

/* regex/pattern.c */
pregex_ptn* pregex_ptn_create_char( pccl* ccl );
pregex_ptn* pregex_ptn_create_string( char* str, int flags );
pregex_ptn* pregex_ptn_create_sub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_refsub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_alt( pregex_ptn* left, ... );
pregex_ptn* pregex_ptn_create_kle( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_pos( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_opt( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_seq( pregex_ptn* first, ... );
pregex_ptn* pregex_ptn_free( pregex_ptn* ptn );
void pregex_ptn_print( pregex_ptn* ptn, int rec );
char* pregex_ptn_to_regex( pregex_ptn* ptn );
pboolean pregex_ptn_to_nfa( pregex_nfa* nfa, pregex_ptn* ptn );
pboolean pregex_ptn_to_dfa( pregex_dfa* dfa, pregex_ptn* ptn );
int pregex_ptn_to_dfatab( wchar_t*** dfatab, pregex_ptn* ptn );
pboolean pregex_ptn_parse( pregex_ptn** ptn, char* str, int flags );

/* regex/regex.c */
pregex* pregex_create( char* pat, int flags );
pregex* pregex_free( pregex* regex );
pboolean pregex_match( pregex* regex, char* start, char** end );
char* pregex_find( pregex* regex, char* start, char** end );
int pregex_findall( pregex* regex, char* start, parray** matches );
char* pregex_split( pregex* regex, char* start, char** end, char** next );
int pregex_splitall( pregex* regex, char* start, parray** matches );
char* pregex_replace( pregex* regex, char* str, char* replacement );

/* string/convert.c */
char* pwcs_to_str( wchar_t* str, pboolean freestr );
wchar_t* pstr_to_wcs( char* str, pboolean freestr );
char* pdbl_to_str( double d );
#ifdef UNICODE
wchar_t* pdbl_to_wcs( double d );
#endif

/* string/string.c */
char* pstrcatchar( char* str, char chr );
char* pstrcatstr( char* dest, char* src, pboolean freesrc );
char* pstrncatstr( char* str, char* append, size_t n );
char* pstrreplace( char* str, char* find, char* replace );
char* pstrdup( char* str );
char* pstrndup( char* str, size_t len );
size_t pstrlen( char* str );
char* pstrput( char** str, char* val );
char* pstrget( char* str );
char* pstrrender( char* tpl, ... );
char* pstrltrim( char* s );
char* pstrrtrim( char* s );
char* pstrtrim( char* s );
int pstrsplit( char*** tokens, char* str, char* sep, int limit );
char* pstrupr( char* s );
char* pstrlwr( char* s );
int pstrcasecmp( char* s1, char* s2 );
int pstrncasecmp( char* s1, char* s2, int n );
int pstrparsechar( wchar_t* retc, char *str, pboolean escapeseq );
int pvasprintf( char** str, char* fmt, va_list ap );
char* pasprintf( char* fmt, ... );
#ifdef UNICODE
wchar_t* pwcsdup( wchar_t* str );
wchar_t* pwcscatchar( wchar_t* str, wchar_t chr );
wchar_t* pwcscatstr( wchar_t* dest, wchar_t* src, pboolean freesrc );
wchar_t* pwcsncatstr( wchar_t* str, wchar_t* append, size_t n );
size_t pwcslen( wchar_t* str );
wchar_t* pwcsndup( wchar_t* str, size_t len );
int pvawcsprintf( wchar_t** str, wchar_t* fmt, va_list ap );
wchar_t* pawcsprintf( wchar_t* fmt, ... );
#endif 

/* string/utf8.c */
pboolean u8_isutf( unsigned char c );
int u8_seqlen(char *s);
wchar_t u8_char( char* str );
char* u8_move( char* str, int count );
wchar_t u8_parse_char( char** ch );
int u8_toucs(wchar_t *dest, int sz, char *src, int srcsz);
int u8_toutf8(char *dest, int sz, wchar_t *src, int srcsz);
int u8_wc_toutf8(char *dest, wchar_t ch);
int u8_offset(char *str, int charnum);
int u8_charnum(char *s, int offset);
int u8_strlen(char *s);
wchar_t u8_nextchar(char *s, int *i);
void u8_inc(char *s, int *i);
void u8_dec(char *s, int *i);
int octal_digit(char c);
int hex_digit(char c);
int u8_read_escape_sequence(char *str, wchar_t *dest);
int u8_unescape(char *buf, int sz, char *src);
int u8_escape_wchar(char *buf, int sz, wchar_t ch);
int u8_escape(char *buf, int sz, char *src, int escape_quotes);
char *u8_strchr(char *s, wchar_t ch, int *charn);
char *u8_memchr(char *s, wchar_t ch, size_t sz, int *charn);
int u8_is_locale_utf8(char *locale);

/* util/system.c */
char* pwhich( char* filename, char* directories );
char* pbasename( char* path );
pboolean pfileexists( char* filename );
pboolean pfiletostr( char** cont, char* filename );
int pgetopt( char* opt, char** param, int* next, int argc, char** argv, char* optstr, char* loptstr, int idx );

/* util/xml.c */
XML_T xml_child( XML_T xml, char* name );
XML_T xml_idx( XML_T xml, int idx );
char* xml_attr( XML_T xml, char* attr );
long xml_int_attr( XML_T xml, char* attr );
double xml_float_attr( XML_T xml, char* attr );
XML_T xml_vget( XML_T xml, va_list ap );
XML_T xml_get( XML_T xml, ... );
char ** xml_pi( XML_T xml, char* target );
char* xml_decode( char* s, char ** ent, char t );
char* xml_str2utf8( char ** s, size_t* len );
void xml_free_attr( char ** attr );
XML_T xml_parse_str( char* s, size_t len );
XML_T xml_parse_fp( FILE* fp );
XML_T xml_parse_file( char* file );
char* xml_ampencode( char* s, size_t len, char ** dst, size_t* dlen, size_t* max, short a );
char* xml_toxml( XML_T xml );
void xml_free( XML_T xml );
char* xml_error( XML_T xml );
XML_T xml_new( char* name );
XML_T xml_insert( XML_T xml, XML_T dest, size_t off );
XML_T xml_add_child( XML_T xml, char* name, size_t off );
XML_T xml_set_txt( XML_T xml, char* txt );
XML_T xml_set_attr( XML_T xml, char* name, char* value );
XML_T xml_set_int_attr( XML_T xml, char* name, long value );
XML_T xml_set_float_attr( XML_T xml, char* name, double value );
XML_T xml_set_flag( XML_T xml, short flag );
int xml_count( XML_T xml );
int xml_count_all( XML_T xml );
XML_T xml_cut( XML_T xml );

/* value/value.c */
pboolean pvalue_init( pvalue* val );
pvalue* pvalue_create( void );
pboolean pvalue_reset( pvalue* val );
pvalue* pvalue_free( pvalue* val );
pboolean pvalue_set_constant( pvalue* val, pboolean constant );
pboolean pvalue_get_constant( pvalue* val );
pboolean pvalue_set_autoconvert( pvalue* val, pboolean autoconvert );
pboolean pvalue_get_autoconvert( pvalue* val );

/* value/value.conv.c */
char pvalue_to_char( pvalue* val );
int pvalue_to_int( pvalue* val );
long pvalue_to_long( pvalue* val );
ulong pvalue_to_ulong( pvalue* val );
float pvalue_to_float( pvalue* val );
double pvalue_to_double( pvalue* val );
char* pvalue_to_str( pvalue* val );
wchar_t* pvalue_to_wcs( pvalue* val );
void* pvalue_to_ptr( pvalue* val );
pboolean pvalue_convert( pvalue* val, pvaluetype type );

/* value/value.get.c */
char pvalue_get_char( pvalue* val );
int pvalue_get_int( pvalue* val );
long pvalue_get_long( pvalue* val );
ulong pvalue_get_ulong( pvalue* val );
float pvalue_get_float( pvalue* val );
double pvalue_get_double( pvalue* val );
char* pvalue_get_cstr( pvalue* val );
char* pvalue_get_str( pvalue* val );
wchar_t* pvalue_get_cwcs( pvalue* val );
wchar_t* pvalue_get_wcs( pvalue* val );
void* pvalue_get_ptr( pvalue* val );

/* value/value.set.c */
char pvalue_set_char( pvalue* val, char c );
int pvalue_set_int( pvalue* val, int i );
long pvalue_set_long( pvalue* val, long l );
ulong pvalue_set_ulong( pvalue* val, ulong ul );
float pvalue_set_float( pvalue* val, float f );
double pvalue_set_double( pvalue* val, double d );
char* pvalue_set_cstr( pvalue* val, char* s );
char* pvalue_set_str( pvalue* val, char* s );
wchar_t* pvalue_set_cwcs( pvalue* val, wchar_t* ws );
wchar_t* pvalue_set_wcs( pvalue* val, wchar_t* ws );
void* pvalue_set_ptr( pvalue* val, void* ptr );

