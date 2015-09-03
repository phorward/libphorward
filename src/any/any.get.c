/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Returns the char data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as char.
This value could be converted from the original value.
*/
char pany_get_char( pany* val )
{
	PROC( "pany_get_char" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	if( val->type != PANYTYPE_CHAR)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_CHAR ) )
				RETURN( (char)0 );
		}
		else
			RETURN( (char)0 );
	}

	RETURN( val->val.c );
}

/** Returns the int data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as int.
This value could be converted from the original value.
*/
int pany_get_int( pany* val )
{
	PROC( "pany_get_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	if( val->type != PANYTYPE_INT)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_INT ) )
				RETURN( (int)0 );
		}
		else
			RETURN( (int)0 );
	}

	RETURN( val->val.i );
}

/** Returns the long data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as long.
This value could be converted from the original value.
*/
long pany_get_long( pany* val )
{
	PROC( "pany_get_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	if( val->type != PANYTYPE_LONG)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_LONG ) )
				RETURN( (long)0 );
		}
		else
			RETURN( (long)0 );
	}

	RETURN( val->val.l );
}

/** Returns the ulong data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as ulong.
This value could be converted from the original value.
*/
ulong pany_get_ulong( pany* val )
{
	PROC( "pany_get_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	if( val->type != PANYTYPE_ULONG)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_ULONG ) )
				RETURN( (ulong)0 );
		}
		else
			RETURN( (ulong)0 );
	}

	RETURN( val->val.ul );
}

/** Returns the float data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as float.
This value could be converted from the original value.
*/
float pany_get_float( pany* val )
{
	PROC( "pany_get_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	if( val->type != PANYTYPE_FLOAT)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_FLOAT ) )
				RETURN( (float)0.0 );
		}
		else
			RETURN( (float)0.0 );
	}

	RETURN( val->val.f );
}

/** Returns the double data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as double.
This value could be converted from the original value.
*/
double pany_get_double( pany* val )
{
	PROC( "pany_get_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	if( val->type != PANYTYPE_DOUBLE)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_DOUBLE ) )
				RETURN( (double)0.0 );
		}
		else
			RETURN( (double)0.0 );
	}

	RETURN( val->val.d );
}

/** Returns the char* data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as char*.
This value could be converted from the original value.
*/
char* pany_get_cstr( pany* val )
{
	PROC( "pany_get_cstr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( val->type != PANYTYPE_STR)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_STR ) )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( val->val.s );
}

/** Returns the char* data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as char*.
This value could be converted from the original value.
*/
char* pany_get_str( pany* val )
{
	PROC( "pany_get_str" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( val->type != PANYTYPE_STR)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_STR ) )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( val->val.s );
}

/** Returns the wchar_t* data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as wchar_t*.
This value could be converted from the original value.
*/
wchar_t* pany_get_cwcs( pany* val )
{
	PROC( "pany_get_cwcs" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	if( val->type != PANYTYPE_WCS)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_WCS ) )
				RETURN( (wchar_t*)NULL );
		}
		else
			RETURN( (wchar_t*)NULL );
	}

	RETURN( val->val.ws );
}

/** Returns the wchar_t* data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as wchar_t*.
This value could be converted from the original value.
*/
wchar_t* pany_get_wcs( pany* val )
{
	PROC( "pany_get_wcs" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	if( val->type != PANYTYPE_WCS)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_WCS ) )
				RETURN( (wchar_t*)NULL );
		}
		else
			RETURN( (wchar_t*)NULL );
	}

	RETURN( val->val.ws );
}

/** Returns the void* data value if //val//.

If the pany contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pany structure.

The function returns the value assigned to //val// as void*.
This value could be converted from the original value.
*/
void* pany_get_ptr( pany* val )
{
	PROC( "pany_get_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( val->type != PANYTYPE_PTR)
	{
		if( pany_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pany_convert( val, PANYTYPE_PTR ) )
				RETURN( (void*)NULL );
		}
		else
			RETURN( (void*)NULL );
	}

	RETURN( val->val.ptr );
}

