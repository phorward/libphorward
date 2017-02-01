/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
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

/* Types */
typedef enum
{
	PANYTYPE_NULL,

	/* Primary */
	PANYTYPE_BOOL,
	PANYTYPE_CHAR,
	PANYTYPE_INT,
	PANYTYPE_LONG,
	PANYTYPE_ULONG,
	PANYTYPE_FLOAT,
	PANYTYPE_DOUBLE,

	/* String */
	PANYTYPE_STR,
	PANYTYPE_CSTR,
	PANYTYPE_WCS,
	PANYTYPE_CWCS,

	/* Special */
	PANYTYPE_PTR
} panytype;

/* Object */
typedef struct
{
	panytype		type;			/* Data type */

	union							/* Unfortunatelly not the
										brewery of Dortmund,
											but anyways... ;) */
	{
		pboolean		b;
		/*type:bool
			format:%d
			empty:FALSE

			to char*: pstrdup( BOOLEAN_STR( val->val.c ) )
			to wchar_t*: pwcsdup( val->val.c ? L"TRUE" : L"FALSE" )
			to void*: NULL
		*/

		char			c;
		/*type:char
			format:%c
			empty:0

			to pboolean: TRUEBOOLEAN( val->val.c )
			to char*: pasprintf( "%d", val->val.c )
			to wchar_t*: pawcsprintf( L"%d", val->val.c )
			to void*: NULL
			from char*: *$from
			from wchar_t*: (char)( *$from )
		*/

		int				i;
		/*type:int
			format:%d
			empty:0

			to pboolean: TRUEBOOLEAN( val->val.i )
			to char*: pasprintf( "%d", val->val.i )
			to wchar_t*: pawcsprintf( L"%d", val->val.i )
			to void*: NULL
			from char*: atoi( $from )
			from wchar_t*: (int)wcstol( $from, (wchar_t**)NULL, 0 )
		*/

		long			l;
		/*type:long
			format:%ld
			empty:0

			to pboolean: TRUEBOOLEAN( val->val.l )
			to char*: pasprintf( "%ld", val->val.l )
			to wchar_t*: pawcsprintf( L"%ld", val->val.l )
			to void*: val->val.l
			from char*: atol( $from )
		*/

		unsigned long	ul;
		/*type:ulong
			format:%ld
			empty:0

			to pboolean: TRUEBOOLEAN( val->val.ul )
			to char*: pasprintf( "%ld", val->val.ul )
			to wchar_t*: pawcsprintf( L"%ld", val->val.ul )
			to void*: val->val.ul
		*/

		float			f;
		/*type:float
			format:%f
			empty:0.0

			to pboolean: TRUEBOOLEAN( val->val.f )
			to char*: pdbl_to_str( (double)val->val.f )
			to wchar_t*: pdbl_to_wcs( (double)val->val.f )
			to void*: NULL
		*/

		double			d;
		/*type:double
			format:%lf
			empty:0.0

			to pboolean: TRUEBOOLEAN( val->val.d )
			to char*: pdbl_to_str( val->val.d )
			to wchar_t*: pdbl_to_wcs( val->val.d )
			to void*: NULL
		*/

		char*			s;
		/*type:cstr
			format:%s
			empty:NULL
		*/
		/*type:str
			format:%s
			empty:NULL

			to pboolean: TRUEBOOLEAN( *pstrget( val->val.s ) )
			to char: strtol( val->val.s, (char**)NULL, 0 )
			to int: (same)
			to long: (same)
			to unsigned long: (same)
			to float: strtod( val->val.s, (char**)NULL )
			to double: (same)
			to wchar_t*: pstr_to_wcs( val->val.s, FALSE )
			to void*: val->val.s
		*/

		wchar_t*		ws;
		/*type:cwcs
			format:%ls
			empty:NULL
		*/

		/*type:wcs
			format:%ls
			empty:NULL

			to pboolean: TRUEBOOLEAN( *pwcsget( val->val.ws ) )
			to char: wcstol( val->val.ws, (wchar_t**)NULL, 0 )
			to int: (same)
			to long: (same)
			to unsigned long: (same)
			to float: wcstod( val->val.ws, (wchar_t**)NULL )
			to double: (same)
			to char*: pwcs_to_str( val->val.ws, FALSE )
			to void*: val->val.ws
		*/

		void*			ptr;
		/*type:ptr
			format:%p
			empty:NULL

			to pboolean: TRUEBOOLEAN( val->val.ptr )
			to char: 0
			to int: 0
			to long: val->val.ptr
			to unsigned long: (same)
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
#define pany_set_strndup( val, str, n ) \
			pany_set_str( val, pstrndup( str, n ) )

#define pany_set_wcsdup( val, wcs ) \
			pany_set_wcs( val, pwcsdup( wcs ) )
#define pany_set_wcsndup( val, wcs ) \
			pany_set_wcs( val, pwcnsdup( wcs ) )

