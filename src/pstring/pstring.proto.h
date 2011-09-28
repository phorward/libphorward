/*
 * This is a generated file - manual editing is not recommended!
 */

/* get.c */
pchar* pstring_get( pstring obj );
pint pstring_get_int( pstring obj );
plong pstring_get_long( pstring obj );
pulong pstring_get_ulong( pstring obj );
pfloat pstring_get_float( pstring obj );
pdouble pstring_get_double( pstring obj );

/* set.c */
int pstring_set( pstring obj, pchar* str );
pint pstring_set_int( pstring obj, pint i );
pint pstring_set_long( pstring obj, plong l );
pint pstring_set_ulong( pstring obj, pulong ul );
pint pstring_set_float( pstring obj, pfloat f );
pint pstring_set_double( pstring obj, pdouble d );

/* pstring.c */
pstring pstring_new( pchar* str );
pstring pstring_free( pstring obj );
void pstring_init( pstring obj );
void pstring_reset( pstring obj );
psize pstring_len( pstring obj );
pchar* pstring_GET( pstring obj );
int pstring_PUT( pstring obj, pchar* ptr, psize len );
pboolean pstring_empty( pstring obj );
int pstring_copy( pstring dst, pstring src );
pstring pstring_dup( pstring obj );
pstring pstring_APPEND( pstring obj, pchar* ptr, psize len );
pstring pstring_append( pstring dst, pstring src, pboolean drop_src );
pchar* pstring_charat( pstring obj, psize position );

/* util.c */
int pstring_compare( pstring obj1, pstring obj2, pboolean case_ins );
int pstring_from_uchar( pstring obj, uchar* str );
uchar* pstring_to_uchar( pstring obj );

