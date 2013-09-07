/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include <phorward.h>

/** Returns a punions pbyte data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pbyte.
This value could be converted from the punions original value.
*/
pbyte punion_get_byte( punion* var )
{
	PROC( "punion_get_byte" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_BYTE)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_BYTE ) != ERR_OK )
				RETURN( (pbyte)0 );
		}
		else
			RETURN( (pbyte)0 );
	}

	RETURN( var->val.b );
}

/** Returns a punions char data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as char.
This value could be converted from the punions original value.
*/
char punion_get_char( punion* var )
{
	PROC( "punion_get_char" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_CHAR)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_CHAR ) != ERR_OK )
				RETURN( (char)0 );
		}
		else
			RETURN( (char)0 );
	}

	RETURN( var->val.c );
}

/** Returns a punions pint data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pint.
This value could be converted from the punions original value.
*/
pint punion_get_int( punion* var )
{
	PROC( "punion_get_int" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_INT)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_INT ) != ERR_OK )
				RETURN( (pint)0 );
		}
		else
			RETURN( (pint)0 );
	}

	RETURN( var->val.i );
}

/** Returns a punions plong data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as plong.
This value could be converted from the punions original value.
*/
plong punion_get_long( punion* var )
{
	PROC( "punion_get_long" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_LONG)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_LONG ) != ERR_OK )
				RETURN( (plong)0 );
		}
		else
			RETURN( (plong)0 );
	}

	RETURN( var->val.l );
}

/** Returns a punions pulong data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pulong.
This value could be converted from the punions original value.
*/
pulong punion_get_ulong( punion* var )
{
	PROC( "punion_get_ulong" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_ULONG)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_ULONG ) != ERR_OK )
				RETURN( (pulong)0 );
		}
		else
			RETURN( (pulong)0 );
	}

	RETURN( var->val.ul );
}

/** Returns a punions pfloat data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pfloat.
This value could be converted from the punions original value.
*/
pfloat punion_get_float( punion* var )
{
	PROC( "punion_get_float" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_FLOAT)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_FLOAT ) != ERR_OK )
				RETURN( (pfloat)0.0 );
		}
		else
			RETURN( (pfloat)0.0 );
	}

	RETURN( var->val.f );
}

/** Returns a punions pdouble data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pdouble.
This value could be converted from the punions original value.
*/
pdouble punion_get_double( punion* var )
{
	PROC( "punion_get_double" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_DOUBLE)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_DOUBLE ) != ERR_OK )
				RETURN( (pdouble)0.0 );
		}
		else
			RETURN( (pdouble)0.0 );
	}

	RETURN( var->val.d );
}

/** Returns a punions char* data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as char*.
This value could be converted from the punions original value.
*/
char* punion_get_cstring( punion* var )
{
	PROC( "punion_get_cstring" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_STRING)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_STRING ) != ERR_OK )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( var->val.s );
}

/** Returns a punions char* data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as char*.
This value could be converted from the punions original value.
*/
char* punion_get_string( punion* var )
{
	PROC( "punion_get_string" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_STRING)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_STRING ) != ERR_OK )
				RETURN( (char*)NULL );
		}
		else
			RETURN( (char*)NULL );
	}

	RETURN( var->val.s );
}

/** Returns a punions pchar* data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pchar*.
This value could be converted from the punions original value.
*/
pchar* punion_get_wcstring( punion* var )
{
	PROC( "punion_get_wcstring" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_WSTRING)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_WSTRING ) != ERR_OK )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( var->val.ws );
}

/** Returns a punions pchar* data value.

If the variant exists in another data type, it will be converted,
so use it carefully if data loss is not desired.

//var// is the pointer to the punion structure.

The function returns the value assigned to //var// as pchar*.
This value could be converted from the punions original value.
*/
pchar* punion_get_wstring( punion* var )
{
	PROC( "punion_get_wstring" );
	PARMS( "var", "%p", var );

	if( punion_type( var ) != PUNION_WSTRING)
	{
		if( punion_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( punion_convert( var, PUNION_WSTRING ) != ERR_OK )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( var->val.ws );
}

