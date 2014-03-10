/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Converts the current value of //val// into a char value.

//val// is the pgvalue-object to convert from.

The function returns the char-value of //val//.
*/
char pg_value_to_char( pgvalue* val )
{
	PROC( "pg_value_to_char" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( val->val.c );
		case PGVALUETYPE_INT:
			RETURN( (char)val->val.i );
		case PGVALUETYPE_LONG:
			RETURN( (char)val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( (char)val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( (char)val->val.f );
		case PGVALUETYPE_DOUBLE:
			RETURN( (char)val->val.d );
		case PGVALUETYPE_STRING:
			RETURN( (char)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUETYPE_WSTRING:
			RETURN( (char)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PGVALUETYPE_PTR:
			RETURN( (char)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char)0 );
}

/** Converts the current value of //val// into a int value.

//val// is the pgvalue-object to convert from.

The function returns the int-value of //val//.
*/
int pg_value_to_int( pgvalue* val )
{
	PROC( "pg_value_to_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (int)val->val.c );
		case PGVALUETYPE_INT:
			RETURN( val->val.i );
		case PGVALUETYPE_LONG:
			RETURN( (int)val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( (int)val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( (int)val->val.f );
		case PGVALUETYPE_DOUBLE:
			RETURN( (int)val->val.d );
		case PGVALUETYPE_STRING:
			RETURN( (int)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUETYPE_WSTRING:
			RETURN( (int)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PGVALUETYPE_PTR:
			RETURN( (int)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (int)0 );
}

/** Converts the current value of //val// into a long value.

//val// is the pgvalue-object to convert from.

The function returns the long-value of //val//.
*/
long pg_value_to_long( pgvalue* val )
{
	PROC( "pg_value_to_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (long)val->val.c );
		case PGVALUETYPE_INT:
			RETURN( (long)val->val.i );
		case PGVALUETYPE_LONG:
			RETURN( val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( (long)val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( (long)val->val.f );
		case PGVALUETYPE_DOUBLE:
			RETURN( (long)val->val.d );
		case PGVALUETYPE_STRING:
			RETURN( (long)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUETYPE_WSTRING:
			RETURN( (long)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PGVALUETYPE_PTR:
			RETURN( (long)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (long)0 );
}

/** Converts the current value of //val// into a ulong value.

//val// is the pgvalue-object to convert from.

The function returns the ulong-value of //val//.
*/
ulong pg_value_to_ulong( pgvalue* val )
{
	PROC( "pg_value_to_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (ulong)val->val.c );
		case PGVALUETYPE_INT:
			RETURN( (ulong)val->val.i );
		case PGVALUETYPE_LONG:
			RETURN( (ulong)val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( (ulong)val->val.f );
		case PGVALUETYPE_DOUBLE:
			RETURN( (ulong)val->val.d );
		case PGVALUETYPE_STRING:
			RETURN( (ulong)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUETYPE_WSTRING:
			RETURN( (ulong)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PGVALUETYPE_PTR:
			RETURN( (ulong)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (ulong)0 );
}

/** Converts the current value of //val// into a float value.

//val// is the pgvalue-object to convert from.

The function returns the float-value of //val//.
*/
float pg_value_to_float( pgvalue* val )
{
	PROC( "pg_value_to_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (float)val->val.c );
		case PGVALUETYPE_INT:
			RETURN( (float)val->val.i );
		case PGVALUETYPE_LONG:
			RETURN( (float)val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( (float)val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( val->val.f );
		case PGVALUETYPE_DOUBLE:
			RETURN( (float)val->val.d );
		case PGVALUETYPE_STRING:
			RETURN( (float)strtod( val->val.s, (char**)NULL ) );
		case PGVALUETYPE_WSTRING:
			RETURN( (float)wcstod( val->val.ws, (wchar_t**)NULL ) );
		case PGVALUETYPE_PTR:
			RETURN( (float)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (float)0.0 );
}

/** Converts the current value of //val// into a double value.

//val// is the pgvalue-object to convert from.

The function returns the double-value of //val//.
*/
double pg_value_to_double( pgvalue* val )
{
	PROC( "pg_value_to_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (double)val->val.c );
		case PGVALUETYPE_INT:
			RETURN( (double)val->val.i );
		case PGVALUETYPE_LONG:
			RETURN( (double)val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( (double)val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( (double)val->val.f );
		case PGVALUETYPE_DOUBLE:
			RETURN( val->val.d );
		case PGVALUETYPE_STRING:
			RETURN( (double)strtod( val->val.s, (char**)NULL ) );
		case PGVALUETYPE_WSTRING:
			RETURN( (double)wcstod( val->val.ws, (wchar_t**)NULL ) );
		case PGVALUETYPE_PTR:
			RETURN( (double)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (double)0.0 );
}

/** Converts the current value of //val// into a char* value.
The returned memory is allocated, and must be freed by the caller.

//val// is the pgvalue-object to convert from.

The function returns the char*-value of //val//.
This pointer must be manually freed by the caller.
*/
char* pg_value_to_string( pgvalue* val )
{
	PROC( "pg_value_to_string" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (char*)pasprintf( "%d", val->val.c ) );
		case PGVALUETYPE_INT:
			RETURN( (char*)pasprintf( "%d", val->val.i ) );
		case PGVALUETYPE_LONG:
			RETURN( (char*)pasprintf( "%ld", val->val.l ) );
		case PGVALUETYPE_ULONG:
			RETURN( (char*)pasprintf( "%ld", val->val.ul ) );
		case PGVALUETYPE_FLOAT:
			RETURN( (char*)pdbl_to_str( (double)val->val.f ) );
		case PGVALUETYPE_DOUBLE:
			RETURN( (char*)pdbl_to_str( val->val.d ) );
		case PGVALUETYPE_STRING:
			RETURN( val->val.s );
		case PGVALUETYPE_WSTRING:
			RETURN( (char*)pwcs_to_str( val->val.ws, FALSE ) );
		case PGVALUETYPE_PTR:
			RETURN( (char*)pasprintf( "%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char*)NULL );
}

/** Converts the current value of //val// into a wchar_t* value.
The returned memory is allocated, and must be freed by the caller.

//val// is the pgvalue-object to convert from.

The function returns the wchar_t*-value of //val//.
This pointer must be manually freed by the caller.
*/
wchar_t* pg_value_to_wstring( pgvalue* val )
{
	PROC( "pg_value_to_wstring" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (wchar_t*)pawcsprintf( L"%d", val->val.c ) );
		case PGVALUETYPE_INT:
			RETURN( (wchar_t*)pawcsprintf( L"%d", val->val.i ) );
		case PGVALUETYPE_LONG:
			RETURN( (wchar_t*)pawcsprintf( L"%ld", val->val.l ) );
		case PGVALUETYPE_ULONG:
			RETURN( (wchar_t*)pawcsprintf( L"%ld", val->val.ul ) );
		case PGVALUETYPE_FLOAT:
			RETURN( (wchar_t*)pdbl_to_wcs( (double)val->val.f ) );
		case PGVALUETYPE_DOUBLE:
			RETURN( (wchar_t*)pdbl_to_wcs( val->val.d ) );
		case PGVALUETYPE_STRING:
			RETURN( (wchar_t*)pstr_to_wcs( val->val.s, FALSE ) );
		case PGVALUETYPE_WSTRING:
			RETURN( val->val.ws );
		case PGVALUETYPE_PTR:
			RETURN( (wchar_t*)pawcsprintf( L"%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (wchar_t*)NULL );
}

/** Converts the current value of //val// into a void* value.

//val// is the pgvalue-object to convert from.

The function returns the void*-value of //val//.
*/
void* pg_value_to_ptr( pgvalue* val )
{
	PROC( "pg_value_to_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	switch( val->type )
	{
		case PGVALUETYPE_CHAR:
			RETURN( (void*)NULL );
		case PGVALUETYPE_INT:
			RETURN( (void*)NULL );
		case PGVALUETYPE_LONG:
			RETURN( (void*)val->val.l );
		case PGVALUETYPE_ULONG:
			RETURN( (void*)val->val.ul );
		case PGVALUETYPE_FLOAT:
			RETURN( (void*)NULL );
		case PGVALUETYPE_DOUBLE:
			RETURN( (void*)NULL );
		case PGVALUETYPE_STRING:
			RETURN( (void*)val->val.s );
		case PGVALUETYPE_WSTRING:
			RETURN( (void*)val->val.ws );
		case PGVALUETYPE_PTR:
			RETURN( val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (void*)NULL );
}

/** Converts a pgvalue-structure to any supported type.

//val// is the pgvalue-object to be converted.
//type// is the type define to which //val// should be converted to.

The function returns TRUE on success, FALSE else.
*/
pboolean pg_value_convert( pgvalue* val, pgvaluetype type )
{
	PROC( "pgvalue_convert" );
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
		case PGVALUETYPE_CHAR:
			pg_value_set_char( val, pg_value_to_char( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_INT:
			pg_value_set_int( val, pg_value_to_int( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_LONG:
			pg_value_set_long( val, pg_value_to_long( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_ULONG:
			pg_value_set_ulong( val, pg_value_to_ulong( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_FLOAT:
			pg_value_set_float( val, pg_value_to_float( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_DOUBLE:
			pg_value_set_double( val, pg_value_to_double( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_STRING:
			pg_value_set_string( val, pg_value_to_string( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_WSTRING:
			pg_value_set_wstring( val, pg_value_to_wstring( val ) );
			RETURN( TRUE );

		case PGVALUETYPE_PTR:
			pg_value_set_ptr( val, pg_value_to_ptr( val ) );
			RETURN( TRUE );

	}

	RETURN( FALSE );
}

