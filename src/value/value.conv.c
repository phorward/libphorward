/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Converts the current value of //val// into a char value.

//val// is the pgvalue-object to convert from.

The function returns the char-value of the variant.
*/
char pgvalue_to_char( pgvalue* val )
{
	PROC( "pgvalue_to_char" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( val->val.c );
		case PGVALUE_INT:
			RETURN( (char)val->val.i );
		case PGVALUE_LONG:
			RETURN( (char)val->val.l );
		case PGVALUE_ULONG:
			RETURN( (char)val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( (char)val->val.f );
		case PGVALUE_DOUBLE:
			RETURN( (char)val->val.d );
		case PGVALUE_STRING:
			RETURN( (char)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUE_WSTRING:
			RETURN( (char)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
		case PGVALUE_PTR:
			RETURN( (char)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char)0 );
}

/** Converts the current value of //val// into a int value.

//val// is the pgvalue-object to convert from.

The function returns the int-value of the variant.
*/
int pgvalue_to_int( pgvalue* val )
{
	PROC( "pgvalue_to_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (int)val->val.c );
		case PGVALUE_INT:
			RETURN( val->val.i );
		case PGVALUE_LONG:
			RETURN( (int)val->val.l );
		case PGVALUE_ULONG:
			RETURN( (int)val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( (int)val->val.f );
		case PGVALUE_DOUBLE:
			RETURN( (int)val->val.d );
		case PGVALUE_STRING:
			RETURN( (int)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUE_WSTRING:
			RETURN( (int)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
		case PGVALUE_PTR:
			RETURN( (int)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (int)0 );
}

/** Converts the current value of //val// into a long value.

//val// is the pgvalue-object to convert from.

The function returns the long-value of the variant.
*/
long pgvalue_to_long( pgvalue* val )
{
	PROC( "pgvalue_to_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (long)val->val.c );
		case PGVALUE_INT:
			RETURN( (long)val->val.i );
		case PGVALUE_LONG:
			RETURN( val->val.l );
		case PGVALUE_ULONG:
			RETURN( (long)val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( (long)val->val.f );
		case PGVALUE_DOUBLE:
			RETURN( (long)val->val.d );
		case PGVALUE_STRING:
			RETURN( (long)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUE_WSTRING:
			RETURN( (long)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
		case PGVALUE_PTR:
			RETURN( (long)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (long)0 );
}

/** Converts the current value of //val// into a ulong value.

//val// is the pgvalue-object to convert from.

The function returns the ulong-value of the variant.
*/
ulong pgvalue_to_ulong( pgvalue* val )
{
	PROC( "pgvalue_to_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (ulong)val->val.c );
		case PGVALUE_INT:
			RETURN( (ulong)val->val.i );
		case PGVALUE_LONG:
			RETURN( (ulong)val->val.l );
		case PGVALUE_ULONG:
			RETURN( val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( (ulong)val->val.f );
		case PGVALUE_DOUBLE:
			RETURN( (ulong)val->val.d );
		case PGVALUE_STRING:
			RETURN( (ulong)strtol( val->val.s, (char**)NULL, 0 ) );
		case PGVALUE_WSTRING:
			RETURN( (ulong)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
		case PGVALUE_PTR:
			RETURN( (ulong)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (ulong)0 );
}

/** Converts the current value of //val// into a float value.

//val// is the pgvalue-object to convert from.

The function returns the float-value of the variant.
*/
float pgvalue_to_float( pgvalue* val )
{
	PROC( "pgvalue_to_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (float)val->val.c );
		case PGVALUE_INT:
			RETURN( (float)val->val.i );
		case PGVALUE_LONG:
			RETURN( (float)val->val.l );
		case PGVALUE_ULONG:
			RETURN( (float)val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( val->val.f );
		case PGVALUE_DOUBLE:
			RETURN( (float)val->val.d );
		case PGVALUE_STRING:
			RETURN( (float)strtod( val->val.s, (char**)NULL ) );
		case PGVALUE_WSTRING:
			RETURN( (float)wcstod( val->val.ws, (pchar**)NULL ) );
		case PGVALUE_PTR:
			RETURN( (float)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (float)0.0 );
}

/** Converts the current value of //val// into a double value.

//val// is the pgvalue-object to convert from.

The function returns the double-value of the variant.
*/
double pgvalue_to_double( pgvalue* val )
{
	PROC( "pgvalue_to_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (double)val->val.c );
		case PGVALUE_INT:
			RETURN( (double)val->val.i );
		case PGVALUE_LONG:
			RETURN( (double)val->val.l );
		case PGVALUE_ULONG:
			RETURN( (double)val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( (double)val->val.f );
		case PGVALUE_DOUBLE:
			RETURN( val->val.d );
		case PGVALUE_STRING:
			RETURN( (double)strtod( val->val.s, (char**)NULL ) );
		case PGVALUE_WSTRING:
			RETURN( (double)wcstod( val->val.ws, (pchar**)NULL ) );
		case PGVALUE_PTR:
			RETURN( (double)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (double)0.0 );
}

/** Converts the current value of //val// into a char* value.
The returned memory is allocated, and must be freed by the caller.

//val// is the pgvalue-object to convert from.

The function returns the char*-value of the variant.
This pointer must be manually freed by the caller.
*/
char* pgvalue_to_string( pgvalue* val )
{
	PROC( "pgvalue_to_string" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (char*)pasprintf( "%d", val->val.c ) );
		case PGVALUE_INT:
			RETURN( (char*)pasprintf( "%d", val->val.i ) );
		case PGVALUE_LONG:
			RETURN( (char*)pasprintf( "%ld", val->val.l ) );
		case PGVALUE_ULONG:
			RETURN( (char*)pasprintf( "%ld", val->val.ul ) );
		case PGVALUE_FLOAT:
			RETURN( (char*)pdouble_to_uchar( (double)val->val.f ) );
		case PGVALUE_DOUBLE:
			RETURN( (char*)pdouble_to_uchar( val->val.d ) );
		case PGVALUE_STRING:
			RETURN( val->val.s );
		case PGVALUE_WSTRING:
			RETURN( (char*)wchar_to_u8( val->val.ws, FALSE ) );
		case PGVALUE_PTR:
			RETURN( (char*)pasprintf( "%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char*)NULL );
}

/** Converts the current value of //val// into a pchar* value.
The returned memory is allocated, and must be freed by the caller.

//val// is the pgvalue-object to convert from.

The function returns the pchar*-value of the variant.
This pointer must be manually freed by the caller.
*/
pchar* pgvalue_to_wstring( pgvalue* val )
{
	PROC( "pgvalue_to_wstring" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pchar*)NULL );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (pchar*)pawcsprintf( L"%d", val->val.c ) );
		case PGVALUE_INT:
			RETURN( (pchar*)pawcsprintf( L"%d", val->val.i ) );
		case PGVALUE_LONG:
			RETURN( (pchar*)pawcsprintf( L"%ld", val->val.l ) );
		case PGVALUE_ULONG:
			RETURN( (pchar*)pawcsprintf( L"%ld", val->val.ul ) );
		case PGVALUE_FLOAT:
			RETURN( (pchar*)pdouble_to_pchar( (double)val->val.f ) );
		case PGVALUE_DOUBLE:
			RETURN( (pchar*)pdouble_to_pchar( val->val.d ) );
		case PGVALUE_STRING:
			RETURN( (pchar*)u8_to_wchar( val->val.s, FALSE ) );
		case PGVALUE_WSTRING:
			RETURN( val->val.ws );
		case PGVALUE_PTR:
			RETURN( (pchar*)pawcsprintf( L"%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pchar*)NULL );
}

/** Converts the current value of //val// into a void* value.

//val// is the pgvalue-object to convert from.

The function returns the void*-value of the variant.
*/
void* pgvalue_to_ptr( pgvalue* val )
{
	PROC( "pgvalue_to_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	switch( val->type )
	{
		case PGVALUE_CHAR:
			RETURN( (void*)NULL );
		case PGVALUE_INT:
			RETURN( (void*)NULL );
		case PGVALUE_LONG:
			RETURN( (void*)val->val.l );
		case PGVALUE_ULONG:
			RETURN( (void*)val->val.ul );
		case PGVALUE_FLOAT:
			RETURN( (void*)NULL );
		case PGVALUE_DOUBLE:
			RETURN( (void*)NULL );
		case PGVALUE_STRING:
			RETURN( (void*)val->val.s );
		case PGVALUE_WSTRING:
			RETURN( (void*)val->val.ws );
		case PGVALUE_PTR:
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
pboolean pgvalue_convert( pgvalue* val, pgvaluetype type )
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
		case PGVALUE_CHAR:
			pgvalue_set_char( val, pgvalue_to_char( val ) );
			RETURN( TRUE );

		case PGVALUE_INT:
			pgvalue_set_int( val, pgvalue_to_int( val ) );
			RETURN( TRUE );

		case PGVALUE_LONG:
			pgvalue_set_long( val, pgvalue_to_long( val ) );
			RETURN( TRUE );

		case PGVALUE_ULONG:
			pgvalue_set_ulong( val, pgvalue_to_ulong( val ) );
			RETURN( TRUE );

		case PGVALUE_FLOAT:
			pgvalue_set_float( val, pgvalue_to_float( val ) );
			RETURN( TRUE );

		case PGVALUE_DOUBLE:
			pgvalue_set_double( val, pgvalue_to_double( val ) );
			RETURN( TRUE );

		case PGVALUE_STRING:
			pgvalue_set_string( val, pgvalue_to_string( val ) );
			RETURN( TRUE );

		case PGVALUE_WSTRING:
			pgvalue_set_wstring( val, pgvalue_to_wstring( val ) );
			RETURN( TRUE );

		case PGVALUE_PTR:
			pgvalue_set_ptr( val, pgvalue_to_ptr( val ) );
			RETURN( TRUE );

	}

	RETURN( FALSE );
}

