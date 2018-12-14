/* any/any.c */
pboolean pany_init( pany* val );
pany* pany_create( char* str );
pboolean pany_reset( pany* val );
pany* pany_free( pany* val );
pboolean pany_parse( pany* val, char* str, panytype enforce );
pboolean pany_copy( pany* dest, pany* src );
pany* pany_dup( pany* src );

/* any/any.conv.c */
pboolean pany_to_bool( pany* val );
char pany_to_char( pany* val );
int pany_to_int( pany* val );
long pany_to_long( pany* val );
unsigned long pany_to_ulong( pany* val );
float pany_to_float( pany* val );
double pany_to_double( pany* val );
char* pany_to_str( pany* val );
wchar_t* pany_to_wcs( pany* val );
void* pany_to_ptr( pany* val );
pboolean pany_convert( pany* val, panytype type );

/* any/any.print.c */
void _dbg_any_dump( char* file, int line, char* function, char* name, pany* val );
void pany_fprint( FILE* stream, pany* val );

/* any/any.get.c */
pboolean pany_get_bool( pany* val );
char pany_get_char( pany* val );
int pany_get_int( pany* val );
long pany_get_long( pany* val );
unsigned long pany_get_ulong( pany* val );
float pany_get_float( pany* val );
double pany_get_double( pany* val );
char* pany_get_cstr( pany* val );
char* pany_get_str( pany* val );
wchar_t* pany_get_cwcs( pany* val );
wchar_t* pany_get_wcs( pany* val );
void* pany_get_ptr( pany* val );

/* any/any.set.c */
pboolean pany_set_bool( pany* val, pboolean b );
char pany_set_char( pany* val, char c );
int pany_set_int( pany* val, int i );
long pany_set_long( pany* val, long l );
unsigned long pany_set_ulong( pany* val, unsigned long ul );
float pany_set_float( pany* val, float f );
double pany_set_double( pany* val, double d );
char* pany_set_cstr( pany* val, char* s );
char* pany_set_str( pany* val, char* s );
wchar_t* pany_set_cwcs( pany* val, wchar_t* ws );
wchar_t* pany_set_wcs( pany* val, wchar_t* ws );
void* pany_set_ptr( pany* val, void* ptr );

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
void parray_iter( parray* array, parrayfn callback );
void parray_riter( parray* array, parrayfn callback );
void* parray_first( parray* array );
void* parray_last( parray* array );
void* parray_next( parray* array, void* ptr );
void* parray_prev( parray* array, void* ptr );
void* parray_swap( parray* array, size_t pos1, size_t pos2 );
size_t parray_count( parray* array );
pboolean parray_partof( parray* array, void* ptr );
size_t parray_offset( parray* array, void* ptr );

/* base/ccl.c */
pccl* pccl_create( int min, int max, char* ccldef );
pboolean pccl_compat( pccl* l, pccl* r );
int pccl_size( pccl* ccl );
int pccl_count( pccl* ccl );
pccl* pccl_dup( pccl* ccl );
pboolean pccl_testrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean pccl_test( pccl* ccl, wchar_t ch );
pboolean pccl_instest( pccl* ccl, wchar_t ch );
pboolean pccl_addrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean pccl_add( pccl* ccl, wchar_t ch );
pboolean pccl_delrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean pccl_del( pccl* ccl, wchar_t ch );
pccl* pccl_negate( pccl* ccl );
pccl* pccl_union( pccl* ccl, pccl* add );
pccl* pccl_diff( pccl* ccl, pccl* rem );
int pccl_compare( pccl* left, pccl* right );
pccl* pccl_intersect( pccl* ccl, pccl* within );
pboolean pccl_get( wchar_t* from, wchar_t* to, pccl* ccl, int offset );
size_t pccl_parsechar( wchar_t* retc, char *str, pboolean escapeseq );
pboolean pccl_parseshorthand( pccl* ccl, char** str );
pboolean pccl_parse( pccl* ccl, char* ccldef, pboolean extend );
pboolean pccl_erase( pccl* ccl );
pccl* pccl_free( pccl* ccl );
char* pccl_to_str( pccl* ccl, pboolean escape );
void pccl_print( FILE* stream, pccl* ccl, int break_after );

/* base/dbg.c */
pboolean _dbg_trace_enabled( char* file, char* function );
void _dbg_trace( char* file, int line, char* type, char* function, char* format, ... );

/* base/list.c */
pboolean plist_init( plist* list, size_t size, size_t table_size, int flags );
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
int plist_concat( plist* dest, plist* src );
void plist_iter( plist* list, plistelfn callback );
void plist_riter( plist* list, plistelfn callback );
void plist_iter_access( plist* list, plistfn callback );
void plist_riter_access( plist* list, plistfn callback );
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
void plist_dbgstats( FILE* stream, plist* list );

/* base/memory.c */
void* pmalloc( size_t size );
void* prealloc( void* oldptr, size_t size );
void* pfree( void* ptr );
void* pmemdup( void* ptr, size_t size );

/* base/system.c */
char* pwhich( char* filename, char* directories );
char* pbasename( char* path );
pboolean pfileexists( char* filename );
pboolean pfiletostr( char** cont, char* filename );
int pgetopt( char* opt, char** param, int* next, int argc, char** argv, char* optstr, char* loptstr, int idx );
size_t pgetline( char** lineptr, size_t* n, FILE* stream );

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
pregex_ptn* plex_define( plex* lex, char* pat, int match_id, int flags );
int plex_lex( plex* lex, char* start, char** end );
char* plex_next( plex* lex, char* start, unsigned int* id, char** end );
size_t plex_tokenize( plex* lex, char* start, parray** matches );
void plex_dump_dot( FILE* stream, plex* lex );
plexctx* plexctx_init( plexctx* ctx, plex* lex );
plexctx* plexctx_create( plex* lex );
plexctx* plexctx_reset( plexctx* ctx );
plexctx* plexctx_free( plexctx* ctx );
pboolean plexctx_lex( plexctx* ctx, wchar_t ch );

/* regex/misc.c */
pboolean pregex_check_anchors( char* all, char* str, size_t len, int anchors, int flags );

/* regex/nfa.c */
pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, char* chardef, int flags );
void pregex_nfa_print( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_create( void );
pboolean pregex_nfa_reset( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_free( pregex_nfa* nfa );
int pregex_nfa_move( pregex_nfa* nfa, plist* hits, wchar_t from, wchar_t to );
int pregex_nfa_epsilon_closure( pregex_nfa* nfa, plist* closure, unsigned int* accept, int* flags );
int pregex_nfa_match( pregex_nfa* nfa, char* str, size_t* len, int* mflags, prange** ref, int* ref_count, int flags );
pboolean pregex_nfa_from_string( pregex_nfa* nfa, char* str, int flags, int acc );

/* regex/ptn.c */
pregex_ptn* pregex_ptn_create( char* pat, int flags );
pregex_ptn* pregex_ptn_create_char( pccl* ccl );
pregex_ptn* pregex_ptn_create_string( char* str, int flags );
pregex_ptn* pregex_ptn_create_sub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_refsub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_alt( pregex_ptn* left, ... );
pregex_ptn* pregex_ptn_create_kle( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_pos( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_opt( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_seq( pregex_ptn* first, ... );
pregex_ptn* pregex_ptn_dup( pregex_ptn* ptn );
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
int pstrncasecmp( char* s1, char* s2, size_t n );
char* pstrunescape( char* str );
int pvasprintf( char** str, char* fmt, va_list ap );
char* pasprintf( char* fmt, ... );
#ifdef UNICODE
wchar_t* pwcsdup( wchar_t* str );
wchar_t* pwcscatchar( wchar_t* str, wchar_t chr );
wchar_t* pwcscatstr( wchar_t* dest, wchar_t* src, pboolean freesrc );
wchar_t* pwcsncatstr( wchar_t* str, wchar_t* append, size_t n );
size_t pwcslen( wchar_t* str );
wchar_t* pwcsput( wchar_t** str, wchar_t* val );
wchar_t* pwcsget( wchar_t* str );
wchar_t* pwcsndup( wchar_t* str, size_t len );
int pvawcsprintf( wchar_t** str, wchar_t* fmt, va_list ap );
wchar_t* pawcsprintf( wchar_t* fmt, ... );
#endif 

/* string/utf8.c */
pboolean putf8_isutf( unsigned char c );
int putf8_seqlen(char *s);
wchar_t putf8_char( char* str );
char* putf8_move( char* str, int count );
wchar_t putf8_parse_char( char** ch );
int putf8_toucs(wchar_t *dest, int sz, char *src, int srcsz);
int putf8_toutf8(char *dest, int sz, wchar_t *src, int srcsz);
int putf8_wc_toutf8(char *dest, wchar_t ch);
int putf8_offset(char *str, int charnum);
int putf8_charnum(char *s, int offset);
int putf8_strlen(char *s);
wchar_t putf8_nextchar(char *s, int *i);
void putf8_inc(char *s, int *i);
void putf8_dec(char *s, int *i);
int octal_digit(char c);
int hex_digit(char c);
int putf8_read_escape_sequence(char *str, wchar_t *dest);
int putf8_unescape(char *buf, int sz, char *src);
int putf8_escape_wchar(char *buf, int sz, wchar_t ch);
int putf8_escape(char *buf, int sz, char *src, int escape_quotes);
char *putf8_strchr(char *s, wchar_t ch, int *charn);
char *putf8_memchr(char *s, wchar_t ch, size_t sz, int *charn);
int putf8_is_locale_utf8(char *locale);

