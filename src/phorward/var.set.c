/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_byte()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pbyte data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pbyte	b	Value to be assigned.

	Returns:		pbyte		The value of b.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pbyte pvar_set_byte( pvar* var, pbyte b )
{
	PROC( "pvar_set_byte" );
	PARMS( "var", "%p", var );
	PARMS( "b", "%d", b );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_BYTE;
	var->val.b = b;

	RETURN( var->val.b );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_char()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's uchar data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					uchar	c	Value to be assigned.

	Returns:		uchar		The value of c.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar pvar_set_char( pvar* var, uchar c )
{
	PROC( "pvar_set_char" );
	PARMS( "var", "%p", var );
	PARMS( "c", "%c", c );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_CHAR;
	var->val.c = c;

	RETURN( var->val.c );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_int()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pint data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pint	i	Value to be assigned.

	Returns:		pint		The value of i.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pvar_set_int( pvar* var, pint i )
{
	PROC( "pvar_set_int" );
	PARMS( "var", "%p", var );
	PARMS( "i", "%d", i );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_INT;
	var->val.i = i;

	RETURN( var->val.i );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_long()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's plong data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					plong	l	Value to be assigned.

	Returns:		plong		The value of l.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong pvar_set_long( pvar* var, plong l )
{
	PROC( "pvar_set_long" );
	PARMS( "var", "%p", var );
	PARMS( "l", "%ld", l );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_LONG;
	var->val.l = l;

	RETURN( var->val.l );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_ulong()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pulong data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pulong	ul	Value to be assigned.

	Returns:		pulong		The value of ul.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong pvar_set_ulong( pvar* var, pulong ul )
{
	PROC( "pvar_set_ulong" );
	PARMS( "var", "%p", var );
	PARMS( "ul", "%ld", ul );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_ULONG;
	var->val.ul = ul;

	RETURN( var->val.ul );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_float()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pfloat data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pfloat	f	Value to be assigned.

	Returns:		pfloat		The value of f.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat pvar_set_float( pvar* var, pfloat f )
{
	PROC( "pvar_set_float" );
	PARMS( "var", "%p", var );
	PARMS( "f", "%f", f );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_FLOAT;
	var->val.f = f;

	RETURN( var->val.f );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_double()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pdouble data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pdouble	d	Value to be assigned.

	Returns:		pdouble		The value of d.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble pvar_set_double( pvar* var, pdouble d )
{
	PROC( "pvar_set_double" );
	PARMS( "var", "%p", var );
	PARMS( "d", "%lf", d );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_DOUBLE;
	var->val.d = d;

	RETURN( var->val.d );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_cstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's uchar* data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					uchar*	s	Value to be assigned.

	Returns:		uchar*		The value of s.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pvar_set_cstring( pvar* var, uchar* s )
{
	PROC( "pvar_set_cstring" );
	PARMS( "var", "%p", var );
	PARMS( "s", "%s", s );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_STRING;
	var->val.s = s;
	pvar_set_constant( var );

	RETURN( var->val.s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_string()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's uchar* data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					uchar*	s	Value to be assigned.

	Returns:		uchar*		The value of s.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pvar_set_string( pvar* var, uchar* s )
{
	PROC( "pvar_set_string" );
	PARMS( "var", "%p", var );
	PARMS( "s", "%s", s );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_STRING;
	var->val.s = s;

	RETURN( var->val.s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_wcstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pchar* data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pchar*	ws	Value to be assigned.

	Returns:		pchar*		The value of ws.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pvar_set_wcstring( pvar* var, pchar* ws )
{
	PROC( "pvar_set_wcstring" );
	PARMS( "var", "%p", var );
	PARMS( "ws", "%ls", ws );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_WSTRING;
	var->val.ws = ws;
	pvar_set_constant( var );

	RETURN( var->val.ws );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pvar_set_wstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pchar* data value and type.

	Parameters:		pvar*	var	Pointer to pvar structure.
					pchar*	ws	Value to be assigned.

	Returns:		pchar*		The value of ws.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* pvar_set_wstring( pvar* var, pchar* ws )
{
	PROC( "pvar_set_wstring" );
	PARMS( "var", "%p", var );
	PARMS( "ws", "%ls", ws );

	pvar_reset( var );
 var->type &= ~0x0F;
	var->type |= PVAR_WSTRING;
	var->val.ws = ws;

	RETURN( var->val.ws );
}

