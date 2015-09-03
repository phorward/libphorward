/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Sets the char data value and type.
//val// is the pany-object to be set.

//c// is the char value to be assigned to //val//.

The function always returns the value //c//.
*/
char pany_set_char( pany* val, char c )
{
	PROC( "pany_set_char" );
	PARMS( "val", "%p", val );
	PARMS( "c", "%c", c );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_CHAR;
	val->val.c = c;

	RETURN( val->val.c );
}

/** Sets the int data value and type.
//val// is the pany-object to be set.

//i// is the int value to be assigned to //val//.

The function always returns the value //i//.
*/
int pany_set_int( pany* val, int i )
{
	PROC( "pany_set_int" );
	PARMS( "val", "%p", val );
	PARMS( "i", "%d", i );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_INT;
	val->val.i = i;

	RETURN( val->val.i );
}

/** Sets the long data value and type.
//val// is the pany-object to be set.

//l// is the long value to be assigned to //val//.

The function always returns the value //l//.
*/
long pany_set_long( pany* val, long l )
{
	PROC( "pany_set_long" );
	PARMS( "val", "%p", val );
	PARMS( "l", "%ld", l );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_LONG;
	val->val.l = l;

	RETURN( val->val.l );
}

/** Sets the ulong data value and type.
//val// is the pany-object to be set.

//ul// is the ulong value to be assigned to //val//.

The function always returns the value //ul//.
*/
ulong pany_set_ulong( pany* val, ulong ul )
{
	PROC( "pany_set_ulong" );
	PARMS( "val", "%p", val );
	PARMS( "ul", "%ld", ul );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_ULONG;
	val->val.ul = ul;

	RETURN( val->val.ul );
}

/** Sets the float data value and type.
//val// is the pany-object to be set.

//f// is the float value to be assigned to //val//.

The function always returns the value //f//.
*/
float pany_set_float( pany* val, float f )
{
	PROC( "pany_set_float" );
	PARMS( "val", "%p", val );
	PARMS( "f", "%f", f );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_FLOAT;
	val->val.f = f;

	RETURN( val->val.f );
}

/** Sets the double data value and type.
//val// is the pany-object to be set.

//d// is the double value to be assigned to //val//.

The function always returns the value //d//.
*/
double pany_set_double( pany* val, double d )
{
	PROC( "pany_set_double" );
	PARMS( "val", "%p", val );
	PARMS( "d", "%lf", d );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_DOUBLE;
	val->val.d = d;

	RETURN( val->val.d );
}

/** Sets the char* data value and type.
//val// is the pany-object to be set.

//s// is the char* value to be assigned to //val//.

The function always returns the value //s//.
*/
char* pany_set_cstr( pany* val, char* s )
{
	PROC( "pany_set_cstr" );
	PARMS( "val", "%p", val );
	PARMS( "s", "%s", s );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	pany_reset( val );
	val->type = PANYTYPE_STR;
	val->val.s = s;
	pany_set_constant( val, TRUE );

	RETURN( val->val.s );
}

/** Sets the char* data value and type.
//val// is the pany-object to be set.

//s// is the char* value to be assigned to //val//.

The function always returns the value //s//.
*/
char* pany_set_str( pany* val, char* s )
{
	PROC( "pany_set_str" );
	PARMS( "val", "%p", val );
	PARMS( "s", "%s", s );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	pany_reset( val );
	val->type = PANYTYPE_STR;
	val->val.s = s;

	RETURN( val->val.s );
}

/** Sets the wchar_t* data value and type.
//val// is the pany-object to be set.

//ws// is the wchar_t* value to be assigned to //val//.

The function always returns the value //ws//.
*/
wchar_t* pany_set_cwcs( pany* val, wchar_t* ws )
{
	PROC( "pany_set_cwcs" );
	PARMS( "val", "%p", val );
	PARMS( "ws", "%ls", ws );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	pany_reset( val );
	val->type = PANYTYPE_WCS;
	val->val.ws = ws;
	pany_set_constant( val, TRUE );

	RETURN( val->val.ws );
}

/** Sets the wchar_t* data value and type.
//val// is the pany-object to be set.

//ws// is the wchar_t* value to be assigned to //val//.

The function always returns the value //ws//.
*/
wchar_t* pany_set_wcs( pany* val, wchar_t* ws )
{
	PROC( "pany_set_wcs" );
	PARMS( "val", "%p", val );
	PARMS( "ws", "%ls", ws );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	pany_reset( val );
	val->type = PANYTYPE_WCS;
	val->val.ws = ws;

	RETURN( val->val.ws );
}

/** Sets the void* data value and type.
//val// is the pany-object to be set.

//ptr// is the void* value to be assigned to //val//.

The function always returns the value //ptr//.
*/
void* pany_set_ptr( pany* val, void* ptr )
{
	PROC( "pany_set_ptr" );
	PARMS( "val", "%p", val );
	PARMS( "ptr", "%p", ptr );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	pany_reset( val );
	val->type = PANYTYPE_PTR;
	val->val.ptr = ptr;

	RETURN( val->val.ptr );
}
