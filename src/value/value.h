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

#ifndef PGVALUE_H
#define PGVALUE_H

/* Defines */
typedef enum
{
	PGVALUE_NULL,

	/* Primary */
	PGVALUE_CHAR,
	PGVALUE_INT,
	PGVALUE_LONG,
	PGVALUE_ULONG,
	PGVALUE_FLOAT,
	PGVALUE_DOUBLE,

	/* String */
	PGVALUE_STRING,
	PGVALUE_WSTRING,

	/* Special */
	PGVALUE_PTR
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
			to pchar*: pawcsprintf( L"%d", val->val.c )
			to void*: NULL
		*/

		int	i;
		/*vargen:int:%d::0
			to char*: pasprintf( "%d", val->val.i )
			to pchar*: pawcsprintf( L"%d", val->val.i )
			to void*: NULL
		*/

		long	l;
		/*vargen:long:%ld::0
			to char*: pasprintf( "%ld", val->val.l )
			to pchar*: pawcsprintf( L"%ld", val->val.l )
			to void*: val->val.l
		*/

		ulong	ul;
		/*vargen:ulong:%ld::0
			to char*: pasprintf( "%ld", val->val.ul )
			to pchar*: pawcsprintf( L"%ld", val->val.ul )
			to void*: val->val.ul
		*/

		float	f;
		/*vargen:float:%f::0.0
			to char*: pdouble_to_uchar( (double)val->val.f )
			to pchar*: pdouble_to_pchar( (double)val->val.f )
			to void*: NULL
		*/

		double	d;
		/*vargen:double:%lf::0.0
			to char*: pdouble_to_uchar( val->val.d )
			to pchar*: pdouble_to_pchar( val->val.d )
			to void*: NULL
		*/

		char*	s;
		/*vargen:cstring:%s:PGVALUE_STRING:NULL
			set: pgvalue_set_constant( val );
		*/
		/*vargen:string:%s::NULL
			to char: strtol( val->val.s, (char**)NULL, 0 )
			to int: (same)
			to long: (same)
			to ulong: (same)
			to float: strtod( val->val.s, (char**)NULL )
			to double: (same)
			to pchar*: u8_to_wchar( val->val.s, FALSE )
			to void*: val->val.s
		*/

		pchar*	ws;
		/*vargen:wcstring:%ls:PGVALUE_WSTRING:NULL
			set: pgvalue_set_constant( val );
		*/
		/*vargen:wstring:%ls::NULL
			to char: wcstol( val->val.ws, (pchar**)NULL, 0 )
			to int: (same)
			to long: (same)
			to ulong: (same)
			to float: wcstod( val->val.ws, (pchar**)NULL )
			to double: (same)
			to char*: wchar_to_u8( val->val.ws, FALSE )
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
			to pchar*: pawcsprintf( L"%p", val->val.ptr )
		*/
	} val;
} pgvalue;

/* Macros */
#define pgvalue_set_string_d( val, str ) \
			pgvalue_set_string( val, pstrdup( str ) )
#define pgvalue_set_wstring_d( val, str ) \
			pgvalue_set_wstring( val, pwcsdup( str ) )

#endif

