/*
 * This is a generated file - manual editing is not recommended!
 */

/* base64.c */
pint to_base64( uchar** outstream, uchar* instream, psize size );
pint from_base64( uchar** outstream, psize* outsize, uchar* instream );

/* ccl.c */
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

/* convert.c */
uchar* pchar_to_uchar( pchar* str, pboolean freestr );
pchar* uchar_to_pchar( uchar* str, pboolean freestr );
uchar* plong_to_uchar( plong l );
pchar* plong_to_pchar( plong l );
uchar* pulong_to_uchar( pulong ul );
pchar* pulong_to_pchar( pulong ul );
uchar* pdouble_to_uchar( pdouble d );
pchar* pdouble_to_pchar( pdouble d );

/* bitset.c */
bitset bitset_create( int size );
int bitset_set( bitset set, int bit, int state );
int bitset_get( bitset set, int bit );
bitset bitset_copy( int size, bitset source );

/* dbg.c */
void _dbg_trace( FILE* f, int indent, char* file, int line, char* proc, char* type, char* format, ... );
void _dbg_time( FILE* f, int indent, char* file, int line, char* proc );

/* hashtab.c */
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

/* llist.c */
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

/* stack.c */
void stack_init( STACK* stack, psize size, psize step );
void stack_free( STACK* stack, void (*ff)( pbyte* ) );
pbyte* stack_push( STACK* stack, pbyte* item );
pbyte* stack_pop( STACK* stack );
pbyte* stack_access( STACK* stack, psize offset );
void stack_dump( uchar* file, int line, uchar* name, STACK* stack, void (*pf)( pbyte* ) );

/* string.c */
uchar* pstr_append_char( uchar* str, wchar chr );
uchar* pstr_append_str( uchar* dest, uchar* src, boolean freesrc );
uchar* pstr_append_nchar( uchar* str, uchar* append, psize num );
uchar* pstr_replace( uchar* str, uchar* find, uchar* replace );
uchar* pstrzero( uchar* str );
uchar* pstrdup( uchar* str );
psize pstrlen( uchar* str );
int psprintf( uchar* res, uchar* fmt, ... );
int pvasprintf( uchar** str, uchar* fmt, va_list ap );
uchar* pasprintf( uchar* fmt, ... );
long patol( uchar* str );
double patof( uchar* str );
int patoi( uchar* str );
uchar* pstr_render( uchar* tpl, ... );
uchar* pstr_strip( uchar* str );
int pstr_tok( uchar*** tokens, uchar* str, uchar* sep, int limit );
uchar* pstrupr( uchar* str );
uchar* pstrlwr( uchar* str );
int pstrcasecmp( uchar* s1, uchar* s2 );
int pstrncasecmp( uchar* s1, uchar* s2, pint n );
int pstr_char( wchar* retc, uchar *str, pboolean escapeseq );
#ifdef UNICODE
pchar* Pstrdup( pchar* str );
int Psprintf( pchar* res, pchar* fmt, ... );
int Pvasprintf( pchar** str, pchar* fmt, va_list ap );
pchar* Pasprintf( pchar* fmt, ... );
pchar* Pstr_append_char( pchar* str, wchar chr );
pchar* Pstr_append_str( pchar* dest, pchar* src, boolean freesrc );
pchar* Pstr_append_nchar( pchar* str, pchar* append, psize num );
psize Pstrlen( pchar* str );
#endif

/* system.c */
uchar* pbasename( uchar* path );
pboolean pfileexists( uchar* filename );
pint pgetopt( uchar* opt, uchar** param, pint argc, uchar** argv, uchar* optstr, uchar* loptstr, pint idx );

/* utf8.c */
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
#if 0
int u8_vprintf(uchar *fmt, va_list ap);
int u8_printf(uchar *fmt, ...);
#endif

/* util.c */
void* memdup( void* ptr, psize size );
int hash_from_str( uchar* str, int size );
int map_file( char** cont, uchar* filename );

/* var.c */
void pvar_init( pvar* var );
void pvar_reset( pvar* var );

/* var.get.c */
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

/* var.set.c */
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

/* var.conv.c */
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

/* xml.c */
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

