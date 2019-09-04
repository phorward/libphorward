/* -HEADER----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	boolean.h
Usage:	Definitions for a boolean data type
----------------------------------------------------------------------------- */

typedef char 					pboolean;	/* Phorward Boolean */

#ifndef BOOLEAN
#define BOOLEAN					pboolean
#endif

#ifndef boolean
#define boolean 				pboolean
#endif


/** Value 1 for TRUE. */
/*MACRO:TRUE */

#ifndef TRUE
#define TRUE					1
#endif

/** Value 0 for FALSE. */
/*MACRO:FALSE */

#ifndef FALSE
#define FALSE					0
#endif


/** Returns the string represenation of the value or expression in //b//. */
/*MACRO:BOOLEAN_STR( b )*/
#define BOOLEAN_STR( b ) 		( ( b ) ? "TRUE" : "FALSE" )

/** Turns the value or expression //b// into a pboolean value TRUE or FALSE. */
/*MACRO:MAKE_BOOLEAN( b )*/
#define MAKE_BOOLEAN( b ) 		( ( b ) ? TRUE : FALSE )
