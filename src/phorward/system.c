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
#include "phorward.h"

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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pgetopt()
	
	Author:			Jan Max Meyer
	
	Usage:			Implementation of a command-line option interpreter.
					This function works similar to the getopt() functions
					of the GNU Standard Library, but uses a different style
					of parameter submit. It supports both short- and long-
					option-style parameters.
					
	Parameters:		uchar*		opt				Requires a pointer to a buffer
												with enough space to store the
												requested parameter to. For
												short parameters, this is only
												one char, for long-parameters
												the full name. The string will
												be zero-terminated.
					uchar**		param			A pointer to uchar* to store
												a possible parameter value to,
												if the detected option allows
												for parameters.
					pint		argc			Argument count as taken from
												the main() function.
					uchar**		argv			Argument values as taken from
												the main() function.
					uchar*		optstr			The possible short-options.
												This is a string where each
												character defines an option.
												If an option takes a parameter,
												a colon is submitted. E.g.
												"abc:def:g". The Options "-c"
												and "-f" will take a parameter
												that is returned to param.
												This parameter can be (uchar*)
												NULL.
					uchar*		loptstr			The possible long-options.
												This is a string containing all
												long option names, each
												separated by a blank. Long
												options taking parameters have
												an attached colon to the name.
												E.g. "hello world: next" defines
												three long options, where option
												'world' takes one parameter that
												is returned to param. This
												parameter can be (uchar*)NULL.
					pint		idx				The index of the requested
												option, 0 for the first
												option behind argv[0].
	
	Returns:		pint						Returns ERR_OK, if no 
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint pgetopt( uchar* opt, uchar** param,
				pint argc, uchar** argv, uchar* optstr,
					uchar* loptstr, pint idx )
{
	BOOLEAN		has_parm;
	BOOLEAN		found;
	BOOLEAN		lopt;
	int			i;
	uchar*		str;
	uchar*		pos;
	uchar*		del;
	int			cnt		= 0;
	static char	optinfo	[ 2+1 ];

	PROC( "pgetopt" );
	PARMS( "opt", "%p", opt );
	PARMS( "param", "%p", param );
	PARMS( "argc", "%d", argc );
	PARMS( "argv", "%p", argv );
	PARMS( "optstr", "%s", optstr );
	PARMS( "loptstr", "%s", loptstr );
	PARMS( "idx", "%d", idx );

	if( !( opt && param && idx >= 0 && ( optstr || loptstr ) ) )
	{
		RETURN( ERR_PARMS );
	}

	*param = (uchar*)NULL;

	if( argc < 2 )
		RETURN( 1 );

	for( i = 1; i < argc; i++ )
	{
		found = FALSE;
		str = argv[i];
		lopt = FALSE;

		if( *str == '-' )
		{
			str++;
			if( *str == '-' )
			{
				str++;
				lopt = TRUE;
			}
		}
		else
		{
			*param = argv[i];
			RETURN( 1 );
		}

		if( !lopt && optstr && *optstr )
		{
			while( *str && idx >= 0 )
			{
				found = FALSE;
				for( pos = optstr; *pos; pos++ )
				{
					if( *pos == ':' )
						continue;

					if( *pos == *str )
					{
						if( *( pos + 1 ) == ':' )
						{
							if( argc > ++i )
							{
								if( !idx )
									*param = argv[ i ];
							}
						}

						if( !idx )
						{
							psprintf( opt, "%c", *pos );
							RETURN( ERR_OK );
						}

						found = TRUE;
						break;
					}
				}

				if( !found )
				{
					sprintf( optinfo, "-%c", *str );
					*param = optinfo;
					RETURN( ERR_FAILURE );
				}

				idx--;
				str++;
			}
		}
		else if( lopt && loptstr && *loptstr )
		{
			for( pos = loptstr; *pos; pos = ( *del ? del + 1 : del ) )
			{
				if( !( del = pstrstr( pos, " " ) ) )
					del = pos + pstrlen( pos );

				if( del == pos )
					continue;

				if( del > pos && *( del - 1 ) == ':' )
				{
					has_parm = TRUE;
					del--;
				}
				else
					has_parm = FALSE;

				if( pstrncmp( pos, str, del - pos ) == 0 )
				{
					if( has_parm && argc > ++i )
					{
						if( !idx )
							*param = argv[ i ];
					}

					if( !idx )
					{
						psprintf( opt, "%.*s", (int)( del - pos ), str );
						RETURN( ERR_OK );
					}

					found = TRUE;
					break;
				}
			}

			if( !found )
			{
				*param = argv[ i ];
				RETURN( ERR_FAILURE );
			}

			idx--;
		}
		else
			break;
	}

	RETURN( ERR_FAILURE );
}
