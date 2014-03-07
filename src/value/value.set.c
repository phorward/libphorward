/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Sets the pbyte data value and type.
//val// is the pgvalue-object to be set.

//b// is the pbyte value to be assigned to //val//.

The function always returns the value //b//.
*/
pbyte pgvalue_set_byte( pgvalue* val, pbyte b )
{
	PROC( "pgvalue_set_byte" );
	PARMS( "val", "%p", val );
	PARMS( "b", "%d", b );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_BYTE;
	val->val.b = b;

	RETURN( val->val.b );
}

/** Sets the char data value and type.
//val// is the pgvalue-object to be set.

//c// is the char value to be assigned to //val//.

The function always returns the value //c//.
*/
char pgvalue_set_char( pgvalue* val, char c )
{
	PROC( "pgvalue_set_char" );
	PARMS( "val", "%p", val );
	PARMS( "c", "%c", c );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_CHAR;
	val->val.c = c;

	RETURN( val->val.c );
}

/** Sets the pint data value and type.
//val// is the pgvalue-object to be set.

//i// is the pint value to be assigned to //val//.

The function always returns the value //i//.
*/
pint pgvalue_set_int( pgvalue* val, pint i )
{
	PROC( "pgvalue_set_int" );
	PARMS( "val", "%p", val );
	PARMS( "i", "%d", i );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_INT;
	val->val.i = i;

	RETURN( val->val.i );
}

/** Sets the plong data value and type.
//val// is the pgvalue-object to be set.

//l// is the plong value to be assigned to //val//.

The function always returns the value //l//.
*/
plong pgvalue_set_long( pgvalue* val, plong l )
{
	PROC( "pgvalue_set_long" );
	PARMS( "val", "%p", val );
	PARMS( "l", "%ld", l );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_LONG;
	val->val.l = l;

	RETURN( val->val.l );
}

/** Sets the pulong data value and type.
//val// is the pgvalue-object to be set.

//ul// is the pulong value to be assigned to //val//.

The function always returns the value //ul//.
*/
pulong pgvalue_set_ulong( pgvalue* val, pulong ul )
{
	PROC( "pgvalue_set_ulong" );
	PARMS( "val", "%p", val );
	PARMS( "ul", "%ld", ul );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_ULONG;
	val->val.ul = ul;

	RETURN( val->val.ul );
}

/** Sets the pfloat data value and type.
//val// is the pgvalue-object to be set.

//f// is the pfloat value to be assigned to //val//.

The function always returns the value //f//.
*/
pfloat pgvalue_set_float( pgvalue* val, pfloat f )
{
	PROC( "pgvalue_set_float" );
	PARMS( "val", "%p", val );
	PARMS( "f", "%f", f );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_FLOAT;
	val->val.f = f;

	RETURN( val->val.f );
}

/** Sets the pdouble data value and type.
//val// is the pgvalue-object to be set.

//d// is the pdouble value to be assigned to //val//.

The function always returns the value //d//.
*/
pdouble pgvalue_set_double( pgvalue* val, pdouble d )
{
	PROC( "pgvalue_set_double" );
	PARMS( "val", "%p", val );
	PARMS( "d", "%lf", d );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_DOUBLE;
	val->val.d = d;

	RETURN( val->val.d );
}

/** Sets the char* data value and type.
//val// is the pgvalue-object to be set.

//s// is the char* value to be assigned to //val//.

The function always returns the value //s//.
*/
char* pgvalue_set_cstring( pgvalue* val, char* s )
{
	PROC( "pgvalue_set_cstring" );
	PARMS( "val", "%p", val );
	PARMS( "s", "%s", s );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_STRING;
	val->val.s = s;
	pgvalue_set_constant( val );

	RETURN( val->val.s );
}

/** Sets the char* data value and type.
//val// is the pgvalue-object to be set.

//s// is the char* value to be assigned to //val//.

The function always returns the value //s//.
*/
char* pgvalue_set_string( pgvalue* val, char* s )
{
	PROC( "pgvalue_set_string" );
	PARMS( "val", "%p", val );
	PARMS( "s", "%s", s );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_STRING;
	val->val.s = s;

	RETURN( val->val.s );
}

/** Sets the pchar* data value and type.
//val// is the pgvalue-object to be set.

//ws// is the pchar* value to be assigned to //val//.

The function always returns the value //ws//.
*/
pchar* pgvalue_set_wcstring( pgvalue* val, pchar* ws )
{
	PROC( "pgvalue_set_wcstring" );
	PARMS( "val", "%p", val );
	PARMS( "ws", "%ls", ws );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_WSTRING;
	val->val.ws = ws;
	pgvalue_set_constant( val );

	RETURN( val->val.ws );
}

/** Sets the pchar* data value and type.
//val// is the pgvalue-object to be set.

//ws// is the pchar* value to be assigned to //val//.

The function always returns the value //ws//.
*/
pchar* pgvalue_set_wstring( pgvalue* val, pchar* ws )
{
	PROC( "pgvalue_set_wstring" );
	PARMS( "val", "%p", val );
	PARMS( "ws", "%ls", ws );

	pgvalue_reset( val );
	val->type &= ~0x0F;
	val->type |= PUNION_WSTRING;
	val->val.ws = ws;

	RETURN( val->val.ws );
}

