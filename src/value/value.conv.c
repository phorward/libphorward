/*
 * WARNING:
 * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!
 */

#include "phorward.h"

/** Converts the current value of //val// into a char value.

//val// is the pvalue-object to convert from.

The function returns the char-value of //val//.
*/
char pvalue_to_char( pvalue* val )
{
	PROC( "pvalue_to_char" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char)0 );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( val->val.c );
		case PVALUETYPE_INT:
			RETURN( (char)val->val.i );
		case PVALUETYPE_LONG:
			RETURN( (char)val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( (char)val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( (char)val->val.f );
		case PVALUETYPE_DOUBLE:
			RETURN( (char)val->val.d );
		case PVALUETYPE_STR:
			RETURN( (char)strtol( val->val.s, (char**)NULL, 0 ) );
		case PVALUETYPE_WCS:
			RETURN( (char)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PVALUETYPE_PTR:
			RETURN( (char)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char)0 );
}

/** Converts the current value of //val// into a int value.

//val// is the pvalue-object to convert from.

The function returns the int-value of //val//.
*/
int pvalue_to_int( pvalue* val )
{
	PROC( "pvalue_to_int" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (int)0 );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (int)val->val.c );
		case PVALUETYPE_INT:
			RETURN( val->val.i );
		case PVALUETYPE_LONG:
			RETURN( (int)val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( (int)val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( (int)val->val.f );
		case PVALUETYPE_DOUBLE:
			RETURN( (int)val->val.d );
		case PVALUETYPE_STR:
			RETURN( (int)strtol( val->val.s, (char**)NULL, 0 ) );
		case PVALUETYPE_WCS:
			RETURN( (int)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PVALUETYPE_PTR:
			RETURN( (int)0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (int)0 );
}

/** Converts the current value of //val// into a long value.

//val// is the pvalue-object to convert from.

The function returns the long-value of //val//.
*/
long pvalue_to_long( pvalue* val )
{
	PROC( "pvalue_to_long" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (long)0 );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (long)val->val.c );
		case PVALUETYPE_INT:
			RETURN( (long)val->val.i );
		case PVALUETYPE_LONG:
			RETURN( val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( (long)val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( (long)val->val.f );
		case PVALUETYPE_DOUBLE:
			RETURN( (long)val->val.d );
		case PVALUETYPE_STR:
			RETURN( (long)strtol( val->val.s, (char**)NULL, 0 ) );
		case PVALUETYPE_WCS:
			RETURN( (long)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PVALUETYPE_PTR:
			RETURN( (long)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (long)0 );
}

/** Converts the current value of //val// into a ulong value.

//val// is the pvalue-object to convert from.

The function returns the ulong-value of //val//.
*/
ulong pvalue_to_ulong( pvalue* val )
{
	PROC( "pvalue_to_ulong" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (ulong)0 );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (ulong)val->val.c );
		case PVALUETYPE_INT:
			RETURN( (ulong)val->val.i );
		case PVALUETYPE_LONG:
			RETURN( (ulong)val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( (ulong)val->val.f );
		case PVALUETYPE_DOUBLE:
			RETURN( (ulong)val->val.d );
		case PVALUETYPE_STR:
			RETURN( (ulong)strtol( val->val.s, (char**)NULL, 0 ) );
		case PVALUETYPE_WCS:
			RETURN( (ulong)wcstol( val->val.ws, (wchar_t**)NULL, 0 ) );
		case PVALUETYPE_PTR:
			RETURN( (ulong)val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (ulong)0 );
}

/** Converts the current value of //val// into a float value.

//val// is the pvalue-object to convert from.

The function returns the float-value of //val//.
*/
float pvalue_to_float( pvalue* val )
{
	PROC( "pvalue_to_float" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (float)0.0 );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (float)val->val.c );
		case PVALUETYPE_INT:
			RETURN( (float)val->val.i );
		case PVALUETYPE_LONG:
			RETURN( (float)val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( (float)val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( val->val.f );
		case PVALUETYPE_DOUBLE:
			RETURN( (float)val->val.d );
		case PVALUETYPE_STR:
			RETURN( (float)strtod( val->val.s, (char**)NULL ) );
		case PVALUETYPE_WCS:
			RETURN( (float)wcstod( val->val.ws, (wchar_t**)NULL ) );
		case PVALUETYPE_PTR:
			RETURN( (float)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (float)0.0 );
}

/** Converts the current value of //val// into a double value.

//val// is the pvalue-object to convert from.

The function returns the double-value of //val//.
*/
double pvalue_to_double( pvalue* val )
{
	PROC( "pvalue_to_double" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (double)0.0 );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (double)val->val.c );
		case PVALUETYPE_INT:
			RETURN( (double)val->val.i );
		case PVALUETYPE_LONG:
			RETURN( (double)val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( (double)val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( (double)val->val.f );
		case PVALUETYPE_DOUBLE:
			RETURN( val->val.d );
		case PVALUETYPE_STR:
			RETURN( (double)strtod( val->val.s, (char**)NULL ) );
		case PVALUETYPE_WCS:
			RETURN( (double)wcstod( val->val.ws, (wchar_t**)NULL ) );
		case PVALUETYPE_PTR:
			RETURN( (double)0.0 );
	}

	MSG( "Can't convert this type!" );
	RETURN( (double)0.0 );
}

/** Converts the current value of //val// into a char* value.

//val// is the pvalue-object to convert from.

The function returns the char*-value of //val//.
*/
char* pvalue_to_str( pvalue* val )
{
	PROC( "pvalue_to_str" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (char*)pasprintf( "%d", val->val.c ) );
		case PVALUETYPE_INT:
			RETURN( (char*)pasprintf( "%d", val->val.i ) );
		case PVALUETYPE_LONG:
			RETURN( (char*)pasprintf( "%ld", val->val.l ) );
		case PVALUETYPE_ULONG:
			RETURN( (char*)pasprintf( "%ld", val->val.ul ) );
		case PVALUETYPE_FLOAT:
			RETURN( (char*)pdbl_to_str( (double)val->val.f ) );
		case PVALUETYPE_DOUBLE:
			RETURN( (char*)pdbl_to_str( val->val.d ) );
		case PVALUETYPE_STR:
			RETURN( val->val.s );
		case PVALUETYPE_WCS:
			RETURN( (char*)pwcs_to_str( val->val.ws, FALSE ) );
		case PVALUETYPE_PTR:
			RETURN( (char*)pasprintf( "%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (char*)NULL );
}

/** Converts the current value of //val// into a wchar_t* value.

//val// is the pvalue-object to convert from.

The function returns the wchar_t*-value of //val//.
*/
wchar_t* pvalue_to_wcs( pvalue* val )
{
	PROC( "pvalue_to_wcs" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (wchar_t*)NULL );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (wchar_t*)pawcsprintf( L"%d", val->val.c ) );
		case PVALUETYPE_INT:
			RETURN( (wchar_t*)pawcsprintf( L"%d", val->val.i ) );
		case PVALUETYPE_LONG:
			RETURN( (wchar_t*)pawcsprintf( L"%ld", val->val.l ) );
		case PVALUETYPE_ULONG:
			RETURN( (wchar_t*)pawcsprintf( L"%ld", val->val.ul ) );
		case PVALUETYPE_FLOAT:
			RETURN( (wchar_t*)pdbl_to_wcs( (double)val->val.f ) );
		case PVALUETYPE_DOUBLE:
			RETURN( (wchar_t*)pdbl_to_wcs( val->val.d ) );
		case PVALUETYPE_STR:
			RETURN( (wchar_t*)pstr_to_wcs( val->val.s, FALSE ) );
		case PVALUETYPE_WCS:
			RETURN( val->val.ws );
		case PVALUETYPE_PTR:
			RETURN( (wchar_t*)pawcsprintf( L"%p", val->val.ptr ) );
	}

	MSG( "Can't convert this type!" );
	RETURN( (wchar_t*)NULL );
}

/** Converts the current value of //val// into a void* value.

//val// is the pvalue-object to convert from.

The function returns the void*-value of //val//.
*/
void* pvalue_to_ptr( pvalue* val )
{
	PROC( "pvalue_to_ptr" );
	PARMS( "val", "%p", val );

	if( !val )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	switch( val->type )
	{
		case PVALUETYPE_CHAR:
			RETURN( (void*)NULL );
		case PVALUETYPE_INT:
			RETURN( (void*)NULL );
		case PVALUETYPE_LONG:
			RETURN( (void*)val->val.l );
		case PVALUETYPE_ULONG:
			RETURN( (void*)val->val.ul );
		case PVALUETYPE_FLOAT:
			RETURN( (void*)NULL );
		case PVALUETYPE_DOUBLE:
			RETURN( (void*)NULL );
		case PVALUETYPE_STR:
			RETURN( (void*)val->val.s );
		case PVALUETYPE_WCS:
			RETURN( (void*)val->val.ws );
		case PVALUETYPE_PTR:
			RETURN( val->val.ptr );
	}

	MSG( "Can't convert this type!" );
	RETURN( (void*)NULL );
}

/** Converts a pvalue-structure to any supported type.

//val// is the pvalue-object to be converted.
//type// is the type define to which //val// should be converted to.

The function returns TRUE on success, FALSE else.
*/
pboolean pvalue_convert( pvalue* val, pvaluetype type )
{
	PROC( "pvalue_convert" );
	PARMS( "val", "%p", val );
	PARMS( "type", "%d", type );

	if( !val )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( val->type == type )
		RETURN( TRUE );

	switch( type )
	{
		case PVALUETYPE_CHAR:
			pvalue_set_char( val, pvalue_to_char( val ) );
			RETURN( TRUE );

		case PVALUETYPE_INT:
			pvalue_set_int( val, pvalue_to_int( val ) );
			RETURN( TRUE );

		case PVALUETYPE_LONG:
			pvalue_set_long( val, pvalue_to_long( val ) );
			RETURN( TRUE );

		case PVALUETYPE_ULONG:
			pvalue_set_ulong( val, pvalue_to_ulong( val ) );
			RETURN( TRUE );

		case PVALUETYPE_FLOAT:
			pvalue_set_float( val, pvalue_to_float( val ) );
			RETURN( TRUE );

		case PVALUETYPE_DOUBLE:
			pvalue_set_double( val, pvalue_to_double( val ) );
			RETURN( TRUE );

		case PVALUETYPE_STR:
			pvalue_set_str( val, pvalue_to_str( val ) );
			RETURN( TRUE );

		case PVALUETYPE_WCS:
			pvalue_set_wcs( val, pvalue_to_wcs( val ) );
			RETURN( TRUE );

		case PVALUETYPE_PTR:
			pvalue_set_ptr( val, pvalue_to_ptr( val ) );
			RETURN( TRUE );

	}

	RETURN( FALSE );
}

