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
					pregex_result**	results		Array of results to the
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
	PARMS( "flags", "%d", flags );
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
			/*
			if( !matches )
				*results = (pregex_result*)pmalloc( sizeof( pregex_result ) );
			else
				*results = (pregex_result*)prealloc(
								(pregex_result*)*results,
									( matches + 1 ) * sizeof( pregex_result ) );
			*/
			if( !matches )
				*results = (pregex_result*)pmalloc(
								REGEX_ALLOC_STEP * sizeof( pregex_result ) );
			else if( !( matches % REGEX_ALLOC_STEP ) )
				*results = (pregex_result*)prealloc(
								(pregex_result*)*results,
									( ( ( matches / REGEX_ALLOC_STEP ) + 1 )
										* REGEX_ALLOC_STEP )
											*  sizeof( pregex_result ) );

			if( !*results )
				RETURN( ERR_MEM );

			(*results)[ matches ].begin = pstr;
			(*results)[ matches ].pos = pstr - str;

#ifdef UTF8
			for( ; len > 0; len-- )
				pstr += u8_seqlen( pstr );
#else
			pstr += len;
#endif

			(*results)[ matches ].end = pstr;
			(*results)[ matches ].len = pstr - (*results)[ matches ].begin;
			matches++;
						
			if( !( flags & REGEX_MOD_GLOBAL ) )
				break;
		}
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}
	}
	
	pregex_nfa_free( &nfa );

	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_split()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs a regular expression search on a string and uses
					the expression as separator; All strings that where split
					are returned as results-array.
					
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			Searchstring the pattern
												will be ran on.
					int				flags		Flags for regular
												expression mode switching
					pregex_result**	results		Array of results to the
												split substrings. Each element
												of this array contains begin-
												and end-pointer to the
												related strings within the
												input-string str.
																	
	Returns:		>= 0						The amount of elements within
												the returned results-array.
												
					ERR_...						ERR_*-define on error.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_split( uchar* regex, uchar* str, int flags, pregex_result** results )
{
	int				matches	= 0;
	int				ret;
	size_t			len;
	uchar*			pstr;
	uchar*			prev;
	pregex_nfa		nfa;

	PROC( "pregex_split" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );
	PARMS( "results", "%p", results );

	memset( &nfa, 0, sizeof( pregex_nfa ) );
	*results = (pregex_result*)NULL;

	if( ( ret = pregex_compile_to_nfa( regex, &nfa, flags, 0 ) ) != ERR_OK )
	{
		MSG( "Unable to compile regex" );
		RETURN( ret );
	}

	for( prev = pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );
		if( pregex_nfa_match( &nfa, pstr, &len,
				(pregex_result**)NULL, (int*)NULL ) == 0 )
		{
			if( !matches )
				*results = (pregex_result*)pmalloc(
								REGEX_ALLOC_STEP * sizeof( pregex_result ) );
			else if( !( matches % REGEX_ALLOC_STEP ) )
				*results = (pregex_result*)prealloc(
								(pregex_result*)*results,
									( ( ( matches / REGEX_ALLOC_STEP ) + 1 )
										* REGEX_ALLOC_STEP )
											*  sizeof( pregex_result ) );

			if( !*results )
				RETURN( ERR_MEM );

			(*results)[ matches ].begin = prev;
			(*results)[ matches ].end = pstr;
			(*results)[ matches ].pos = prev - str;
			(*results)[ matches ].len = pstr - prev;
			matches++;
			
#ifdef UTF8
			for( ; len > 0; len-- )
				pstr += u8_seqlen( pstr );
#else
			pstr += len;
#endif
			prev = pstr;

			if( !( flags & REGEX_MOD_GLOBAL ) )
			{
				pstr += pstrlen( pstr );
				break;
			}
		}
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}
	}
	
	pregex_nfa_free( &nfa );
	
	/* Put last one if required! */
	if( prev != pstr )
	{
		if( !matches )
			*results = (pregex_result*)pmalloc(
							REGEX_ALLOC_STEP * sizeof( pregex_result ) );
		else if( !( matches % REGEX_ALLOC_STEP ) )
			*results = (pregex_result*)prealloc(
							(pregex_result*)*results,
								( ( ( matches / REGEX_ALLOC_STEP ) + 1 )
									* REGEX_ALLOC_STEP )
										*  sizeof( pregex_result ) );

		if( !*results )
			RETURN( ERR_MEM );

		(*results)[ matches ].begin = prev;
		(*results)[ matches ].end = pstr;
		(*results)[ matches ].pos = prev - str;
		(*results)[ matches ].len = pstr - prev;
		matches++;
	}
	
	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_replace()
	
	Author:			Jan Max Meyer
	
	Usage:			Replaces all matches of a regular expression pattern within
					a string with the replacement. Backreferences can be used
					with $x for each opening bracket within the regular 
					expression.
					
	Parameters:		uchar*			regex		The regular expression
												pattern
					uchar*			str			String the pattern
												will be ran on.
					uchar*			replacement	String that will be inserted
												as replacement for each pattern
												match. $x backreferences
												can be used
					int				flags		Flags for regular
												expression mode switching
					uchar**			result		Returns a pointer to the result
												string.
																	
	Returns:		>= 0						The amount of matches.
												
					ERR_...						ERR_*-define on error.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_replace( uchar* regex, uchar* str, uchar* replacement,
	int flags, uchar** result )
{
	pregex_result*	refs;
	int				refs_cnt	= 0;
	int				matches		= 0;
	int				ret;
	int				i;
	int				ref;
	int				lref;
	size_t			len;
	uchar*			pstr;
	uchar*			prev;
	uchar*			rpstr;
	uchar*			rbegin;
	uchar*			rprev;
	pregex_nfa		nfa;
	uchar*			refout;
	uchar*			replace;

	PROC( "pregex_replace" );
	PARMS( "regex", "%s", regex );
	PARMS( "str", "%s", str );
	PARMS( "replacement", "%s", replacement );
	PARMS( "flags", "%d", flags );
	PARMS( "result", "%p", result );

	memset( &nfa, 0, sizeof( pregex_nfa ) );
	*result = (uchar*)NULL;

	if( ( ret = pregex_compile_to_nfa( regex, &nfa, flags, 0 ) ) != ERR_OK )
	{
		MSG( "Unable to compile regex" );
		RETURN( ret );
	}

	for( prev = pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );
		if( pregex_nfa_match( &nfa, pstr, &len,
			( ( flags & REGEX_MOD_NO_REFERENCES ) ?
					(pregex_result**)NULL : &refs ), &refs_cnt ) == 0 )
		{
			if( flags & REGEX_MOD_NO_REFERENCES )
			{
				MSG( "No references wanted by caller" );
				replace = replacement;
			}
			else
			{
				VARS( "refs_cnt", "%d", refs_cnt );

				MSG( "Constructing replacement" );
				for( rprev = rpstr = replacement, replace = (uchar*)NULL;
						*rpstr; )
				{
					VARS( "*rpstr", "%c", *rpstr );
					if( *rpstr == '$' )
					{
						rbegin = rpstr;

						if( isdigit( *( ++rpstr ) ) )
						{
							ref = atoi( rpstr );
							VARS( "ref", "%d", ref );

							/* Skip length of the number */
							if( !ref )
								rpstr++;
							else
								for( lref = ref; lref; rpstr++ )
									lref /= 10;

							/* Extend first from prev of replacement */
							if( !( replace = pstr_append_nchar( replace,
										rprev, rbegin - rprev ) ) )
								RETURN( ERR_MEM );

							VARS( "replace", "%s", replace );
							
							VARS( "refs[ ref ].begin", "%p",
									refs[ ref ].begin );
							VARS( "refs[ ref ].end", "%p",
									refs[ ref ].end );

							/* Obtain reference information */
							if( refs[ ref ].begin && refs[ ref ].end )
							{
								MSG( "There is a reference!" );
								VARS( "refs[ ref ].begin", "%s",
										refs[ ref ].begin );
								VARS( "len", "%d",
									refs[ ref ].end - refs[ ref ].begin );

								if( !( replace = pstr_append_nchar( replace,
										refs[ ref ].begin,
											refs[ ref ].end -
												refs[ ref ].begin ) ) )
									RETURN( ERR_MEM );
							}
							
							VARS( "replace", "%s", replace );						
							rprev = rpstr;
						}
					}
					else
						rpstr++;
						
					pfree( refs );
				}

				VARS( "rpstr", "%p", rpstr );
				VARS( "rprev", "%p", rprev );
				if( rpstr != rprev && !( replace = pstr_append_str(
											replace, rprev, FALSE ) ) )
					RETURN( ERR_MEM );
			}

			VARS( "replace", "%s", replace );		
			MSG( "Extend result string" );
			
			/* Extend result */
			if( !( *result = pstr_append_nchar(
					*result, prev, pstr - prev ) ) )
				RETURN( ERR_MEM );

			if( !( *result = pstr_append_str( *result, replace,
					( ( replace == replacement ) ? FALSE : TRUE ) ) ) )
				RETURN( ERR_MEM );
			
			VARS( "len", "%d", len );
			VARS( "pstr", "%s", pstr );

			for( ; len > 0; len-- )
				pstr += u8_seqlen( pstr );
			prev = pstr;
			
			VARS( "my pstr", "%s", pstr );
			
			if( !( flags & REGEX_MOD_GLOBAL ) )
				break;
		}
		else
			pstr += u8_seqlen( pstr );
	}
	
	if( refs_cnt )
		pfree( refs );
	pregex_nfa_free( &nfa );

	if( prev != pstr && !( *result = pstr_append_str( *result, prev, FALSE ) ) )
		RETURN( ERR_MEM );

	RETURN( matches );
}

