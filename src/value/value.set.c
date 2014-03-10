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
char pg_value_set_char( pgvalue* val, char c )
{
	PROC( "pg_value_set_char" );
	PARMS( "val", "%p", val );
	PARMS( "c", "%c", c );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_CHAR;
	val->val.c = c;

	RETURN( val->val.c );
}

/** Sets the int data value and type.
//val// is the pgvalue-object to be set.

//i// is the int value to be assigned to //val//.

The function always returns the value //i//.
*/
int pg_value_set_int( pgvalue* val, int i )
{
	PROC( "pg_value_set_int" );
	PARMS( "val", "%p", val );
	PARMS( "i", "%d", i );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_INT;
	val->val.i = i;

	RETURN( val->val.i );
}

/** Sets the long data value and type.
//val// is the pgvalue-object to be set.

//l// is the long value to be assigned to //val//.

The function always returns the value //l//.
*/
long pg_value_set_long( pgvalue* val, long l )
{
	PROC( "pg_value_set_long" );
	PARMS( "val", "%p", val );
	PARMS( "l", "%ld", l );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_LONG;
	val->val.l = l;

	RETURN( val->val.l );
}

/** Sets the ulong data value and type.
//val// is the pgvalue-object to be set.

//ul// is the ulong value to be assigned to //val//.

The function always returns the value //ul//.
*/
ulong pg_value_set_ulong( pgvalue* val, ulong ul )
{
	PROC( "pg_value_set_ulong" );
	PARMS( "val", "%p", val );
	PARMS( "ul", "%ld", ul );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_ULONG;
	val->val.ul = ul;

	RETURN( val->val.ul );
}

/** Sets the float data value and type.
//val// is the pgvalue-object to be set.

//f// is the float value to be assigned to //val//.

The function always returns the value //f//.
*/
float pg_value_set_float( pgvalue* val, float f )
{
	PROC( "pg_value_set_float" );
	PARMS( "val", "%p", val );
	PARMS( "f", "%f", f );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_FLOAT;
	val->val.f = f;

	RETURN( val->val.f );
}

/** Sets the double data value and type.
//val// is the pgvalue-object to be set.

//d// is the double value to be assigned to //val//.

The function always returns the value //d//.
*/
double pg_value_set_double( pgvalue* val, double d )
{
	PROC( "pg_value_set_double" );
	PARMS( "val", "%p", val );
	PARMS( "d", "%lf", d );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_DOUBLE;
	val->val.d = d;

	RETURN( val->val.d );
}

/** Sets the char* data value and type.
//val// is the pgvalue-object to be set.

//s// is the char* value to be assigned to //val//.

The function always returns the value //s//.
*/
char* pg_value_set_cstring( pgvalue* val, char* s )
{
	PROC( "pg_value_set_cstring" );
	PARMS( "val", "%p", val );
	PARMS( "s", "%s", s );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_STRING;
	val->val.s = s;
	pg_value_set_constant( val, TRUE );

	RETURN( val->val.s );
}

/** Sets the char* data value and type.
//val// is the pgvalue-object to be set.

//s// is the char* value to be assigned to //val//.

The function always returns the value //s//.
*/
char* pg_value_set_string( pgvalue* val, char* s )
{
	PROC( "pg_value_set_string" );
	PARMS( "val", "%p", val );
	PARMS( "s", "%s", s );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_STRING;
	val->val.s = s;

	RETURN( val->val.s );
}

/** Sets the wchar_t* data value and type.
//val// is the pgvalue-object to be set.

//ws// is the wchar_t* value to be assigned to //val//.

The function always returns the value //ws//.
*/
wchar_t* pg_value_set_wcstring( pgvalue* val, wchar_t* ws )
{
	PROC( "pg_value_set_wcstring" );
	PARMS( "val", "%p", val );
	PARMS( "ws", "%ls", ws );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_WSTRING;
	val->val.ws = ws;
	pg_value_set_constant( val, TRUE );

	RETURN( val->val.ws );
}

/** Sets the wchar_t* data value and type.
//val// is the pgvalue-object to be set.

//ws// is the wchar_t* value to be assigned to //val//.

The function always returns the value //ws//.
*/
wchar_t* pg_value_set_wstring( pgvalue* val, wchar_t* ws )
{
	PROC( "pg_value_set_wstring" );
	PARMS( "val", "%p", val );
	PARMS( "ws", "%ls", ws );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_WSTRING;
	val->val.ws = ws;

	RETURN( val->val.ws );
}

/** Sets the void* data value and type.
//val// is the pgvalue-object to be set.

//ptr// is the void* value to be assigned to //val//.

The function always returns the value //ptr//.
*/
void* pg_value_set_ptr( pgvalue* val, void* ptr )
{
	PROC( "pg_value_set_ptr" );
	PARMS( "val", "%p", val );
	PARMS( "ptr", "%p", ptr );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	pg_value_reset( val );
	val->type = PGVALUETYPE_PTR;
	val->val.ptr = ptr;

	RETURN( val->val.ptr );
}

