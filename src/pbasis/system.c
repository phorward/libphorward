/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pfileexists()
	
	Author:			Jan Max Meyer
	
	Usage:			Checks wether a file exists or not.
					
	Parameters:		uchar*		filename		Path to a file.
	
	Returns:		pboolean					TRUE on success, FALSE if not.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean pfileexists( uchar* filename )
{
	PROC( "pfileexists" );
	PARMS( "filename", "%s", filename );
	
	if( !
	#ifndef _WIN32
		access( filename, F_OK )
	#else
		access( filename, 0 )
	#endif
	)
	{
		MSG( "File exists!" );
		RETURN( TRUE );
	}
		
	MSG( "File does not exist!" );
	RETURN( FALSE );
}

