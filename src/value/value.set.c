/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_CHAR;
	val->val.c = c;

	RETURN( val->val.c );
}

/** Sets the int data value and type.
//val// is the pgvalue-object to be set.

//i// is the int value to be assigned to //val//.

The function always returns the value //i//.
*/
int pgvalue_set_int( pgvalue* val, int i )
{
	PROC( "pgvalue_set_int" );
	PARMS( "val", "%p", val );
	PARMS( "i", "%d", i );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_INT;
	val->val.i = i;

	RETURN( val->val.i );
}

/** Sets the long data value and type.
//val// is the pgvalue-object to be set.

//l// is the long value to be assigned to //val//.

The function always returns the value //l//.
*/
long pgvalue_set_long( pgvalue* val, long l )
{
	PROC( "pgvalue_set_long" );
	PARMS( "val", "%p", val );
	PARMS( "l", "%ld", l );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_LONG;
	val->val.l = l;

	RETURN( val->val.l );
}

/** Sets the ulong data value and type.
//val// is the pgvalue-object to be set.

//ul// is the ulong value to be assigned to //val//.

The function always returns the value //ul//.
*/
ulong pgvalue_set_ulong( pgvalue* val, ulong ul )
{
	PROC( "pgvalue_set_ulong" );
	PARMS( "val", "%p", val );
	PARMS( "ul", "%ld", ul );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_ULONG;
	val->val.ul = ul;

	RETURN( val->val.ul );
}

/** Sets the float data value and type.
//val// is the pgvalue-object to be set.

//f// is the float value to be assigned to //val//.

The function always returns the value //f//.
*/
float pgvalue_set_float( pgvalue* val, float f )
{
	PROC( "pgvalue_set_float" );
	PARMS( "val", "%p", val );
	PARMS( "f", "%f", f );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_FLOAT;
	val->val.f = f;

	RETURN( val->val.f );
}

/** Sets the double data value and type.
//val// is the pgvalue-object to be set.

//d// is the double value to be assigned to //val//.

The function always returns the value //d//.
*/
double pgvalue_set_double( pgvalue* val, double d )
{
	PROC( "pgvalue_set_double" );
	PARMS( "val", "%p", val );
	PARMS( "d", "%lf", d );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_DOUBLE;
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_STRING;
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_STRING;
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pchar*)NULL );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_WSTRING;
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

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pchar*)NULL );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_WSTRING;
	val->val.ws = ws;

	RETURN( val->val.ws );
}

/** Sets the void* data value and type.
//val// is the pgvalue-object to be set.

//ptr// is the void* value to be assigned to //val//.

The function always returns the value //ptr//.
*/
void* pgvalue_set_ptr( pgvalue* val, void* ptr )
{
	PROC( "pgvalue_set_ptr" );
	PARMS( "val", "%p", val );
	PARMS( "ptr", "%p", ptr );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	pgvalue_reset( val );
	val->type = PGVALUE_PTR;
	val->val.ptr = ptr;

	RETURN( val->val.ptr );
}

