/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include <phorward.h>

/** Sets a variants pbyte data value and type.

//var// is the pointer to the punion structure.

//b// is the pbyte value to be assigned to //var//.

The function always returns the value //b//.
*/
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

/** Sets a variants char data value and type.

//var// is the pointer to the punion structure.

//c// is the char value to be assigned to //var//.

The function always returns the value //c//.
*/
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

/** Sets a variants pint data value and type.

//var// is the pointer to the punion structure.

//i// is the pint value to be assigned to //var//.

The function always returns the value //i//.
*/
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

/** Sets a variants plong data value and type.

//var// is the pointer to the punion structure.

//l// is the plong value to be assigned to //var//.

The function always returns the value //l//.
*/
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

/** Sets a variants pulong data value and type.

//var// is the pointer to the punion structure.

//ul// is the pulong value to be assigned to //var//.

The function always returns the value //ul//.
*/
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

/** Sets a variants pfloat data value and type.

//var// is the pointer to the punion structure.

//f// is the pfloat value to be assigned to //var//.

The function always returns the value //f//.
*/
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

/** Sets a variants pdouble data value and type.

//var// is the pointer to the punion structure.

//d// is the pdouble value to be assigned to //var//.

The function always returns the value //d//.
*/
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

/** Sets a variants char* data value and type.

//var// is the pointer to the punion structure.

//s// is the char* value to be assigned to //var//.

The function always returns the value //s//.
*/
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

/** Sets a variants char* data value and type.

//var// is the pointer to the punion structure.

//s// is the char* value to be assigned to //var//.

The function always returns the value //s//.
*/
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

/** Sets a variants pchar* data value and type.

//var// is the pointer to the punion structure.

//ws// is the pchar* value to be assigned to //var//.

The function always returns the value //ws//.
*/
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

/** Sets a variants pchar* data value and type.

//var// is the pointer to the punion structure.

//ws// is the pchar* value to be assigned to //var//.

The function always returns the value //ws//.
*/
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

