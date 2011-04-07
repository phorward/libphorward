/* -MODULE----------------------------------------------------------------------
Phorward Basis Library :: Platform-independent system functions
Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	system.c
Author:	Jan Max Meyer
Usage:	System functions for various usages.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pbasis.h"

/*
 * Global variables
 */

/*
 * Defines
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pbasename()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the Basename of a file.
					
	Parameters:		uchar*		path			File path pointer
	
	Returns:		uchar*						Static pointer to the basename.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* pbasename( uchar* path )
{
	uchar*		basename;
	
	PROC( "pbasename" );
	PARMS( "path", "%s", path );
	
	basename = strrchr( path, PPATHSEP );
	VARS( "basename", "%s", basename ? basename+1 : path );
	RETURN( ( basename ) ? ++basename : path );
}

