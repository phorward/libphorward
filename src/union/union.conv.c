/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include <phorward.h>

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_byte()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pbyte variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pbyte		The pbyte-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pbyte punion_to_byte( punion* var )
{
	PROC( "punion_to_byte" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( var->val.b );
		case PUNION_CHAR:
			RETURN( (pbyte)var->val.c );
		case PUNION_INT:
			RETURN( (pbyte)var->val.i );
		case PUNION_LONG:
			RETURN( (pbyte)var->val.l );
		case PUNION_ULONG:
			RETURN( (pbyte)var->val.ul );
		case PUNION_FLOAT:
			RETURN( (pbyte)var->val.f );
		case PUNION_DOUBLE:
			RETURN( (pbyte)var->val.d );
		case PUNION_STRING:
			RETURN( (pbyte)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (pbyte)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pbyte)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_char()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a uchar variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		uchar		The uchar-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar punion_to_char( punion* var )
{
	PROC( "punion_to_char" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (uchar)var->val.b );
		case PUNION_CHAR:
			RETURN( var->val.c );
		case PUNION_INT:
			RETURN( (uchar)var->val.i );
		case PUNION_LONG:
			RETURN( (uchar)var->val.l );
		case PUNION_ULONG:
			RETURN( (uchar)var->val.ul );
		case PUNION_FLOAT:
			RETURN( (uchar)var->val.f );
		case PUNION_DOUBLE:
			RETURN( (uchar)var->val.d );
		case PUNION_STRING:
			RETURN( (uchar)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (uchar)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (uchar)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_int()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pint variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pint		The pint-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint punion_to_int( punion* var )
{
	PROC( "punion_to_int" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (pint)var->val.b );
		case PUNION_CHAR:
			RETURN( (pint)var->val.c );
		case PUNION_INT:
			RETURN( var->val.i );
		case PUNION_LONG:
			RETURN( (pint)var->val.l );
		case PUNION_ULONG:
			RETURN( (pint)var->val.ul );
		case PUNION_FLOAT:
			RETURN( (pint)var->val.f );
		case PUNION_DOUBLE:
			RETURN( (pint)var->val.d );
		case PUNION_STRING:
			RETURN( (pint)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (pint)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pint)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_long()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a plong variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		plong		The plong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong punion_to_long( punion* var )
{
	PROC( "punion_to_long" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (plong)var->val.b );
		case PUNION_CHAR:
			RETURN( (plong)var->val.c );
		case PUNION_INT:
			RETURN( (plong)var->val.i );
		case PUNION_LONG:
			RETURN( var->val.l );
		case PUNION_ULONG:
			RETURN( (plong)var->val.ul );
		case PUNION_FLOAT:
			RETURN( (plong)var->val.f );
		case PUNION_DOUBLE:
			RETURN( (plong)var->val.d );
		case PUNION_STRING:
			RETURN( (plong)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (plong)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (plong)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_ulong()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pulong variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pulong		The pulong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong punion_to_ulong( punion* var )
{
	PROC( "punion_to_ulong" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (pulong)var->val.b );
		case PUNION_CHAR:
			RETURN( (pulong)var->val.c );
		case PUNION_INT:
			RETURN( (pulong)var->val.i );
		case PUNION_LONG:
			RETURN( (pulong)var->val.l );
		case PUNION_ULONG:
			RETURN( var->val.ul );
		case PUNION_FLOAT:
			RETURN( (pulong)var->val.f );
		case PUNION_DOUBLE:
			RETURN( (pulong)var->val.d );
		case PUNION_STRING:
			RETURN( (pulong)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PUNION_WSTRING:
			RETURN( (pulong)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pulong)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_float()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pfloat variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pfloat		The pfloat-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat punion_to_float( punion* var )
{
	PROC( "punion_to_float" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (pfloat)var->val.b );
		case PUNION_CHAR:
			RETURN( (pfloat)var->val.c );
		case PUNION_INT:
			RETURN( (pfloat)var->val.i );
		case PUNION_LONG:
			RETURN( (pfloat)var->val.l );
		case PUNION_ULONG:
			RETURN( (pfloat)var->val.ul );
		case PUNION_FLOAT:
			RETURN( var->val.f );
		case PUNION_DOUBLE:
			RETURN( (pfloat)var->val.d );
		case PUNION_STRING:
			RETURN( (pfloat)pstrtod( var->val.s, (uchar**)NULL ) );
		case PUNION_WSTRING:
			RETURN( (pfloat)Pstrtod( var->val.ws, (pchar**)NULL ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pfloat)0.0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_double()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pdouble variable.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pdouble		The pdouble-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble punion_to_double( punion* var )
{
	PROC( "punion_to_double" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (pdouble)var->val.b );
		case PUNION_CHAR:
			RETURN( (pdouble)var->val.c );
		case PUNION_INT:
			RETURN( (pdouble)var->val.i );
		case PUNION_LONG:
			RETURN( (pdouble)var->val.l );
		case PUNION_ULONG:
			RETURN( (pdouble)var->val.ul );
		case PUNION_FLOAT:
			RETURN( (pdouble)var->val.f );
		case PUNION_DOUBLE:
			RETURN( var->val.d );
		case PUNION_STRING:
			RETURN( (pdouble)pstrtod( var->val.s, (uchar**)NULL ) );
		case PUNION_WSTRING:
			RETURN( (pdouble)Pstrtod( var->val.ws, (pchar**)NULL ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pdouble)0.0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_string()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a uchar* variable.
					The returned memory is allocated, and must be freed by the caller.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		uchar*		The uchar*-value of the variant.
					Memory must be freed by caller!

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* punion_to_string( punion* var )
{
	PROC( "punion_to_string" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (uchar*)plong_to_uchar( (plong)var->val.b ) );
		case PUNION_CHAR:
			RETURN( (uchar*)plong_to_uchar( (plong)var->val.c ) );
		case PUNION_INT:
			RETURN( (uchar*)plong_to_uchar( (plong)var->val.i ) );
		case PUNION_LONG:
			RETURN( (uchar*)plong_to_uchar( var->val.l ) );
		case PUNION_ULONG:
			RETURN( (uchar*)pulong_to_uchar( var->val.ul ) );
		case PUNION_FLOAT:
			RETURN( (uchar*)pdouble_to_uchar( (double)var->val.f ) );
		case PUNION_DOUBLE:
			RETURN( (uchar*)pdouble_to_uchar( var->val.d ) );
		case PUNION_STRING:
			RETURN( var->val.s );
		case PUNION_WSTRING:
			RETURN( (uchar*)pchar_to_uchar( var->val.ws, FALSE ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (uchar*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_to_wstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pchar* variable.
					The returned memory is allocated, and must be freed by the caller.

	Parameters:		punion*	var	Pointer to punion structure.

	Returns:		pchar*		The pchar*-value of the variant.
					Memory must be freed by caller!

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* punion_to_wstring( punion* var )
{
	PROC( "punion_to_wstring" );
	PARMS( "var", "%p", var );

	switch( punion_type( var ) )
	{
		case PUNION_BYTE:
			RETURN( (pchar*)plong_to_pchar( (plong)var->val.b ) );
		case PUNION_CHAR:
			RETURN( (pchar*)plong_to_pchar( (plong)var->val.c ) );
		case PUNION_INT:
			RETURN( (pchar*)plong_to_pchar( (plong)var->val.i ) );
		case PUNION_LONG:
			RETURN( (pchar*)plong_to_pchar( var->val.l ) );
		case PUNION_ULONG:
			RETURN( (pchar*)pulong_to_pchar( var->val.ul ) );
		case PUNION_FLOAT:
			RETURN( (pchar*)pdouble_to_pchar( (double)var->val.f ) );
		case PUNION_DOUBLE:
			RETURN( (pchar*)pdouble_to_pchar( var->val.d ) );
		case PUNION_STRING:
			RETURN( (pchar*)uchar_to_pchar( var->val.s, FALSE ) );
		case PUNION_WSTRING:
			RETURN( var->val.ws );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pchar*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_convert()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a punion-structure to any supported type.

	Parameters:		punion*	var		Pointer to punion structure.
					pbyte	type	Type to which var should be converted to.

	Returns:		pint	ERR_OK	on success, else an ERR_-define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint punion_convert( punion* var, pbyte type )
{
	PROC( "punion_convert" );
	PARMS( "var", "%p", var );
	PARMS( "type", "%d", type );

	if( punion_type( var ) == type )
		RETURN( ERR_OK );

	switch( type )
	{
		case PUNION_BYTE:
			punion_set_byte( var, punion_to_byte( var ) );
			RETURN( ERR_OK );

		case PUNION_CHAR:
			punion_set_char( var, punion_to_char( var ) );
			RETURN( ERR_OK );

		case PUNION_INT:
			punion_set_int( var, punion_to_int( var ) );
			RETURN( ERR_OK );

		case PUNION_LONG:
			punion_set_long( var, punion_to_long( var ) );
			RETURN( ERR_OK );

		case PUNION_ULONG:
			punion_set_ulong( var, punion_to_ulong( var ) );
			RETURN( ERR_OK );

		case PUNION_FLOAT:
			punion_set_float( var, punion_to_float( var ) );
			RETURN( ERR_OK );

		case PUNION_DOUBLE:
			punion_set_double( var, punion_to_double( var ) );
			RETURN( ERR_OK );

		case PUNION_STRING:
			punion_set_string( var, punion_to_string( var ) );
			RETURN( ERR_OK );

		case PUNION_WSTRING:
			punion_set_wstring( var, punion_to_wstring( var ) );
			RETURN( ERR_OK );

	}

	RETURN( ERR_FAILURE );
}

