/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "../phorward.h"

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_byte()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pbyte variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pbyte		The pbyte-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pbyte pvar_to_byte( pvar* var )
{
	PROC( "pvar_to_byte" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( var->val.b );
		case PVAR_CHAR:
			RETURN( (pbyte)var->val.c );
		case PVAR_INT:
			RETURN( (pbyte)var->val.i );
		case PVAR_LONG:
			RETURN( (pbyte)var->val.l );
		case PVAR_ULONG:
			RETURN( (pbyte)var->val.ul );
		case PVAR_FLOAT:
			RETURN( (pbyte)var->val.f );
		case PVAR_DOUBLE:
			RETURN( (pbyte)var->val.d );
		case PVAR_STRING:
			RETURN( (pbyte)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PVAR_WSTRING:
			RETURN( (pbyte)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pbyte)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_char()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a uchar variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		uchar		The uchar-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar pvar_to_char( pvar* var )
{
	PROC( "pvar_to_char" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (uchar)var->val.b );
		case PVAR_CHAR:
			RETURN( var->val.c );
		case PVAR_INT:
			RETURN( (uchar)var->val.i );
		case PVAR_LONG:
			RETURN( (uchar)var->val.l );
		case PVAR_ULONG:
			RETURN( (uchar)var->val.ul );
		case PVAR_FLOAT:
			RETURN( (uchar)var->val.f );
		case PVAR_DOUBLE:
			RETURN( (uchar)var->val.d );
		case PVAR_STRING:
			RETURN( (uchar)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PVAR_WSTRING:
			RETURN( (uchar)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (uchar)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_int()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pint variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pint		The pint-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pvar_to_int( pvar* var )
{
	PROC( "pvar_to_int" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (pint)var->val.b );
		case PVAR_CHAR:
			RETURN( (pint)var->val.c );
		case PVAR_INT:
			RETURN( var->val.i );
		case PVAR_LONG:
			RETURN( (pint)var->val.l );
		case PVAR_ULONG:
			RETURN( (pint)var->val.ul );
		case PVAR_FLOAT:
			RETURN( (pint)var->val.f );
		case PVAR_DOUBLE:
			RETURN( (pint)var->val.d );
		case PVAR_STRING:
			RETURN( (pint)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PVAR_WSTRING:
			RETURN( (pint)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pint)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_long()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a plong variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		plong		The plong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong pvar_to_long( pvar* var )
{
	PROC( "pvar_to_long" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (plong)var->val.b );
		case PVAR_CHAR:
			RETURN( (plong)var->val.c );
		case PVAR_INT:
			RETURN( (plong)var->val.i );
		case PVAR_LONG:
			RETURN( var->val.l );
		case PVAR_ULONG:
			RETURN( (plong)var->val.ul );
		case PVAR_FLOAT:
			RETURN( (plong)var->val.f );
		case PVAR_DOUBLE:
			RETURN( (plong)var->val.d );
		case PVAR_STRING:
			RETURN( (plong)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PVAR_WSTRING:
			RETURN( (plong)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (plong)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_ulong()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pulong variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pulong		The pulong-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong pvar_to_ulong( pvar* var )
{
	PROC( "pvar_to_ulong" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (pulong)var->val.b );
		case PVAR_CHAR:
			RETURN( (pulong)var->val.c );
		case PVAR_INT:
			RETURN( (pulong)var->val.i );
		case PVAR_LONG:
			RETURN( (pulong)var->val.l );
		case PVAR_ULONG:
			RETURN( var->val.ul );
		case PVAR_FLOAT:
			RETURN( (pulong)var->val.f );
		case PVAR_DOUBLE:
			RETURN( (pulong)var->val.d );
		case PVAR_STRING:
			RETURN( (pulong)pstrtol( var->val.s, (uchar**)NULL, 0 ) );
		case PVAR_WSTRING:
			RETURN( (pulong)Pstrtol( var->val.ws, (pchar**)NULL, 0 ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pulong)0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_float()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pfloat variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pfloat		The pfloat-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat pvar_to_float( pvar* var )
{
	PROC( "pvar_to_float" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (pfloat)var->val.b );
		case PVAR_CHAR:
			RETURN( (pfloat)var->val.c );
		case PVAR_INT:
			RETURN( (pfloat)var->val.i );
		case PVAR_LONG:
			RETURN( (pfloat)var->val.l );
		case PVAR_ULONG:
			RETURN( (pfloat)var->val.ul );
		case PVAR_FLOAT:
			RETURN( var->val.f );
		case PVAR_DOUBLE:
			RETURN( (pfloat)var->val.d );
		case PVAR_STRING:
			RETURN( (pfloat)pstrtod( var->val.s, (uchar**)NULL ) );
		case PVAR_WSTRING:
			RETURN( (pfloat)Pstrtod( var->val.ws, (pchar**)NULL ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pfloat)0.0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_double()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pdouble variable.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pdouble		The pdouble-value of the variant.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble pvar_to_double( pvar* var )
{
	PROC( "pvar_to_double" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (pdouble)var->val.b );
		case PVAR_CHAR:
			RETURN( (pdouble)var->val.c );
		case PVAR_INT:
			RETURN( (pdouble)var->val.i );
		case PVAR_LONG:
			RETURN( (pdouble)var->val.l );
		case PVAR_ULONG:
			RETURN( (pdouble)var->val.ul );
		case PVAR_FLOAT:
			RETURN( (pdouble)var->val.f );
		case PVAR_DOUBLE:
			RETURN( var->val.d );
		case PVAR_STRING:
			RETURN( (pdouble)pstrtod( var->val.s, (uchar**)NULL ) );
		case PVAR_WSTRING:
			RETURN( (pdouble)Pstrtod( var->val.ws, (pchar**)NULL ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pdouble)0.0 );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_string()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a uchar* variable.
					The returned memory is allocated, and must be freed by the caller.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		uchar*		The uchar*-value of the variant.
					Memory must be freed by caller!

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pvar_to_string( pvar* var )
{
	PROC( "pvar_to_string" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (uchar*)plong_to_uchar( (plong)var->val.b ) );
		case PVAR_CHAR:
			RETURN( (uchar*)plong_to_uchar( (plong)var->val.c ) );
		case PVAR_INT:
			RETURN( (uchar*)plong_to_uchar( (plong)var->val.i ) );
		case PVAR_LONG:
			RETURN( (uchar*)plong_to_uchar( var->val.l ) );
		case PVAR_ULONG:
			RETURN( (uchar*)pulong_to_uchar( var->val.ul ) );
		case PVAR_FLOAT:
			RETURN( (uchar*)pdouble_to_uchar( (double)var->val.f ) );
		case PVAR_DOUBLE:
			RETURN( (uchar*)pdouble_to_uchar( var->val.d ) );
		case PVAR_STRING:
			RETURN( var->val.s );
		case PVAR_WSTRING:
			RETURN( (uchar*)pchar_to_uchar( var->val.ws, FALSE ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (uchar*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_to_wstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a variant's current value into a pchar* variable.
					The returned memory is allocated, and must be freed by the caller.

	Parameters:		pvar*	var	Pointer to pvar structure.

	Returns:		pchar*		The pchar*-value of the variant.
					Memory must be freed by caller!

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pvar_to_wstring( pvar* var )
{
	PROC( "pvar_to_wstring" );
	PARMS( "var", "%p", var );

	switch( pvar_type( var ) )
	{
		case PVAR_BYTE:
			RETURN( (pchar*)plong_to_pchar( (plong)var->val.b ) );
		case PVAR_CHAR:
			RETURN( (pchar*)plong_to_pchar( (plong)var->val.c ) );
		case PVAR_INT:
			RETURN( (pchar*)plong_to_pchar( (plong)var->val.i ) );
		case PVAR_LONG:
			RETURN( (pchar*)plong_to_pchar( var->val.l ) );
		case PVAR_ULONG:
			RETURN( (pchar*)pulong_to_pchar( var->val.ul ) );
		case PVAR_FLOAT:
			RETURN( (pchar*)pdouble_to_pchar( (double)var->val.f ) );
		case PVAR_DOUBLE:
			RETURN( (pchar*)pdouble_to_pchar( var->val.d ) );
		case PVAR_STRING:
			RETURN( (pchar*)uchar_to_pchar( var->val.s, FALSE ) );
		case PVAR_WSTRING:
			RETURN( var->val.ws );
	}

	MSG( "Can't convert this type!" );
	RETURN( (pchar*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_convert()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Converts a pvar-structure to any supported type.

	Parameters:		pvar*	var		Pointer to pvar structure.
					pbyte	type	Type to which var should be converted to.

	Returns:		pint	ERR_OK	on success, else an ERR_-define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pvar_convert( pvar* var, pbyte type )
{
	PROC( "pvar_convert" );
	PARMS( "var", "%p", var );
	PARMS( "type", "%d", type );

	if( pvar_type( var ) == type )
		RETURN( ERR_OK );

	switch( type )
	{
		case PVAR_BYTE:
			pvar_set_byte( var, pvar_to_byte( var ) );
			RETURN( ERR_OK );

		case PVAR_CHAR:
			pvar_set_char( var, pvar_to_char( var ) );
			RETURN( ERR_OK );

		case PVAR_INT:
			pvar_set_int( var, pvar_to_int( var ) );
			RETURN( ERR_OK );

		case PVAR_LONG:
			pvar_set_long( var, pvar_to_long( var ) );
			RETURN( ERR_OK );

		case PVAR_ULONG:
			pvar_set_ulong( var, pvar_to_ulong( var ) );
			RETURN( ERR_OK );

		case PVAR_FLOAT:
			pvar_set_float( var, pvar_to_float( var ) );
			RETURN( ERR_OK );

		case PVAR_DOUBLE:
			pvar_set_double( var, pvar_to_double( var ) );
			RETURN( ERR_OK );

		case PVAR_STRING:
			pvar_set_string( var, pvar_to_string( var ) );
			RETURN( ERR_OK );

		case PVAR_WSTRING:
			pvar_set_wstring( var, pvar_to_wstring( var ) );
			RETURN( ERR_OK );

	}

	RETURN( ERR_FAILURE );
}

