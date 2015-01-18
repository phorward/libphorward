/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Returns the char data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as char.
This value could be converted from the original value.
*/
char pvalue_get_char( pvalue* val )
{
	PROC( "pvalue_get_char" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	if( val->type != PGVALUETYPE_CHAR)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_CHAR ) )
				RETURN( (char)0 );
		}
		else
			RETURN( (char)0 );
	}

	RETURN( val->val.c );
}

/** Returns the int data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as int.
This value could be converted from the original value.
*/
int pvalue_get_int( pvalue* val )
{
	PROC( "pvalue_get_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	if( val->type != PGVALUETYPE_INT)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_INT ) )
				RETURN( (int)0 );
		}
		else
			RETURN( (int)0 );
	}

	RETURN( val->val.i );
}

/** Returns the long data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as long.
This value could be converted from the original value.
*/
long pvalue_get_long( pvalue* val )
{
	PROC( "pvalue_get_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	if( val->type != PGVALUETYPE_LONG)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_LONG ) )
				RETURN( (long)0 );
		}
		else
			RETURN( (long)0 );
	}

	RETURN( val->val.l );
}

/** Returns the ulong data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as ulong.
This value could be converted from the original value.
*/
ulong pvalue_get_ulong( pvalue* val )
{
	PROC( "pvalue_get_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	if( val->type != PGVALUETYPE_ULONG)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_ULONG ) )
				RETURN( (ulong)0 );
		}
		else
			RETURN( (ulong)0 );
	}

	RETURN( val->val.ul );
}

/** Returns the float data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as float.
This value could be converted from the original value.
*/
float pvalue_get_float( pvalue* val )
{
	PROC( "pvalue_get_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	if( val->type != PGVALUETYPE_FLOAT)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_FLOAT ) )
				RETURN( (float)0.0 );
		}
		else
			RETURN( (float)0.0 );
	}

	RETURN( val->val.f );
}

/** Returns the double data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as double.
This value could be converted from the original value.
*/
double pvalue_get_double( pvalue* val )
{
	PROC( "pvalue_get_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	if( val->type != PGVALUETYPE_DOUBLE)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_DOUBLE ) )
				RETURN( (double)0.0 );
		}
		else
			RETURN( (double)0.0 );
	}

	RETURN( val->val.d );
}

/** Returns the char* data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as char*.
This value could be converted from the original value.
*/
char* pvalue_get_cstr( pvalue* val )
{
	PROC( "pvalue_get_cstr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( val->type != PGVALUETYPE_STR)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_STR ) )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( val->val.s );
}

/** Returns the char* data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as char*.
This value could be converted from the original value.
*/
char* pvalue_get_str( pvalue* val )
{
	PROC( "pvalue_get_str" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( val->type != PGVALUETYPE_STR)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_STR ) )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( val->val.s );
}

/** Returns the wchar_t* data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as wchar_t*.
This value could be converted from the original value.
*/
wchar_t* pvalue_get_cwcs( pvalue* val )
{
	PROC( "pvalue_get_cwcs" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	if( val->type != PGVALUETYPE_WCS)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_WCS ) )
				RETURN( (wchar_t*)NULL );
		}
		else
			RETURN( (wchar_t*)NULL );
	}

	RETURN( val->val.ws );
}

/** Returns the wchar_t* data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as wchar_t*.
This value could be converted from the original value.
*/
wchar_t* pvalue_get_wcs( pvalue* val )
{
	PROC( "pvalue_get_wcs" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	if( val->type != PGVALUETYPE_WCS)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_WCS ) )
				RETURN( (wchar_t*)NULL );
		}
		else
			RETURN( (wchar_t*)NULL );
	}

	RETURN( val->val.ws );
}

/** Returns the void* data value if //val//.

If the pvalue contains another data type, it will be converted,
so use it carefully if data loss is not wanted.

//val// is the pointer to the pvalue structure.

The function returns the value assigned to //val// as void*.
This value could be converted from the original value.
*/
void* pvalue_get_ptr( pvalue* val )
{
	PROC( "pvalue_get_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( val->type != PGVALUETYPE_PTR)
	{
		if( pvalue_get_autoconvert( val ) )
		{
			MSG( "Conversion allowed and required" );
			if( !pvalue_convert( val, PGVALUETYPE_PTR ) )
				RETURN( (void*)NULL );
		}
		else
			RETURN( (void*)NULL );
	}

	RETURN( val->val.ptr );
}

