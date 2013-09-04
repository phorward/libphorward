/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include <phorward.h>

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_byte()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pbyte data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pbyte		The pbyte-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_char()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's char data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		char		The char-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_int()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pint data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pint		The pint-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_long()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's plong data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		plong		The plong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_ulong()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pulong data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pulong		The pulong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_float()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pfloat data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pfloat		The pfloat-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_double()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pdouble data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pdouble		The pdouble-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_cstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's char* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		char*		The char*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_string()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's char* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		char*		The char*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_wcstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pchar* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pchar*		The pchar*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_get_wstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a punion's pchar* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pchar*		The pchar*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

