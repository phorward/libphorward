/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	union.h
Author:	Jan Max Meyer
Usage:	Structures and definitions for a variant-style data storage type punion.
		The only hand-written modules of the punion variant data type exists in
		union.h and union.c. The files union.get.c, union.set.c and union.conv.c
		are automatically generated from the definitions below, using
		union.gen.awk. Due this automatic generation of the get/set/conversion
		functions, the punion data type can easily be changed and extended to
		new data types without huger code changes.
----------------------------------------------------------------------------- */

#ifndef PUNION_H
#define PUNION_H

/* Defines */
#define PUNION_NULL			0	/* Please watch out for binary order!! */
#define PUNION_BYTE			1
#define PUNION_CHAR			2
#define PUNION_INT			3
#define PUNION_LONG			4
#define PUNION_ULONG		5
#define PUNION_FLOAT		6
#define PUNION_DOUBLE		7
#define PUNION_STRING		8
#define PUNION_WSTRING		9

#define PUNION_IS_CONSTANT	16	/* Const-value flag for strings, so no automatic
									free is done on these variables */
#define PUNION_IS_CONVERT	32	/* Allow on-the-fly type conversion with
									(possible) data-loss */

/* Typedefs */
typedef struct 
{
	pbyte		type;

	union
	{
		pbyte	b;
		/*vargen:byte:%d::0
			to char*: plong_to_uchar( (plong)var->val.b )
			to pchar*: plong_to_pchar( (plong)var->val.b )
		*/

		char	c;
		/*vargen:char:%c::0
			to char*: plong_to_uchar( (plong)var->val.c )
			to pchar*: plong_to_pchar( (plong)var->val.c )
		*/

		pint	i;
		/*vargen:int:%d::0
			to char*: plong_to_uchar( (plong)var->val.i )
			to pchar*: plong_to_pchar( (plong)var->val.i )
		*/

		plong	l;
		/*vargen:long:%ld::0
			to char*: plong_to_uchar( var->val.l )
			to pchar*: plong_to_pchar( var->val.l )		
		*/

		pulong	ul;
		/*vargen:ulong:%ld::0
			to char*: pulong_to_uchar( var->val.ul )
			to pchar*: pulong_to_pchar( var->val.ul )
		*/

		pfloat	f;
		/*vargen:float:%f::0.0
			to char*: pdouble_to_uchar( (double)var->val.f )
			to pchar*: pdouble_to_pchar( (double)var->val.f )
		*/

		pdouble	d;
		/*vargen:double:%lf::0.0
			to char*: pdouble_to_uchar( var->val.d )
			to pchar*: pdouble_to_pchar( var->val.d )
		*/

		char*	s;
		/*vargen:cstring:%s:PUNION_STRING:NULL
			set: punion_set_constant( var );
		*/
		/*vargen:string:%s::NULL
			to pbyte: strtol( var->val.s, (char**)NULL, 0 )
			to char: (same)
			to pint: (same)
			to plong: (same)
			to pulong: (same)
			to pfloat: strtod( var->val.s, (char**)NULL )
			to pdouble: (same)
			to pchar*: u8_to_wchar( var->val.s, FALSE )
		*/

		pchar*	ws;
		/*vargen:wcstring:%ls:PUNION_WSTRING:NULL
			set: punion_set_constant( var );
		*/
		/*vargen:wstring:%ls::NULL
			to pbyte: wcstol( var->val.ws, (pchar**)NULL, 0 )
			to char: (same)
			to pint: (same)
			to plong: (same)
			to pulong: (same)
			to pfloat: wcstod( var->val.ws, (pchar**)NULL )
			to pdouble: (same)
			to char*: wchar_to_u8( var->val.ws, FALSE )
		*/
	} val;
} punion;

/* Macros */
#define punion_set_string_d( var, val ) \
			punion_set_string( var, pstrdup( val ) )
#define punion_set_wstring_d( var, val ) \
			punion_set_wstring( var, pwcsdup( val ) )

#define punion_set_constant( var ) \
			bit_set( (var)->type, PUNION_IS_CONSTANT )
#define punion_unset_constant( var ) \
			bit_unset( (var)->type, PUNION_IS_CONSTANT )
#define punion_set_convertible( var ) \
			bit_set( (var)->type, PUNION_IS_CONVERT )
#define punion_unset_convertible( var ) \
			bit_unset( (var)->type, PUNION_IS_CONVERT )

#define punion_type( var ) \
			( ( var )->type & 0x0F )
#define punion_is_constant( var ) \
			( ( var )->type & PUNION_IS_CONSTANT )
#define punion_is_convertible( var ) \
			( ( var )->type & PUNION_IS_CONVERT )

#endif

