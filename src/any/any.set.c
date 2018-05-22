/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Sets the ``pboolean``-value and type of //val//.

//val// is the pany-object to be set.
//b// is the ``pboolean``-value to be assigned to //val//.

The function always returns the value //b//.
*/
pboolean pany_set_bool( pany* val, pboolean b )
{
	PROC( "pany_set_bool" );
	PARMS( "val", "%p", val );
	PARMS( "b", "%d", b );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (pboolean)FALSE );
	}

	pany_reset( val );
	val->type = PANYTYPE_BOOL;
	val->val.b = b;

	RETURN( val->val.b );
}

/** Sets the ``char``-value and type of //val//.

//val// is the pany-object to be set.
//c// is the ``char``-value to be assigned to //val//.

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

/** Sets the ``int``-value and type of //val//.

//val// is the pany-object to be set.
//i// is the ``int``-value to be assigned to //val//.

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

/** Sets the ``long``-value and type of //val//.

//val// is the pany-object to be set.
//l// is the ``long``-value to be assigned to //val//.

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

/** Sets the ``unsigned long``-value and type of //val//.

//val// is the pany-object to be set.
//ul// is the ``unsigned long``-value to be assigned to //val//.

The function always returns the value //ul//.
*/
unsigned long pany_set_ulong( pany* val, unsigned long ul )
{
	PROC( "pany_set_ulong" );
	PARMS( "val", "%p", val );
	PARMS( "ul", "%ld", ul );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (unsigned long)0 );
	}

	pany_reset( val );
	val->type = PANYTYPE_ULONG;
	val->val.ul = ul;

	RETURN( val->val.ul );
}

/** Sets the ``float``-value and type of //val//.

//val// is the pany-object to be set.
//f// is the ``float``-value to be assigned to //val//.

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

/** Sets the ``double``-value and type of //val//.

//val// is the pany-object to be set.
//d// is the ``double``-value to be assigned to //val//.

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

/** Sets the ``char*``-value and type of //val//.

//val// is the pany-object to be set.
//s// is the ``char*``-value to be assigned to //val//.

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
	val->type = PANYTYPE_CSTR;
	val->val.s = s;

	RETURN( val->val.s );
}

/** Sets the ``char*``-value and type of //val//.

//val// is the pany-object to be set.
//s// is the ``char*``-value to be assigned to //val//.

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

/** Sets the ``wchar_t*``-value and type of //val//.

//val// is the pany-object to be set.
//ws// is the ``wchar_t*``-value to be assigned to //val//.

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
	val->type = PANYTYPE_CWCS;
	val->val.ws = ws;

	RETURN( val->val.ws );
}

/** Sets the ``wchar_t*``-value and type of //val//.

//val// is the pany-object to be set.
//ws// is the ``wchar_t*``-value to be assigned to //val//.

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

/** Sets the ``void*``-value and type of //val//.

//val// is the pany-object to be set.
//ptr// is the ``void*``-value to be assigned to //val//.

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

