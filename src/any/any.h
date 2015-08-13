/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	any.h
Author:	Jan Max Meyer
Usage:	Structures and definitions for a variant-style data storage type pany.

		The only hand-written modules of the pany variant data type exists in
		any.h and any.c.
		
		The files any.get.c, any.set.c and any.conv.c are automatically
		generated from the definitions below, using any.gen.awk. Due this
		automatic generation of the get/set/conversion functions, the pany
		data type can easily be changed and extended to new data types without
		huger code changes.
----------------------------------------------------------------------------- */

/* Defines */
typedef enum
{
	PANYTYPE_NULL,

	/* Primary */
	PANYTYPE_CHAR,
	PANYTYPE_INT,
	PANYTYPE_LONG,
	PANYTYPE_ULONG,
	PANYTYPE_FLOAT,
	PANYTYPE_DOUBLE,

	/* String */
	PANYTYPE_STR,
	PANYTYPE_WCS,

	/* Special */
	PANYTYPE_PTR
} panytype;


#define PANYFLAG_CONSTANT		16	/* Const-value flag for strings, so no
										automatic freeing is done on these
											variables */
#define PANYFLAG_AUTOCONVERT	32	/* Allow on-the-fly type conversion with
										(possible) data-loss */

/* Typedefs */
typedef struct
{
	panytype	type;				/* Data type */
	short		flags;				/* Flags */

	union
	{
		char		c;
		/*vargen:char:%c::0
			to char*: pasprintf( "%d", val->val.c )
			to wchar_t*: pawcsprintf( L"%d", val->val.c )
			to void*: NULL
		*/

		int			i;
		/*vargen:int:%d::0
			to char*: pasprintf( "%d", val->val.i )
			to wchar_t*: pawcsprintf( L"%d", val->val.i )
			to void*: NULL
		*/

		long		l;
		/*vargen:long:%ld::0
			to char*: pasprintf( "%ld", val->val.l )
			to wchar_t*: pawcsprintf( L"%ld", val->val.l )
			to void*: val->val.l
		*/

		ulong		ul;
		/*vargen:ulong:%ld::0
			to char*: pasprintf( "%ld", val->val.ul )
			to wchar_t*: pawcsprintf( L"%ld", val->val.ul )
			to void*: val->val.ul
		*/

		float		f;
		/*vargen:float:%f::0.0
			to char*: pdbl_to_str( (double)val->val.f )
			to wchar_t*: pdbl_to_wcs( (double)val->val.f )
			to void*: NULL
		*/

		double		d;
		/*vargen:double:%lf::0.0
			to char*: pdbl_to_str( val->val.d )
			to wchar_t*: pdbl_to_wcs( val->val.d )
			to void*: NULL
		*/

		char*		s;
		/*vargen:cstr:%s:PANYTYPE_STR:NULL
			set: pany_set_constant( val, TRUE );
		*/
		/*vargen:str:%s::NULL
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
		/*vargen:cwcs:%ls:PANYTYPE_WCS:NULL
			set: pany_set_constant( val, TRUE );
		*/
		/*vargen:wcs:%ls::NULL
			to char: wcstol( val->val.ws, (wchar_t**)NULL, 0 )
			to int: (same)
			to long: (same)
			to ulong: (same)
			to float: wcstod( val->val.ws, (wchar_t**)NULL )
			to double: (same)
			to char*: pwcs_to_str( val->val.ws, FALSE )
			to void*: val->val.ws
		*/

		void*		ptr;
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
} pany;

/* Macros */
#define pany_set_strdup( val, str ) \
			pany_set_str( val, pstrdup( str ) )
#define pany_set_wcsdup( val, str ) \
			pany_set_wcs( val, pwcsdup( str ) )

