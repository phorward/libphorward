/* -MODULE----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	direct.c
Author:	Jan Max Meyer
Usage:	Direct regular expression access functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pregex.h"

/*
 * Global variables
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_match()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs a regular expression search on a string, and
					returns an array of matches via a pregex_result-structure,
					which holds pointers to the begin- and end-addresses
					of the matches.
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			Searchstring the pattern
												will be ran on.
					int				flags		Flags for regular
												expression mode switching
					pregex_result*	results		Array of results to the
												matched substrings within
												str. results must be freed
												after usage.
																	
	Returns:		>= 0						The amount of matches, which
												is the amount of items within
												the returned results-array.
												
					ERR_...						ERR_*-define on error.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_match( uchar* regex, uchar* str, int flags, pregex_result** results )
{
	int				matches	= 0;
	int				ret;
	size_t			len;
	uchar*			pstr;
	pregex_nfa		nfa;

	PROC( "pregex_match" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "results", "%p", results );

	memset( &nfa, 0, sizeof( pregex_nfa ) );
	if( ( ret = pregex_compile_to_nfa( regex, &nfa, flags, 0 ) ) != ERR_OK )
	{
		MSG( "Unable to compile regex" );
		RETURN( ret );
	}

	for( pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );
		if( pregex_nfa_match( &nfa, pstr, &len,
				(pregex_result**)NULL, (int*)NULL ) == 0 )
		{
			if( !matches )
				*results = (pregex_result*)pmalloc( sizeof( pregex_result ) );
			else
				*results = (pregex_result*)prealloc(
								(pregex_result*)*results,
									( matches + 1 ) * sizeof( pregex_result ) );

			if( !*results )
				RETURN( ERR_MEM );

			(*results)[ matches ].begin = pstr;

			for( ; len > 0; len-- )
				pstr += u8_seqlen( pstr );

			(*results)[ matches ].end = pstr;
			matches++;
		}
		else
			pstr += u8_seqlen( pstr );

	}
	
	pregex_nfa_free( &nfa );	

	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_replace()
	
	Author:			Jan Max Meyer
	
	Usage:			
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			Searchstring the pattern
												will be ran on.
					int				flags		Flags for regular
												expression mode switching
																	
	Returns:		>= 0						The amount of matches, which
												is the amount of items within
												the returned results-array.
												
					ERR_...						ERR_*-define on error.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_replace( uchar* regex, uchar* str, uchar* replace, int flags )
{
	pregex_result*	refs;
	int				refs_cnt;	
	int				matches	= 0;
	int				ret;
	int				i;
	size_t			len;
	uchar*			pstr;
	pregex_nfa		nfa;
	uchar*			refout;

	PROC( "pregex_match" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	
	fprintf( stderr, "string >%s<\n", str );

	memset( &nfa, 0, sizeof( pregex_nfa ) );
	if( ( ret = pregex_compile_to_nfa( regex, &nfa, flags, 0 ) ) != ERR_OK )
	{
		MSG( "Unable to compile regex" );
		RETURN( ret );
	}

	for( pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );
		if( pregex_nfa_match( &nfa, pstr, &len, &refs, &refs_cnt ) == 0 )
		{
			VARS( "refs_cnt", "%d", refs_cnt );

			if( refs_cnt )
			{
				MSG( "Having references" );
				
				for( i = 0; i < refs_cnt; i++ )
				{
					if( refs[i].begin && refs[i].end )
					{
						VARS( "refs[i].begin", "%p", refs[i].begin );
						VARS( "refs[i].end", "%p", refs[i].end );
						refout = (uchar*)pmalloc(
							( refs[ i ].end - refs[ i ].begin + 1 )
								* sizeof( uchar ) );
						sprintf( refout, "%.*s", 
							refs[ i ].end - refs[ i ].begin,
								refs[ i ].begin );
												
						VARS( "Reference", "%s", refout );
						
						fprintf( stderr, "reference in match %d, #%d: >%s<\n",
									matches, i, refout );
						pfree( refout );
					}
					else
					{
						MSG( "Reference is empty / not used" );
					}
				}
				
				pfree( refs );
			}

			pstr += len;
			matches++;
		}
		else
			pstr += u8_seqlen( pstr );

	}
	
	pregex_nfa_free( &nfa );	

	RETURN( matches );
}

