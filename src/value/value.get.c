/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	if( val->type != PGVALUE_CHAR)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_CHAR ) )
				RETURN( (char)0 );
		}
		else
			RETURN( (char)0 );
	}

	RETURN( val->val.c );
}

/** Returns the int data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as int.
This value could be converted from the original value.
*/
int pgvalue_get_int( pgvalue* val )
{
	PROC( "pgvalue_get_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	if( val->type != PGVALUE_INT)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_INT ) )
				RETURN( (int)0 );
		}
		else
			RETURN( (int)0 );
	}

	RETURN( val->val.i );
}

/** Returns the long data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as long.
This value could be converted from the original value.
*/
long pgvalue_get_long( pgvalue* val )
{
	PROC( "pgvalue_get_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	if( val->type != PGVALUE_LONG)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_LONG ) )
				RETURN( (long)0 );
		}
		else
			RETURN( (long)0 );
	}

	RETURN( val->val.l );
}

/** Returns the ulong data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as ulong.
This value could be converted from the original value.
*/
ulong pgvalue_get_ulong( pgvalue* val )
{
	PROC( "pgvalue_get_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	if( val->type != PGVALUE_ULONG)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_ULONG ) )
				RETURN( (ulong)0 );
		}
		else
			RETURN( (ulong)0 );
	}

	RETURN( val->val.ul );
}

/** Returns the float data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as float.
This value could be converted from the original value.
*/
float pgvalue_get_float( pgvalue* val )
{
	PROC( "pgvalue_get_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	if( val->type != PGVALUE_FLOAT)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_FLOAT ) )
				RETURN( (float)0.0 );
		}
		else
			RETURN( (float)0.0 );
	}

	RETURN( val->val.f );
}

/** Returns the double data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as double.
This value could be converted from the original value.
*/
double pgvalue_get_double( pgvalue* val )
{
	PROC( "pgvalue_get_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	if( val->type != PGVALUE_DOUBLE)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_DOUBLE ) )
				RETURN( (double)0.0 );
		}
		else
			RETURN( (double)0.0 );
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( val->type != PGVALUE_STRING)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_STRING ) )
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( val->type != PGVALUE_STRING)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_STRING ) )
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pchar*)NULL );
	}

	if( val->type != PGVALUE_WSTRING)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_WSTRING ) )
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pchar*)NULL );
	}

	if( val->type != PGVALUE_WSTRING)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_WSTRING ) )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( val->val.ws );
}

/** Returns the void* data value if //val//.

If the pgvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pgvalue structure.

The function returns the value assigned to //val// as void*.
This value could be converted from the original value.
*/
void* pgvalue_get_ptr( pgvalue* val )
{
	PROC( "pgvalue_get_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( val->type != PGVALUE_PTR)
	{
		if( pgvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pgvalue_convert( val, PGVALUE_PTR ) )
				RETURN( (void*)NULL );
		}
		else
			RETURN( (void*)NULL );
	}

	RETURN( val->val.ptr );
}

