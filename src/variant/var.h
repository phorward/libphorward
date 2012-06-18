/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	var.h
Author:	Jan Max Meyer
Usage:	Structures and definitions for a variant-style data storage type pvar.
		The only hand-written modules of the pvar variant data type exists in
		var.h and var.c. The files var.get.c, var.set.c and var.conv.c are
		automatically generated from the definitions below, using var.gen.awk.
		Due this automatic generation of the get/set/conversion functions, the
		pvar data type can easily be changed and extended to new data types
		without huger code changes.
----------------------------------------------------------------------------- */

#ifndef VAR_H
#define VAR_H

/* Defines */
#define PVAR_NULL			0	/* Please watch out for binary order!! */
#define PVAR_BYTE			1
#define PVAR_CHAR			2
#define PVAR_INT			3
#define PVAR_LONG			4
#define PVAR_ULONG			5
#define PVAR_FLOAT			6
#define PVAR_DOUBLE			7
#define PVAR_STRING			8
#define PVAR_WSTRING		9

#define PVAR_IS_CONSTANT	16	/* Const-value flag for strings, so no automatic
									free is done on these variables */
#define PVAR_IS_CONVERT		32	/* Allow on-the-fly type conversion with
									(possible) data-loss */

/* Typedefs */
typedef struct 
{
	pbyte		type;

	union
	{
		pbyte	b;
		/*vargen:byte:%d::0
			to uchar*: plong_to_uchar( (plong)var->val.b )
			to pchar*: plong_to_pchar( (plong)var->val.b )
		*/

		uchar	c;
		/*vargen:char:%c::0
			to uchar*: plong_to_uchar( (plong)var->val.c )
			to pchar*: plong_to_pchar( (plong)var->val.c )
		*/

		pint	i;
		/*vargen:int:%d::0
			to uchar*: plong_to_uchar( (plong)var->val.i )
			to pchar*: plong_to_pchar( (plong)var->val.i )
		*/

		plong	l;
		/*vargen:long:%ld::0
			to uchar*: plong_to_uchar( var->val.l )
			to pchar*: plong_to_pchar( var->val.l )		
		*/

		pulong	ul;
		/*vargen:ulong:%ld::0
			to uchar*: pulong_to_uchar( var->val.ul )
			to pchar*: pulong_to_pchar( var->val.ul )
		*/

		pfloat	f;
		/*vargen:float:%f::0.0
			to uchar*: pdouble_to_uchar( (double)var->val.f )
			to pchar*: pdouble_to_pchar( (double)var->val.f )
		*/

		pdouble	d;
		/*vargen:double:%lf::0.0
			to uchar*: pdouble_to_uchar( var->val.d )
			to pchar*: pdouble_to_pchar( var->val.d )
		*/

		uchar*	s;
		/*vargen:cstring:%s:PVAR_STRING:NULL
			set: pvar_set_constant( var );
		*/
		/*vargen:string:%s::NULL
			to pbyte: pstrtol( var->val.s, (uchar**)NULL, 0 )
			to uchar: (same)
			to pint: (same)
			to plong: (same)
			to pulong: (same)
			to pfloat: pstrtod( var->val.s, (uchar**)NULL )
			to pdouble: (same)
			to pchar*: uchar_to_pchar( var->val.s, FALSE )
		*/

		pchar*	ws;
		/*vargen:wcstring:%ls:PVAR_WSTRING:NULL
			set: pvar_set_constant( var );
		*/
		/*vargen:wstring:%ls::NULL
			to pbyte: Pstrtol( var->val.ws, (pchar**)NULL, 0 )
			to uchar: (same)
			to pint: (same)
			to plong: (same)
			to pulong: (same)
			to pfloat: Pstrtod( var->val.ws, (pchar**)NULL )
			to pdouble: (same)
			to uchar*: pchar_to_uchar( var->val.ws, FALSE )
		*/
	} val;
} pvar;

/* Macros */
#define pvar_set_string_d( var, val ) \
			pvar_set_string( var, pstrdup( val ) )
#define pvar_set_wstring_d( var, val ) \
			pvar_set_wstring( var, Pstrdup( val ) )

#define pvar_set_constant( var ) \
			bit_set( (var)->type, PVAR_IS_CONSTANT )
#define pvar_unset_constant( var ) \
			bit_unset( (var)->type, PVAR_IS_CONSTANT )
#define pvar_set_convertible( var ) \
			bit_set( (var)->type, PVAR_IS_CONVERT )
#define pvar_unset_convertible( var ) \
			bit_unset( (var)->type, PVAR_IS_CONVERT )

#define pvar_type( var ) \
			( ( var )->type & 0x0F )
#define pvar_is_constant( var ) \
			( ( var )->type & PVAR_IS_CONSTANT )
#define pvar_is_convertible( var ) \
			( ( var )->type & PVAR_IS_CONVERT )

#endif

