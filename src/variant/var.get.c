/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include <phorward.h>

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_byte()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pbyte data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pbyte		The pbyte-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pbyte pvar_get_byte( pvar* var )
{
	PROC( "pvar_get_byte" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_BYTE)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_BYTE ) != ERR_OK )
				RETURN( (pbyte)0 );
		}
		else
			RETURN( (pbyte)0 );
	}

	RETURN( var->val.b );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_char()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's uchar data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		uchar		The uchar-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar pvar_get_char( pvar* var )
{
	PROC( "pvar_get_char" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_CHAR)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_CHAR ) != ERR_OK )
				RETURN( (uchar)0 );
		}
		else
			RETURN( (uchar)0 );
	}

	RETURN( var->val.c );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_int()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pint data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pint		The pint-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pvar_get_int( pvar* var )
{
	PROC( "pvar_get_int" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_INT)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_INT ) != ERR_OK )
				RETURN( (pint)0 );
		}
		else
			RETURN( (pint)0 );
	}

	RETURN( var->val.i );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_long()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's plong data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		plong		The plong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong pvar_get_long( pvar* var )
{
	PROC( "pvar_get_long" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_LONG)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_LONG ) != ERR_OK )
				RETURN( (plong)0 );
		}
		else
			RETURN( (plong)0 );
	}

	RETURN( var->val.l );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_ulong()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pulong data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pulong		The pulong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong pvar_get_ulong( pvar* var )
{
	PROC( "pvar_get_ulong" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_ULONG)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_ULONG ) != ERR_OK )
				RETURN( (pulong)0 );
		}
		else
			RETURN( (pulong)0 );
	}

	RETURN( var->val.ul );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_float()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pfloat data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pfloat		The pfloat-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat pvar_get_float( pvar* var )
{
	PROC( "pvar_get_float" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_FLOAT)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_FLOAT ) != ERR_OK )
				RETURN( (pfloat)0.0 );
		}
		else
			RETURN( (pfloat)0.0 );
	}

	RETURN( var->val.f );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_double()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pdouble data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pdouble		The pdouble-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble pvar_get_double( pvar* var )
{
	PROC( "pvar_get_double" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_DOUBLE)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_DOUBLE ) != ERR_OK )
				RETURN( (pdouble)0.0 );
		}
		else
			RETURN( (pdouble)0.0 );
	}

	RETURN( var->val.d );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_cstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's uchar* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		uchar*		The uchar*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pvar_get_cstring( pvar* var )
{
	PROC( "pvar_get_cstring" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_STRING)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_STRING ) != ERR_OK )
				RETURN( (uchar*)NULL );
		}
		else
			RETURN( (uchar*)NULL );
	}

	RETURN( var->val.s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_string()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's uchar* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		uchar*		The uchar*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pvar_get_string( pvar* var )
{
	PROC( "pvar_get_string" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_STRING)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_STRING ) != ERR_OK )
				RETURN( (uchar*)NULL );
		}
		else
			RETURN( (uchar*)NULL );
	}

	RETURN( var->val.s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_wcstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pchar* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pchar*		The pchar*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pvar_get_wcstring( pvar* var )
{
	PROC( "pvar_get_wcstring" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_WSTRING)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_WSTRING ) != ERR_OK )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( var->val.ws );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_get_wstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Returns a variant's pchar* data value and type.
					If the variant exists in another data type, it will be
					converted, so use it carefully if data loss is not desired!

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pchar*		The pchar*-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pvar_get_wstring( pvar* var )
{
	PROC( "pvar_get_wstring" );
	PARMS( "var", "%p", var );

	if( pvar_type( var ) != PVAR_WSTRING)
	{
		if( pvar_is_convertible( var ) )
		{
			MSG( "Conversion allowed and required" );
			if( pvar_convert( var, PVAR_WSTRING ) != ERR_OK )
				RETURN( (pchar*)NULL );
		}
		else
			RETURN( (pchar*)NULL );
	}

	RETURN( var->val.ws );
}

