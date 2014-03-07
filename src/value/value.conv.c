/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Converts the current value of //val// into a pbyte value.

//val// is the pgvalue-object to convert from.

The function returns the pbyte-value of the variant.
*/
pbyte pgvalue_to_byte( pgvalue* val )
{
	PROC( "pgvalue_to_byte" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( val->val.b );
		case PUNION_CHAR:
			RETURN( (pbyte)val->val.c );
		case PUNION_INT:
			RETURN( (pbyte)val->val.i );
		case PUNION_LONG:
			RETURN( (pbyte)val->val.l );
		case PUNION_ULONG:
			RETURN( (pbyte)val->val.ul );
		case PUNION_FLOAT:
			RETURN( (pbyte)val->val.f );
		case PUNION_DOUBLE:
			RETURN( (pbyte)val->val.d );
		case PUNION_STRING:
			RETURN( (pbyte)strtol( val->val.s, (char**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (pbyte)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pbyte)0 );
}

/** Converts the current value of //val// into a char value.

//val// is the pgvalue-object to convert from.

The function returns the char-value of the variant.
*/
char pgvalue_to_char( pgvalue* val )
{
	PROC( "pgvalue_to_char" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (char)val->val.b );
		case PUNION_CHAR:
			RETURN( val->val.c );
		case PUNION_INT:
			RETURN( (char)val->val.i );
		case PUNION_LONG:
			RETURN( (char)val->val.l );
		case PUNION_ULONG:
			RETURN( (char)val->val.ul );
		case PUNION_FLOAT:
			RETURN( (char)val->val.f );
		case PUNION_DOUBLE:
			RETURN( (char)val->val.d );
		case PUNION_STRING:
			RETURN( (char)strtol( val->val.s, (char**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (char)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char)0 );
}

/** Converts the current value of //val// into a pint value.

//val// is the pgvalue-object to convert from.

The function returns the pint-value of the variant.
*/
pint pgvalue_to_int( pgvalue* val )
{
	PROC( "pgvalue_to_int" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (pint)val->val.b );
		case PUNION_CHAR:
			RETURN( (pint)val->val.c );
		case PUNION_INT:
			RETURN( val->val.i );
		case PUNION_LONG:
			RETURN( (pint)val->val.l );
		case PUNION_ULONG:
			RETURN( (pint)val->val.ul );
		case PUNION_FLOAT:
			RETURN( (pint)val->val.f );
		case PUNION_DOUBLE:
			RETURN( (pint)val->val.d );
		case PUNION_STRING:
			RETURN( (pint)strtol( val->val.s, (char**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (pint)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pint)0 );
}

/** Converts the current value of //val// into a plong value.

//val// is the pgvalue-object to convert from.

The function returns the plong-value of the variant.
*/
plong pgvalue_to_long( pgvalue* val )
{
	PROC( "pgvalue_to_long" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (plong)val->val.b );
		case PUNION_CHAR:
			RETURN( (plong)val->val.c );
		case PUNION_INT:
			RETURN( (plong)val->val.i );
		case PUNION_LONG:
			RETURN( val->val.l );
		case PUNION_ULONG:
			RETURN( (plong)val->val.ul );
		case PUNION_FLOAT:
			RETURN( (plong)val->val.f );
		case PUNION_DOUBLE:
			RETURN( (plong)val->val.d );
		case PUNION_STRING:
			RETURN( (plong)strtol( val->val.s, (char**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (plong)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (plong)0 );
}

/** Converts the current value of //val// into a pulong value.

//val// is the pgvalue-object to convert from.

The function returns the pulong-value of the variant.
*/
pulong pgvalue_to_ulong( pgvalue* val )
{
	PROC( "pgvalue_to_ulong" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (pulong)val->val.b );
		case PUNION_CHAR:
			RETURN( (pulong)val->val.c );
		case PUNION_INT:
			RETURN( (pulong)val->val.i );
		case PUNION_LONG:
			RETURN( (pulong)val->val.l );
		case PUNION_ULONG:
			RETURN( val->val.ul );
		case PUNION_FLOAT:
			RETURN( (pulong)val->val.f );
		case PUNION_DOUBLE:
			RETURN( (pulong)val->val.d );
		case PUNION_STRING:
			RETURN( (pulong)strtol( val->val.s, (char**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (pulong)wcstol( val->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pulong)0 );
}

/** Converts the current value of //val// into a pfloat value.

//val// is the pgvalue-object to convert from.

The function returns the pfloat-value of the variant.
*/
pfloat pgvalue_to_float( pgvalue* val )
{
	PROC( "pgvalue_to_float" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (pfloat)val->val.b );
		case PUNION_CHAR:
			RETURN( (pfloat)val->val.c );
		case PUNION_INT:
			RETURN( (pfloat)val->val.i );
		case PUNION_LONG:
			RETURN( (pfloat)val->val.l );
		case PUNION_ULONG:
			RETURN( (pfloat)val->val.ul );
		case PUNION_FLOAT:
			RETURN( val->val.f );
		case PUNION_DOUBLE:
			RETURN( (pfloat)val->val.d );
		case PUNION_STRING:
			RETURN( (pfloat)strtod( val->val.s, (char**)NULL ) );
		case PUNION_WSTRING:
			RETURN( (pfloat)wcstod( val->val.ws, (pchar**)NULL ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pfloat)0.0 );
}

/** Converts the current value of //val// into a pdouble value.

//val// is the pgvalue-object to convert from.

The function returns the pdouble-value of the variant.
*/
pdouble pgvalue_to_double( pgvalue* val )
{
	PROC( "pgvalue_to_double" );
	PARMS( "val", "%p", val );

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (pdouble)val->val.b );
		case PUNION_CHAR:
			RETURN( (pdouble)val->val.c );
		case PUNION_INT:
			RETURN( (pdouble)val->val.i );
		case PUNION_LONG:
			RETURN( (pdouble)val->val.l );
		case PUNION_ULONG:
			RETURN( (pdouble)val->val.ul );
		case PUNION_FLOAT:
			RETURN( (pdouble)val->val.f );
		case PUNION_DOUBLE:
			RETURN( val->val.d );
		case PUNION_STRING:
			RETURN( (pdouble)strtod( val->val.s, (char**)NULL ) );
		case PUNION_WSTRING:
			RETURN( (pdouble)wcstod( val->val.ws, (pchar**)NULL ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pdouble)0.0 );
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

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (char*)plong_to_uchar( (plong)val->val.b ) );
		case PUNION_CHAR:
			RETURN( (char*)plong_to_uchar( (plong)val->val.c ) );
		case PUNION_INT:
			RETURN( (char*)plong_to_uchar( (plong)val->val.i ) );
		case PUNION_LONG:
			RETURN( (char*)plong_to_uchar( val->val.l ) );
		case PUNION_ULONG:
			RETURN( (char*)pulong_to_uchar( val->val.ul ) );
		case PUNION_FLOAT:
			RETURN( (char*)pdouble_to_uchar( (double)val->val.f ) );
		case PUNION_DOUBLE:
			RETURN( (char*)pdouble_to_uchar( val->val.d ) );
		case PUNION_STRING:
			RETURN( val->val.s );
		case PUNION_WSTRING:
			RETURN( (char*)wchar_to_u8( val->val.ws, FALSE ) );
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

	switch( pgvalue_type( val ) )
	{
		case PUNION_BYTE:
			RETURN( (pchar*)plong_to_pchar( (plong)val->val.b ) );
		case PUNION_CHAR:
			RETURN( (pchar*)plong_to_pchar( (plong)val->val.c ) );
		case PUNION_INT:
			RETURN( (pchar*)plong_to_pchar( (plong)val->val.i ) );
		case PUNION_LONG:
			RETURN( (pchar*)plong_to_pchar( val->val.l ) );
		case PUNION_ULONG:
			RETURN( (pchar*)pulong_to_pchar( val->val.ul ) );
		case PUNION_FLOAT:
			RETURN( (pchar*)pdouble_to_pchar( (double)val->val.f ) );
		case PUNION_DOUBLE:
			RETURN( (pchar*)pdouble_to_pchar( val->val.d ) );
		case PUNION_STRING:
			RETURN( (pchar*)u8_to_wchar( val->val.s, FALSE ) );
		case PUNION_WSTRING:
			RETURN( val->val.ws );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pchar*)NULL );
}

/** Converts a pgvalue-structure to any supported type.

//val// is the pgvalue-object to be converted.
//type// is the type define to which //val// should be converted to.

The function returns TRUE on success, FALSE else.
*/
pboolean pgvalue_convert( pgvalue* val, pbyte type )
{
	PROC( "pgvalue_convert" );
	PARMS( "val", "%p", val );
	PARMS( "type", "%d", type );

	if( pgvalue_type( val ) == type )
		RETURN( TRUE );

	switch( type )
	{
		case PUNION_BYTE:
			pgvalue_set_byte( val, pgvalue_to_byte( val ) );
			RETURN( TRUE );

		case PUNION_CHAR:
			pgvalue_set_char( val, pgvalue_to_char( val ) );
			RETURN( TRUE );

		case PUNION_INT:
			pgvalue_set_int( val, pgvalue_to_int( val ) );
			RETURN( TRUE );

		case PUNION_LONG:
			pgvalue_set_long( val, pgvalue_to_long( val ) );
			RETURN( TRUE );

		case PUNION_ULONG:
			pgvalue_set_ulong( val, pgvalue_to_ulong( val ) );
			RETURN( TRUE );

		case PUNION_FLOAT:
			pgvalue_set_float( val, pgvalue_to_float( val ) );
			RETURN( TRUE );

		case PUNION_DOUBLE:
			pgvalue_set_double( val, pgvalue_to_double( val ) );
			RETURN( TRUE );

		case PUNION_STRING:
			pgvalue_set_string( val, pgvalue_to_string( val ) );
			RETURN( TRUE );

		case PUNION_WSTRING:
			pgvalue_set_wstring( val, pgvalue_to_wstring( val ) );
			RETURN( TRUE );

	}

	RETURN( FALSE );
}

