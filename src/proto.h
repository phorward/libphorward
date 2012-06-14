/*
 * This is a generated file - manual editing is not recommended!
 */

/* base/bitset.c */
bitset bitset_create( int size );
int bitset_set( bitset set, int bit, int state );
int bitset_get( bitset set, int bit );
bitset bitset_copy( int size, bitset source );

/* base/dbg.c */
void _dbg_trace( FILE* f, int indent, char* file, int line, char* proc, char* type, char* format, ... );
void _dbg_time( FILE* f, int indent, char* file, int line, char* proc );

/* base/hashtab.c */
pint hashtab_init( HASHTAB* ht, pint size, pint flags );
pint hashtab_free( HASHTAB* hashtab, void (*freefct)(void*) );
HASHELEM* hashtab_insert( HASHTAB* hashtab, uchar* key, void* data );
HASHELEM* hashtab_get( HASHTAB* hashtab, uchar* key );
HASHELEM* hashtab_update( HASHTAB* hashtab, uchar* key, void* data );
HASHELEM* hashtab_discard( HASHTAB* hashtab, uchar* key, void (*freefct)(void*) );
pint hashtab_delete( HASHTAB* hashtab, uchar* key, void (*freefct)(void*) );
void hashtab_print( HASHTAB* hashtab, FILE* channel );
pint hashtab_count( HASHTAB* hashtab );
HASHELEM* hashtab_fetch( HASHTAB* hashtab, HASHELEM* current );
void* hashelem_access( HASHELEM* hashelem );
uchar* hashelem_key( HASHELEM* hashelem );
HASHELEM* hashelem_next( HASHELEM* hashelem );
HASHELEM* hashelem_prev( HASHELEM* hashelem );

/* base/llist.c */
LIST* list_push( LIST* list, void* ptr );
LIST* list_pop( LIST* list, void** ptr );
LIST* list_remove( LIST* list, void* ptr );
LIST* list_free( LIST* list );
void list_print( LIST* list, void (*callback)( void* ) );
LIST* list_dup( LIST* src );
int list_find( LIST* list, void* ptr );
void* list_getptr( LIST* list, int cnt );
int list_diff( LIST* first, LIST* second );
LIST* list_union( LIST* first, LIST* second );
int list_count( LIST* list );
int list_subset( LIST* list, LIST* subset );
LIST* list_sort( LIST* list, int (*sf)( void*, void* ) );

/* base/memory.c */
void* pmalloc( psize size );
void* prealloc( void* oldptr, psize size );
void* pfree( void* ptr );
void* pmemdup( void* ptr, psize size );

/* base/stack.c */
void stack_init( STACK* stack, psize size, psize step );
void stack_free( STACK* stack, void (*ff)( pbyte* ) );
pbyte* stack_push( STACK* stack, pbyte* item );
pbyte* stack_pop( STACK* stack );
pbyte* stack_access( STACK* stack, psize offset );
void stack_dump( uchar* file, int line, uchar* name, STACK* stack, void (*pf)( pbyte* ) );

/* ccl/ccl.c */
pchar ccl_max( void );
int ccl_size( CCL ccl );
int ccl_count( CCL ccl );
CCL ccl_dup( CCL ccl );
CCL ccl_negate( CCL ccl );
CCL ccl_union( CCL first, CCL second );
CCL ccl_create( uchar* ccldef );
void ccl_print( FILE* stream, CCL ccl, int break_after );
uchar* ccl_to_str( CCL ccl, pboolean escape );
CCL ccl_addrange( CCL ccl, pchar begin, pchar end );
CCL ccl_delrange( CCL ccl, pchar begin, pchar end );
CCL ccl_add( CCL ccl, pchar ch );
#if 0
CCL ccl_del( CCL ccl, pchar ch );
#endif
pboolean ccl_test( CCL ccl, pchar ch );
pboolean ccl_testrange( CCL ccl, pchar begin, pchar end );
pboolean ccl_instest( CCL ccl, pchar ch );
int ccl_compare( CCL first, CCL second );
CCL ccl_intersect( CCL first, CCL second );
CCL ccl_diff( CCL first, CCL second );

/* regex/dfa.c */
void pregex_dfa_print( FILE* stream, pregex_dfa* dfa );
void pregex_dfa_free( pregex_dfa* dfa );
int pregex_dfa_from_nfa( pregex_dfa* dfa, pregex_nfa* nfa );
int pregex_dfa_minimize( pregex_dfa* dfa );
int pregex_dfa_match( pregex_dfa* dfa, uchar* str, size_t* len, int* anchors, pregex_range** ref, int* ref_count, int flags );

/* regex/direct.c */
int pregex_qmatch( uchar* regex, uchar* str, int flags, pregex_range** results );
int pregex_qsplit( uchar* regex, uchar* str, int flags, pregex_range** results );
uchar* pregex_qreplace( uchar* regex, uchar* str, uchar* replace, int flags );

/* regex/misc.c */
pregex_accept* pregex_accept_init( pregex_accept* accept );
pboolean pregex_check_anchors( uchar* all, uchar* str, psize len, int anchors, int flags );

/* regex/nfa.c */
pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, uchar* chardef, int flags );
void pregex_nfa_print( pregex_nfa* nfa );
void pregex_nfa_free( pregex_nfa* nfa );
LIST* pregex_nfa_move( pregex_nfa* nfa, LIST* input, pchar from, pchar to );
LIST* pregex_nfa_epsilon_closure( pregex_nfa* nfa, LIST* input, pregex_accept* accept );
int pregex_nfa_match( pregex_nfa* nfa, uchar* str, psize* len, int* anchors, pregex_range** ref, int* ref_count, int flags );
int pregex_nfa_from_string( pregex_nfa* nfa, uchar* str, int flags, int acc );

/* regex/pattern.c */
pregex_ptn* pregex_ptn_create_char( CCL ccl );
pregex_ptn* pregex_ptn_create_string( uchar* str, int flags );
pregex_ptn* pregex_ptn_create_sub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_alt( pregex_ptn* left, ... );
pregex_ptn* pregex_ptn_create_kle( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_pos( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_opt( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_seq( pregex_ptn* first, ... );
pregex_ptn* pregex_ptn_free( pregex_ptn* ptn );
void pregex_ptn_print( pregex_ptn* ptn, int rec );
int pregex_ptn_to_regex( uchar** regex, pregex_ptn* ptn );
int pregex_ptn_to_nfa( pregex_nfa* nfa, pregex_ptn* pattern, pregex_accept* accept );
int pregex_ptn_parse( pregex_ptn** ptn, pregex_accept* accept, uchar* str, int flags );

/* regex/pregex.c */
pregex* pregex_create( void );
pregex* pregex_free( pregex* regex );
pregex* pregex_reset( pregex* regex );
int pregex_compile( pregex* regex, uchar* pattern, int accept );
int pregex_finalize( pregex* regex );
pregex_range* pregex_match_next( pregex* regex, uchar* str );
int pregex_match( pregex* regex, uchar* str, pregex_range** results );
pregex_range* pregex_split_next( pregex* regex, uchar* str );
int pregex_split( pregex* regex, uchar* str, pregex_range** results );
uchar* pregex_replace( pregex* regex, uchar* str, uchar* replacement );
pregex_range* pregex_get_range( pregex* regex );
pregex_range* pregex_get_split( pregex* regex );
pregex_range* pregex_get_ref( pregex* regex, int offset );
int pregex_get_match_count( pregex* regex );
int pregex_get_flags( pregex* regex );
BOOLEAN pregex_set_flags( pregex* regex, int flags );
pregex_fn pregex_get_match_fn( pregex* regex );
BOOLEAN pregex_set_match_fn( pregex* regex, pregex_fn match_fn );

/* regex/range.c */
uchar* pregex_range_to_string( pregex_range* range );

/* regex/ref.c */
int pregex_ref_init( pregex_range** ref, int* ref_count, int ref_all, int flags );
void pregex_ref_update( pregex_range* ref, uchar* strp, psize off );

/* string/convert.c */
uchar* pchar_to_uchar( pchar* str, pboolean freestr );
pchar* uchar_to_pchar( uchar* str, pboolean freestr );
uchar* plong_to_uchar( plong l );
pchar* plong_to_pchar( plong l );
uchar* pulong_to_uchar( pulong ul );
pchar* pulong_to_pchar( pulong ul );
uchar* pdouble_to_uchar( pdouble d );
pchar* pdouble_to_pchar( pdouble d );

/* string/string.c */
uchar* pstrcatchar( uchar* str, wchar chr );
uchar* pstrcatstr( uchar* dest, uchar* src, boolean freesrc );
uchar* pstrncatstr( uchar* str, uchar* append, psize n );
uchar* pstrreplace( uchar* str, uchar* find, uchar* replace );
uchar* pstrdup( uchar* str );
uchar* pstrndup( uchar* str, psize len );
psize pstrlen( uchar* str );
int psprintf( uchar* res, uchar* fmt, ... );
int pvasprintf( uchar** str, uchar* fmt, va_list ap );
uchar* pasprintf( uchar* fmt, ... );
uchar* psetstr( uchar** str, uchar* val );
char* pgetstr( char* str );
long patol( uchar* str );
double patof( uchar* str );
int patoi( uchar* str );
uchar* pstrrender( uchar* tpl, ... );
char* pstrltrim( char* s );
char* pstrrtrim( char* s );
char* pstrtrim( char* s );
int pstrsplit( uchar*** tokens, uchar* str, uchar* sep, int limit );
uchar* pstrupr( uchar* str );
uchar* pstrlwr( uchar* str );
int pstrcasecmp( uchar* s1, uchar* s2 );
int pstrncasecmp( uchar* s1, uchar* s2, pint n );
int pstrparsechar( wchar* retc, uchar *str, pboolean escapeseq );
#ifdef UNICODE
pchar* Pstrdup( pchar* str );
int Psprintf( pchar* res, pchar* fmt, ... );
int Pvasprintf( pchar** str, pchar* fmt, va_list ap );
pchar* Pasprintf( pchar* fmt, ... );
pchar* Pstrcatchar( pchar* str, wchar chr );
pchar* Pstrcatstr( pchar* dest, pchar* src, boolean freesrc );
pchar* Pstrncatstr( pchar* str, pchar* append, psize num );
psize Pstrlen( pchar* str );
#endif

/* string/utf8.c */
int u8_seqlen(uchar *s);
#ifdef UTF8
wchar u8_char( uchar* str );
#else
wchar u8_char( uchar* str );
#endif
uchar* u8_move( uchar* str, int count );
wchar u8_parse_char( uchar** ch );
int u8_toucs(wchar *dest, int sz, uchar *src, int srcsz);
int u8_toutf8(uchar *dest, int sz, wchar *src, int srcsz);
int u8_wc_toutf8(uchar *dest, wchar ch);
int u8_offset(uchar *str, int charnum);
int u8_charnum(uchar *s, int offset);
int u8_strlen(uchar *s);
wchar u8_nextchar(uchar *s, int *i);
void u8_inc(uchar *s, int *i);
void u8_dec(uchar *s, int *i);
int octal_digit(uchar c);
int hex_digit(uchar c);
int u8_read_escape_sequence(uchar *str, wchar *dest);
int u8_unescape(uchar *buf, int sz, uchar *src);
int u8_escape_wchar(uchar *buf, int sz, wchar ch);
int u8_escape(uchar *buf, int sz, uchar *src, int escape_quotes);
uchar *u8_strchr(uchar *s, wchar ch, int *charn);
uchar *u8_memchr(uchar *s, wchar ch, size_t sz, int *charn);
int u8_is_locale_utf8(uchar *locale);

/* util/base64.c */
pint to_base64( uchar** outstream, uchar* instream, psize size );
pint from_base64( uchar** outstream, psize* outsize, uchar* instream );

/* util/system.c */
uchar* pwhich( uchar* filename, uchar* directories );
uchar* pbasename( uchar* path );
pboolean pfileexists( uchar* filename );
int map_file( char** cont, uchar* filename );
pint pgetopt( uchar* opt, uchar** param, pint* next, pint argc, uchar** argv, uchar* optstr, uchar* loptstr, pint idx );

/* variant/var.c */
void pvar_init( pvar* var );
void pvar_reset( pvar* var );

/* variant/var.conv.c */
pbyte pvar_to_byte( pvar* var );
uchar pvar_to_char( pvar* var );
pint pvar_to_int( pvar* var );
plong pvar_to_long( pvar* var );
pulong pvar_to_ulong( pvar* var );
pfloat pvar_to_float( pvar* var );
pdouble pvar_to_double( pvar* var );
uchar* pvar_to_string( pvar* var );
pchar* pvar_to_wstring( pvar* var );
pint pvar_convert( pvar* var, pbyte type );

/* variant/var.get.c */
pbyte pvar_get_byte( pvar* var );
uchar pvar_get_char( pvar* var );
pint pvar_get_int( pvar* var );
plong pvar_get_long( pvar* var );
pulong pvar_get_ulong( pvar* var );
pfloat pvar_get_float( pvar* var );
pdouble pvar_get_double( pvar* var );
uchar* pvar_get_cstring( pvar* var );
uchar* pvar_get_string( pvar* var );
pchar* pvar_get_wcstring( pvar* var );
pchar* pvar_get_wstring( pvar* var );

/* variant/var.set.c */
pbyte pvar_set_byte( pvar* var, pbyte b );
uchar pvar_set_char( pvar* var, uchar c );
pint pvar_set_int( pvar* var, pint i );
plong pvar_set_long( pvar* var, plong l );
pulong pvar_set_ulong( pvar* var, pulong ul );
pfloat pvar_set_float( pvar* var, pfloat f );
pdouble pvar_set_double( pvar* var, pdouble d );
uchar* pvar_set_cstring( pvar* var, uchar* s );
uchar* pvar_set_string( pvar* var, uchar* s );
pchar* pvar_set_wcstring( pvar* var, pchar* ws );
pchar* pvar_set_wstring( pvar* var, pchar* ws );

/* xml/xml.c */
XML_T xml_child( XML_T xml, uchar* name );
XML_T xml_idx( XML_T xml, int idx );
uchar* xml_attr( XML_T xml, uchar* attr );
plong xml_int_attr( XML_T xml, uchar* attr );
pdouble xml_float_attr( XML_T xml, uchar* attr );
XML_T xml_vget( XML_T xml, va_list ap );
XML_T xml_get( XML_T xml, ... );
uchar ** xml_pi( XML_T xml, uchar* target );
uchar* xml_decode( uchar* s, uchar ** ent, uchar t );
uchar* xml_str2utf8( uchar ** s, size_t* len );
void xml_free_attr( uchar ** attr );
XML_T xml_parse_str( uchar* s, size_t len );
XML_T xml_parse_fp( FILE* fp );
XML_T xml_parse_file( uchar* file );
uchar* xml_ampencode( uchar* s, size_t len, uchar ** dst, size_t* dlen, size_t* max, short a );
uchar* xml_toxml( XML_T xml );
void xml_free( XML_T xml );
uchar* xml_error( XML_T xml );
XML_T xml_new( uchar* name );
XML_T xml_insert( XML_T xml, XML_T dest, size_t off );
XML_T xml_add_child( XML_T xml, uchar* name, size_t off );
XML_T xml_set_txt( XML_T xml, uchar* txt );
XML_T xml_set_attr( XML_T xml, uchar* name, uchar* value );
XML_T xml_set_int_attr( XML_T xml, uchar* name, plong value );
XML_T xml_set_float_attr( XML_T xml, uchar* name, pdouble value );
XML_T xml_set_flag( XML_T xml, short flag );
int xml_count( XML_T xml );
int xml_count_all( XML_T xml );
XML_T xml_cut( XML_T xml );

