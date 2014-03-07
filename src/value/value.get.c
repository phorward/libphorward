/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Returns the pbyte data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pbyte.
This value could be converted from the original value.
*/
pbyte pgvalue_get_byte( pgvalue* val )
{
	PROC( "pgvalue_get_byte" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_BYTE)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_BYTE ) )
				RETURN( (pbyte)0 );
		}
		else
			RETURN( (pbyte)0 );
	}

	RETURN( val->val.b );
}

/** Returns the char data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as char.
This value could be converted from the original value.
*/
char pgvalue_get_char( pgvalue* val )
{
	PROC( "pgvalue_get_char" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_CHAR)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_CHAR ) )
				RETURN( (char)0 );
		}
		else
			RETURN( (char)0 );
	}

	RETURN( val->val.c );
}

/** Returns the pint data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pint.
This value could be converted from the original value.
*/
pint pgvalue_get_int( pgvalue* val )
{
	PROC( "pgvalue_get_int" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_INT)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_INT ) )
				RETURN( (pint)0 );
		}
		else
			RETURN( (pint)0 );
	}

	RETURN( val->val.i );
}

/** Returns the plong data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as plong.
This value could be converted from the original value.
*/
plong pgvalue_get_long( pgvalue* val )
{
	PROC( "pgvalue_get_long" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_LONG)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_LONG ) )
				RETURN( (plong)0 );
		}
		else
			RETURN( (plong)0 );
	}

	RETURN( val->val.l );
}

/** Returns the pulong data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pulong.
This value could be converted from the original value.
*/
pulong pgvalue_get_ulong( pgvalue* val )
{
	PROC( "pgvalue_get_ulong" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_ULONG)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_ULONG ) )
				RETURN( (pulong)0 );
		}
		else
			RETURN( (pulong)0 );
	}

	RETURN( val->val.ul );
}

/** Returns the pfloat data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pfloat.
This value could be converted from the original value.
*/
pfloat pgvalue_get_float( pgvalue* val )
{
	PROC( "pgvalue_get_float" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_FLOAT)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_FLOAT ) )
				RETURN( (pfloat)0.0 );
		}
		else
			RETURN( (pfloat)0.0 );
	}

	RETURN( val->val.f );
}

/** Returns the pdouble data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pdouble.
This value could be converted from the original value.
*/
pdouble pgvalue_get_double( pgvalue* val )
{
	PROC( "pgvalue_get_double" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_DOUBLE)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_DOUBLE ) )
				RETURN( (pdouble)0.0 );
		}
		else
			RETURN( (pdouble)0.0 );
	}

	RETURN( val->val.d );
}

/** Returns the char* data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as char*.
This value could be converted from the original value.
*/
char* pgvalue_get_cstring( pgvalue* val )
{
	PROC( "pgvalue_get_cstring" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_STRING)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_STRING ) )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( val->val.s );
}

/** Returns the char* data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as char*.
This value could be converted from the original value.
*/
char* pgvalue_get_string( pgvalue* val )
{
	PROC( "pgvalue_get_string" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_STRING)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_STRING ) )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( val->val.s );
}

/** Returns the pchar* data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pchar*.
This value could be converted from the original value.
*/
pchar* pgvalue_get_wcstring( pgvalue* val )
{
	PROC( "pgvalue_get_wcstring" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_WSTRING)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_WSTRING ) )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( val->val.ws );
}

/** Returns the pchar* data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as pchar*.
This value could be converted from the original value.
*/
pchar* pgvalue_get_wstring( pgvalue* val )
{
	PROC( "pgvalue_get_wstring" );
	PARMS( "val", "%p", val );

	if( pgvalue_type( val ) != PUNION_WSTRING)
	{
		if( pgvalue_is_convertible( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PUNION_WSTRING ) )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( val->val.ws );
}

