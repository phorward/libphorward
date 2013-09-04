/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include <phorward.h>

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_byte()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pbyte data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pbyte	b	Value to be assigned.

	Returns:		pbyte		The value of b.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pbyte punion_set_byte( punion* var, pbyte b )
{
	PROC( "punion_set_byte" );
	PARMS( "var", "%p", var );
	PARMS( "b", "%d", b );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_BYTE;
	var->val.b = b;

	RETURN( var->val.b );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_char()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's char data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					char	c	Value to be assigned.

	Returns:		char		The value of c.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char punion_set_char( punion* var, char c )
{
	PROC( "punion_set_char" );
	PARMS( "var", "%p", var );
	PARMS( "c", "%c", c );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_CHAR;
	var->val.c = c;

	RETURN( var->val.c );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_int()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pint data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pint	i	Value to be assigned.

	Returns:		pint		The value of i.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint punion_set_int( punion* var, pint i )
{
	PROC( "punion_set_int" );
	PARMS( "var", "%p", var );
	PARMS( "i", "%d", i );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_INT;
	var->val.i = i;

	RETURN( var->val.i );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_long()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's plong data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					plong	l	Value to be assigned.

	Returns:		plong		The value of l.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
plong punion_set_long( punion* var, plong l )
{
	PROC( "punion_set_long" );
	PARMS( "var", "%p", var );
	PARMS( "l", "%ld", l );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_LONG;
	var->val.l = l;

	RETURN( var->val.l );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_ulong()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pulong data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pulong	ul	Value to be assigned.

	Returns:		pulong		The value of ul.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pulong punion_set_ulong( punion* var, pulong ul )
{
	PROC( "punion_set_ulong" );
	PARMS( "var", "%p", var );
	PARMS( "ul", "%ld", ul );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_ULONG;
	var->val.ul = ul;

	RETURN( var->val.ul );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_float()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pfloat data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pfloat	f	Value to be assigned.

	Returns:		pfloat		The value of f.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pfloat punion_set_float( punion* var, pfloat f )
{
	PROC( "punion_set_float" );
	PARMS( "var", "%p", var );
	PARMS( "f", "%f", f );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_FLOAT;
	var->val.f = f;

	RETURN( var->val.f );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_double()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pdouble data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pdouble	d	Value to be assigned.

	Returns:		pdouble		The value of d.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pdouble punion_set_double( punion* var, pdouble d )
{
	PROC( "punion_set_double" );
	PARMS( "var", "%p", var );
	PARMS( "d", "%lf", d );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_DOUBLE;
	var->val.d = d;

	RETURN( var->val.d );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_cstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's char* data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					char*	s	Value to be assigned.

	Returns:		char*		The value of s.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* punion_set_cstring( punion* var, char* s )
{
	PROC( "punion_set_cstring" );
	PARMS( "var", "%p", var );
	PARMS( "s", "%s", s );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_STRING;
	var->val.s = s;
	punion_set_constant( var );

	RETURN( var->val.s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_string()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's char* data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					char*	s	Value to be assigned.

	Returns:		char*		The value of s.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
char* punion_set_string( punion* var, char* s )
{
	PROC( "punion_set_string" );
	PARMS( "var", "%p", var );
	PARMS( "s", "%s", s );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_STRING;
	var->val.s = s;

	RETURN( var->val.s );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_wcstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pchar* data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pchar*	ws	Value to be assigned.

	Returns:		pchar*		The value of ws.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* punion_set_wcstring( punion* var, pchar* ws )
{
	PROC( "punion_set_wcstring" );
	PARMS( "var", "%p", var );
	PARMS( "ws", "%ls", ws );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_WSTRING;
	var->val.ws = ws;
	punion_set_constant( var );

	RETURN( var->val.ws );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		punion_set_wstring()

	Author:			Jan Max Meyer (generated with var.gen.awk)

	Usage:			Sets a variant's pchar* data value and type.

	Parameters:		punion*	var	Pointer to punion structure.
					pchar*	ws	Value to be assigned.

	Returns:		pchar*		The value of ws.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* punion_set_wstring( punion* var, pchar* ws )
{
	PROC( "punion_set_wstring" );
	PARMS( "var", "%p", var );
	PARMS( "ws", "%ls", ws );

	punion_reset( var );
	var->type &= ~0x0F;
	var->type |= PUNION_WSTRING;
	var->val.ws = ws;

	RETURN( var->val.ws );
}

