/* -MODULE----------------------------------------------------------------------
NEWPROJECT
Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<AT>phorward<DASH>software<DOT>com

File:	main.c
Author:	Jan Max Meyer
Usage:	Program entry and parameter parsing.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <pbasis.h>

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
	Function:		main()
	
	Author:			Jan Max Meyer
	
	Usage:			Program entry
					
	Parameters:		int		argc				Argument count
					char**	argv				Argument values

	
	Returns:		int							Program return code
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int main( int argc, char** argv )
{
	PROC( "main" );

	printf( "Hello World!\n" );

	RETURN( EXIT_SUCCESS );
}

