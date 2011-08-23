/* -MODULE----------------------------------------------------------------------
NEWPROJECT
Copyright (C) 2010, 2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<AT>phorward<DASH>software<DOT>com

File:	main.c
Author:	Jan Max Meyer
Usage:	Program entry and parameter parsing.
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <pbasis.h>
#include <pregex.h>
#include <pstring.h>

/*
 * Global variables
 */

/*
 * Defines
 */
#define VERSION		"0.4"

/*
 * Functions
 */
 
 /* -FUNCTION--------------------------------------------------------------------
	Function:		copyright()
	
	Author:			Jan Max Meyer
	
	Usage:			Prints a program copyright info message.
					
	Parameters:		void

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void copyright( void )
{
	fprintf( stderr, "NEWPROJECT v%s [build %s %s]\n",
			VERSION, __DATE__, __TIME__ );
	fprintf( stderr, "Copyright (C) 2011 by "
						"Phorward Software Technologies, Jan Max Meyer\n" );
	fprintf( stderr, "http://www.phorward-software.com ++ "
						"contact<at>phorward<dash>software<dot>com\n\n" );
						
	fprintf( stderr, "THIS IS A TEMPLATE FOR NEW PROJECTS!\n\n" );
	
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		usage()
	
	Author:			Jan Max Meyer
	
	Usage:			Prints the program usage message.
					
	Parameters:		uchar*		progname			Name of the executable.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void usage( uchar* progname )
{
	fprintf( stderr, "Usage: %s OPTIONS...\n\n"
		"\t-h   --help            Print this help\n"
		"\t-V   --version         Print version and copyright\n",

		progname );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		get_command_line()
	
	Author:			Jan Max Meyer
	
	Usage:			Analyzes the command line parameters passed to this program.
					
	Parameters:		int			argc		Argument count from main()
					uchar**		argv		Argument values from main()
	
	Returns:		BOOLEAN					TRUE, if command-line parameters
											are correct, FALSE else.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
BOOLEAN get_command_line( int argc, char** argv )
{
	int		rc;
	int		i;
	uchar	opt[ 20 + 1 ];
	uchar*	param;

	for( i = 0; ( rc = pgetopt( opt, &param, argc, argv,
						"Vh", "version help", i ) ) == ERR_OK; i++ )
	{
		switch( *opt )
		{
			case 'V':
			case 'v':
				copyright();
				exit( EXIT_SUCCESS );

			case 'h':
				usage( *argv );
				exit( EXIT_SUCCESS );

			default:
				break;
		}
	}

	if( rc == ERR_FAILURE )
	{
		fprintf( stderr, "Unknown command-line option: %s\n", param );
		exit( EXIT_FAILURE );
	}

	return TRUE;
}

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
	
	if( !get_command_line( argc, argv ) )
		RETURN( EXIT_FAILURE );

	printf( "Hello World!\n" );

	RETURN( EXIT_SUCCESS );
}

