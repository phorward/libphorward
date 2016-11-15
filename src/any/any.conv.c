/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Converts the current value of //val// into a pboolean value.

//val// is the pany-object to convert from.

The function returns the pboolean-value of //val//.
*/
pboolean pany_to_bool( pany* val )
{
	PROC( "pany_to_bool" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pboolean)FALSE );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( val->val.b );
		case PANYTYPE_CHAR:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.c ) );
		case PANYTYPE_INT:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.i ) );
		case PANYTYPE_LONG:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.l ) );
		case PANYTYPE_ULONG:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.ul ) );
		case PANYTYPE_FLOAT:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.f ) );
		case PANYTYPE_DOUBLE:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.d ) );
		case PANYTYPE_STR:
			RETURN( (pboolean)TRUEBOOLEAN( *pstrget( val->val.s ) ) );
		case PANYTYPE_WCS:
			RETURN( (pboolean)TRUEBOOLEAN( *pwcsget( val->val.ws ) ) );
		case PANYTYPE_PTR:
			RETURN( (pboolean)TRUEBOOLEAN( val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pboolean)FALSE );
}

/** Converts the current value of //val// into a char value.

//val// is the pany-object to convert from.

The function returns the char-value of //val//.
*/
char pany_to_char( pany* val )
{
	PROC( "pany_to_char" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (char)val->val.b );
		case PANYTYPE_CHAR:
			RETURN( val->val.c );
		case PANYTYPE_INT:
			RETURN( (char)val->val.i );
		case PANYTYPE_LONG:
			RETURN( (char)val->val.l );
		case PANYTYPE_ULONG:
			RETURN( (char)val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( (char)val->val.f );
		case PANYTYPE_DOUBLE:
			RETURN( (char)val->val.d );
		case PANYTYPE_STR:
			RETURN( (char)strtol( val->val.s, (char**)NULL, 0 ) );
		case PANYTYPE_WCS:
			RETURN( (char)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PANYTYPE_PTR:
			RETURN( (char)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char)0 );
}

/** Converts the current value of //val// into a int value.

//val// is the pany-object to convert from.

The function returns the int-value of //val//.
*/
int pany_to_int( pany* val )
{
	PROC( "pany_to_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (int)val->val.b );
		case PANYTYPE_CHAR:
			RETURN( (int)val->val.c );
		case PANYTYPE_INT:
			RETURN( val->val.i );
		case PANYTYPE_LONG:
			RETURN( (int)val->val.l );
		case PANYTYPE_ULONG:
			RETURN( (int)val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( (int)val->val.f );
		case PANYTYPE_DOUBLE:
			RETURN( (int)val->val.d );
		case PANYTYPE_STR:
			RETURN( (int)strtol( val->val.s, (char**)NULL, 0 ) );
		case PANYTYPE_WCS:
			RETURN( (int)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PANYTYPE_PTR:
			RETURN( (int)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (int)0 );
}

/** Converts the current value of //val// into a long value.

//val// is the pany-object to convert from.

The function returns the long-value of //val//.
*/
long pany_to_long( pany* val )
{
	PROC( "pany_to_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (long)val->val.b );
		case PANYTYPE_CHAR:
			RETURN( (long)val->val.c );
		case PANYTYPE_INT:
			RETURN( (long)val->val.i );
		case PANYTYPE_LONG:
			RETURN( val->val.l );
		case PANYTYPE_ULONG:
			RETURN( (long)val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( (long)val->val.f );
		case PANYTYPE_DOUBLE:
			RETURN( (long)val->val.d );
		case PANYTYPE_STR:
			RETURN( (long)strtol( val->val.s, (char**)NULL, 0 ) );
		case PANYTYPE_WCS:
			RETURN( (long)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PANYTYPE_PTR:
			RETURN( (long)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (long)0 );
}

/** Converts the current value of //val// into a unsigned long value.

//val// is the pany-object to convert from.

The function returns the unsigned long-value of //val//.
*/
unsigned long pany_to_ulong( pany* val )
{
	PROC( "pany_to_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (unsigned long)0 );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (unsigned long)val->val.b );
		case PANYTYPE_CHAR:
			RETURN( (unsigned long)val->val.c );
		case PANYTYPE_INT:
			RETURN( (unsigned long)val->val.i );
		case PANYTYPE_LONG:
			RETURN( (unsigned long)val->val.l );
		case PANYTYPE_ULONG:
			RETURN( val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( (unsigned long)val->val.f );
		case PANYTYPE_DOUBLE:
			RETURN( (unsigned long)val->val.d );
		case PANYTYPE_STR:
			RETURN( (unsigned long)val->val.s );
		case PANYTYPE_WCS:
			RETURN( (unsigned long)val->val.ws );
		case PANYTYPE_PTR:
			RETURN( (unsigned long)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (unsigned long)0 );
}

/** Converts the current value of //val// into a float value.

//val// is the pany-object to convert from.

The function returns the float-value of //val//.
*/
float pany_to_float( pany* val )
{
	PROC( "pany_to_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (float)val->val.b );
		case PANYTYPE_CHAR:
			RETURN( (float)val->val.c );
		case PANYTYPE_INT:
			RETURN( (float)val->val.i );
		case PANYTYPE_LONG:
			RETURN( (float)val->val.l );
		case PANYTYPE_ULONG:
			RETURN( (float)val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( val->val.f );
		case PANYTYPE_DOUBLE:
			RETURN( (float)val->val.d );
		case PANYTYPE_STR:
			RETURN( (float)strtod( val->val.s, (char**)NULL ) );
		case PANYTYPE_WCS:
			RETURN( (float)wcstod( val->val.ws, (wchar_t**)NULL ) );
		case PANYTYPE_PTR:
			RETURN( (float)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (float)0.0 );
}

/** Converts the current value of //val// into a double value.

//val// is the pany-object to convert from.

The function returns the double-value of //val//.
*/
double pany_to_double( pany* val )
{
	PROC( "pany_to_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (double)val->val.b );
		case PANYTYPE_CHAR:
			RETURN( (double)val->val.c );
		case PANYTYPE_INT:
			RETURN( (double)val->val.i );
		case PANYTYPE_LONG:
			RETURN( (double)val->val.l );
		case PANYTYPE_ULONG:
			RETURN( (double)val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( (double)val->val.f );
		case PANYTYPE_DOUBLE:
			RETURN( val->val.d );
		case PANYTYPE_STR:
			RETURN( (double)strtod( val->val.s, (char**)NULL ) );
		case PANYTYPE_WCS:
			RETURN( (double)wcstod( val->val.ws, (wchar_t**)NULL ) );
		case PANYTYPE_PTR:
			RETURN( (double)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (double)0.0 );
}

/** Converts the current value of //val// into a char* value.

//val// is the pany-object to convert from.

The function returns the char*-value of //val//.
*/
char* pany_to_str( pany* val )
{
	PROC( "pany_to_str" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (char*)pstrdup( BOOLEAN_STR( val->val.c ) ) );
		case PANYTYPE_CHAR:
			RETURN( (char*)pasprintf( "%d", val->val.c ) );
		case PANYTYPE_INT:
			RETURN( (char*)pasprintf( "%d", val->val.i ) );
		case PANYTYPE_LONG:
			RETURN( (char*)pasprintf( "%ld", val->val.l ) );
		case PANYTYPE_ULONG:
			RETURN( (char*)pasprintf( "%ld", val->val.ul ) );
		case PANYTYPE_FLOAT:
			RETURN( (char*)pdbl_to_str( (double)val->val.f ) );
		case PANYTYPE_DOUBLE:
			RETURN( (char*)pdbl_to_str( val->val.d ) );
		case PANYTYPE_STR:
			RETURN( val->val.s );
		case PANYTYPE_WCS:
			RETURN( (char*)pwcs_to_str( val->val.ws, FALSE ) );
		case PANYTYPE_PTR:
			RETURN( (char*)pasprintf( "%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char*)NULL );
}

/** Converts the current value of //val// into a wchar_t* value.

//val// is the pany-object to convert from.

The function returns the wchar_t*-value of //val//.
*/
wchar_t* pany_to_wcs( pany* val )
{
	PROC( "pany_to_wcs" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (wchar_t*)pwcsdup( val->val.c ? L"TRUE" : L"FALSE" ) );
		case PANYTYPE_CHAR:
			RETURN( (wchar_t*)pawcsprintf( L"%d", val->val.c ) );
		case PANYTYPE_INT:
			RETURN( (wchar_t*)pawcsprintf( L"%d", val->val.i ) );
		case PANYTYPE_LONG:
			RETURN( (wchar_t*)pawcsprintf( L"%ld", val->val.l ) );
		case PANYTYPE_ULONG:
			RETURN( (wchar_t*)pawcsprintf( L"%ld", val->val.ul ) );
		case PANYTYPE_FLOAT:
			RETURN( (wchar_t*)pdbl_to_wcs( (double)val->val.f ) );
		case PANYTYPE_DOUBLE:
			RETURN( (wchar_t*)pdbl_to_wcs( val->val.d ) );
		case PANYTYPE_STR:
			RETURN( (wchar_t*)pstr_to_wcs( val->val.s, FALSE ) );
		case PANYTYPE_WCS:
			RETURN( val->val.ws );
		case PANYTYPE_PTR:
			RETURN( (wchar_t*)pawcsprintf( L"%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (wchar_t*)NULL );
}

/** Converts the current value of //val// into a void* value.

//val// is the pany-object to convert from.

The function returns the void*-value of //val//.
*/
void* pany_to_ptr( pany* val )
{
	PROC( "pany_to_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	switch( val->type )
	{
		case PANYTYPE_BOOL:
			RETURN( (void*)NULL );
		case PANYTYPE_CHAR:
			RETURN( (void*)NULL );
		case PANYTYPE_INT:
			RETURN( (void*)NULL );
		case PANYTYPE_LONG:
			RETURN( (void*)val->val.l );
		case PANYTYPE_ULONG:
			RETURN( (void*)val->val.ul );
		case PANYTYPE_FLOAT:
			RETURN( (void*)NULL );
		case PANYTYPE_DOUBLE:
			RETURN( (void*)NULL );
		case PANYTYPE_STR:
			RETURN( (void*)val->val.s );
		case PANYTYPE_WCS:
			RETURN( (void*)val->val.ws );
		case PANYTYPE_PTR:
			RETURN( val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (void*)NULL );
}

/** Converts a pany-structure to any supported type.

//val// is the pany-object to be converted.
//type// is the type define to which //val// should be converted to.

The function returns TRUE on success, FALSE else.
*/
pboolean pany_convert( pany* val, panytype type )
{
	PROC( "pany_convert" );
	PARMS( "val", "%p", val );
	PARMS( "type", "%d", type );

	if( !val )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( val->type == type )
		RETURN( TRUE );

	switch( type )
	{
		case PANYTYPE_BOOL:
			pany_set_bool( val, pany_to_bool( val ) );
			RETURN( TRUE );

		case PANYTYPE_CHAR:
			pany_set_char( val, pany_to_char( val ) );
			RETURN( TRUE );

		case PANYTYPE_INT:
			pany_set_int( val, pany_to_int( val ) );
			RETURN( TRUE );

		case PANYTYPE_LONG:
			pany_set_long( val, pany_to_long( val ) );
			RETURN( TRUE );

		case PANYTYPE_ULONG:
			pany_set_ulong( val, pany_to_ulong( val ) );
			RETURN( TRUE );

		case PANYTYPE_FLOAT:
			pany_set_float( val, pany_to_float( val ) );
			RETURN( TRUE );

		case PANYTYPE_DOUBLE:
			pany_set_double( val, pany_to_double( val ) );
			RETURN( TRUE );

		case PANYTYPE_STR:
			pany_set_str( val, pany_to_str( val ) );
			RETURN( TRUE );

		case PANYTYPE_WCS:
			pany_set_wcs( val, pany_to_wcs( val ) );
			RETURN( TRUE );

		case PANYTYPE_PTR:
			pany_set_ptr( val, pany_to_ptr( val ) );
			RETURN( TRUE );

	}

	RETURN( FALSE );
}

