/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	value.h
Author:	Jan Max Meyer
Usage:	Structures and definitions for a variant-style data storage type pgvalue.
		The only hand-written modules of the pgvalue variant data type exists in
		union.h and union.c. The files union.get.c, union.set.c and union.conv.c
		are automatically generated from the definitions below, using
		union.gen.awk. Due this automatic generation of the get/set/conversion
		functions, the pgvalue data type can easily be changed and extended to
		new data types without huger code changes.
----------------------------------------------------------------------------- */

#ifndef PGVALUETYPE_H
#define PGVALUETYPE_H

/* Defines */
typedef enum
{
	PGVALUETYPE_NULL,

	/* Primary */
	PGVALUETYPE_CHAR,
	PGVALUETYPE_INT,
	PGVALUETYPE_LONG,
	PGVALUETYPE_ULONG,
	PGVALUETYPE_FLOAT,
	PGVALUETYPE_DOUBLE,

	/* String */
	PGVALUETYPE_STRING,
	PGVALUETYPE_WSTRING,

	/* Special */
	PGVALUETYPE_PTR
} pgvaluetype;


#define PGVALUEFLAG_CONSTANT	16	/* Const-value flag for strings, so no automatic
									free is done on these variables */
#define PGVALUEFLAG_AUTOCONVERT	32	/* Allow on-the-fly type conversion with
									(possible) data-loss */

/* Typedefs */
typedef struct
{
	pgvaluetype	type;				/* Data type */
	short		flags;				/* Flags */

	union
	{
		char	c;
		/*vargen:char:%c::0
			to char*: pasprintf( "%d", val->val.c )
			to wchar_t*: pawcsprintf( L"%d", val->val.c )
			to void*: NULL
		*/

		int	i;
		/*vargen:int:%d::0
			to char*: pasprintf( "%d", val->val.i )
			to wchar_t*: pawcsprintf( L"%d", val->val.i )
			to void*: NULL
		*/

		long	l;
		/*vargen:long:%ld::0
			to char*: pasprintf( "%ld", val->val.l )
			to wchar_t*: pawcsprintf( L"%ld", val->val.l )
			to void*: val->val.l
		*/

		ulong	ul;
		/*vargen:ulong:%ld::0
			to char*: pasprintf( "%ld", val->val.ul )
			to wchar_t*: pawcsprintf( L"%ld", val->val.ul )
			to void*: val->val.ul
		*/

		float	f;
		/*vargen:float:%f::0.0
			to char*: pdbl_to_str( (double)val->val.f )
			to wchar_t*: pdbl_to_wcs( (double)val->val.f )
			to void*: NULL
		*/

		double	d;
		/*vargen:double:%lf::0.0
			to char*: pdbl_to_str( val->val.d )
			to wchar_t*: pdbl_to_wcs( val->val.d )
			to void*: NULL
		*/

		char*	s;
		/*vargen:cstring:%s:PGVALUETYPE_STRING:NULL
			set: pg_value_set_constant( val, TRUE );
		*/
		/*vargen:string:%s::NULL
			to char: strtol( val->val.s, (char**)NULL, 0 )
			to int: (same)
			to long: (same)
			to ulong: (same)
			to float: strtod( val->val.s, (char**)NULL )
			to double: (same)
			to wchar_t*: pstr_to_wcs( val->val.s, FALSE )
			to void*: val->val.s
		*/

		wchar_t*	ws;
		/*vargen:wcstring:%ls:PGVALUETYPE_WSTRING:NULL
			set: pg_value_set_constant( val, TRUE );
		*/
		/*vargen:wstring:%ls::NULL
			to char: wcstol( val->val.ws, (wchar_t**)NULL, 0 )
			to int: (same)
			to long: (same)
			to ulong: (same)
			to float: wcstod( val->val.ws, (wchar_t**)NULL )
			to double: (same)
			to char*: pwcs_to_str( val->val.ws, FALSE )
			to void*: val->val.ws
		*/

		void*	ptr;
		/*vargen:ptr:%p::NULL
			to char: 0
			to int: 0
			to long: val->val.ptr
			to ulong: (same)
			to float: 0.0
			to double: 0.0
			to char*: pasprintf( "%p", val->val.ptr )
			to wchar_t*: pawcsprintf( L"%p", val->val.ptr )
		*/
	} val;
} pgvalue;

/* Macros */
#define pg_value_set_string_d( val, str ) \
			pg_value_set_string( val, pstrdup( str ) )
#define pg_value_set_wstring_d( val, str ) \
			pg_value_set_wstring( val, pwcsdup( str ) )

#endif

