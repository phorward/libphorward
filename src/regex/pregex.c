/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	pregex.c
Author:	Jan Max Meyer
Usage:	The pregex object functions.
----------------------------------------------------------------------------- */

/*
 * Includes
 */

#include <phorward.h>

#define IS_EXECUTABLE( stat )	( (stat) == PREGEX_STAT_NFA || \
									(stat) == PREGEX_STAT_DFA )
/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_create()

	Author:			Jan Max Meyer

	Usage:			Constructor function to create a new pregex object.

	Parameters:		void

	Returns:		pregex*						Returns an initialized instance
												of pregex. This can be used and
												modified with subsequent pregex
												functions.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	09.04.2012	Jan Max Meyer	Removed the function pregex_init() and replaced
								it with pregex_create(). pregex will now be
								turned into an object oriented way.
----------------------------------------------------------------------------- */
pregex* pregex_create( void )
{
	pregex*		regex;

	PROC( "pregex_create" );

	regex = (pregex*)pmalloc( sizeof( pregex ) );
	pregex_set_flags( regex, PREGEX_MOD_GLOBAL );

	RETURN( regex );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_free()

	Author:			Jan Max Meyer

	Usage:			Destructor function for a pregex-object.

	Parameters:		pregex*			regex		Pointer to a pregex-structure,
												that will be reset.

	Returns:		pregex*						Always (pregex*)NULL.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pregex* pregex_free( pregex* regex )
{
	LIST*			l;
	pregex_ptndef*	def;

	PROC( "pregex_free" );
	PARMS( "regex", "%p", regex );

	if( !( regex ) )
	{
		WRONGPARAM;
		RETURN( (pregex*)NULL );
	}

	/* Resetting all runtime parameters */
	pregex_reset( regex );

	/* Freeing temporary string */
	pfree( regex->tmp_str );

	/* Freeing the state machine */
	switch( regex->stat )
	{
		case PREGEX_STAT_NFA:
			pregex_nfa_free( &( regex->machine.nfa ) );
			break;

		case PREGEX_STAT_DFA:
			pregex_dfa_free( &( regex->machine.dfa ) );
			break;

		default:
			break;
	}

	/* Freeing the pattern definitions */
	LISTFOR( regex->defs, l )
	{
		def = (pregex_ptndef*)list_access( l );

		pregex_ptn_free( def->pattern );
		pfree( def );
	}

	regex->defs = list_free( regex->defs );

	pfree( regex );

	RETURN( (pregex*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_reset()

	Author:			Jan Max Meyer

	Usage:			Resets all runtime and ephemerial elements of a
					pregex-object, so it can be used for another regular
					expression match task.

	Parameters:		pregex*		regex			The pregex-object to be
												resetted.

	Returns:		pregex*						Returns a pointer to regex.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	09.04.2012	Jan Max Meyer	Removed the function pregex_init() and replaced
								it with pregex_create(). pregex will now be
								turned into an object oriented way.
----------------------------------------------------------------------------- */
pregex* pregex_reset( pregex* regex )
{
	PROC( "pregex_reset" );
	PARMS( "regex", "%p", regex );

	if( !( regex ) )
	{
		WRONGPARAM;
		RETURN( (pregex*)NULL );
	}

	regex->match_count = 0;
	regex->last_str = (uchar*)NULL;
	regex->last_pos = (uchar*)NULL;

	memset( &( regex->range ), 0, sizeof( pregex_range ) );
	regex->range.accept = PREGEX_ACCEPT_NONE;

	pfree( regex->refs );
	regex->refs_cnt = 0;

	RETURN( regex );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_compile()

	Author:			Jan Max Meyer

	Usage:			Compiles a regular expression into an pattern and
					NFA within a regex object.

	Parameters:		pregex*			regex		Pointer to a pregex-structure,
												that will hold the compiled
												pattern.
					uchar*			pattern		The pattern to be compiled.
					int				accept		Accepting ID if the pattern
												is correctly matched.

	Returns:		int				ERR_OK		on success
									ERR_FAILURE	if the regex is not
												initialized or already turned
												into a different state.
									ERR...		error define else

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	16.11.2011	Jan Max Meyer	Switched this function to new regular expression
								pattern definition structures.
----------------------------------------------------------------------------- */
int pregex_compile( pregex* regex, uchar* pattern, int accept )
{
	int				ret;
	pregex_ptndef*	def;

	PROC( "pregex_compile" );
	PARMS( "regex", "%p", regex );
	PARMS( "pattern", "%s", pattern );
	PARMS( "accept", "%d",accept );

	if( !( regex->stat == PREGEX_STAT_NONE
			|| regex->stat == PREGEX_STAT_NFA ) )
	{
		MSG( "The regex is not inizialized or already turned into DFA!" );
		RETURN( ERR_FAILURE );
	}

	/* Create a pattern definition */
	def = (pregex_ptndef*)pmalloc( sizeof( pregex_ptndef ) );
	def->accept.accept = accept;

	if( ( ret = pregex_ptn_parse( &( def->pattern ), &( def->accept ),
					pattern, regex->flags ) ) != ERR_OK )
	{
		pregex_ptn_free( def->pattern );
		pfree( def );

		RETURN( ret );
	}

	/* pregex_ptn_print( def->pattern, 0 ); */

	/* Conversion to NFA */
	if( regex->stat == PREGEX_STAT_NONE )
	{
		MSG( "Machine is only self-inizialized yet - inizializing NFA!" );
		memset( &( regex->machine.nfa ), 0, sizeof( pregex_nfa ) );
		regex->stat = PREGEX_STAT_NFA;
	}

	/*
		For now, we build the NFA right here.
		This behavior may change in future versions, but there is a cut
		required now to still keep the regular expressions working without
		an entire redesign.
	*/
	if( ( ret = pregex_ptn_to_nfa( &( regex->machine.nfa ),
					def->pattern, &( def->accept ) ) ) != ERR_OK )
	{
		pregex_ptn_free( def->pattern );
		pfree( def );

		RETURN( ret );
	}

	if( regex->flags & PREGEX_MOD_DEBUG )
		pregex_nfa_print( &( regex->machine.nfa ) );

	/*
		Chaining the regular expression pattern definition into
		the list of definitions
	*/
	if( !( regex->defs = list_push( regex->defs, def ) ) )
	{
		pregex_ptn_free( def->pattern );
		pfree( def );

		RETURN( ERR_MEM );
	}

	/* Increment age */
	regex->age++;

	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_finalize()

	Author:			Jan Max Meyer

	Usage:			Finalizes a pregex-compiled regex to a minimized DFA.
					After finalization, new expressions can't be added anymore
					to the object.

	Parameters:		pregex*			regex		Pointer to a pregex-structure,
												that will be finalized.

	Returns:		int				ERR_OK		on success
									ERR_FAILURE	if the regex is not in
												compiled-state
									ERR...		error define else

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Finalize NFA to DFA now activated.
----------------------------------------------------------------------------- */
int pregex_finalize( pregex* regex )
{
	pregex_dfa	dfa;
	int			ret;

	PROC( "pregex_finalize" );
	PARMS( "regex", "%p", regex );

	if( !( regex ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	if( !( regex->stat == PREGEX_STAT_NFA ) )
	{
		MSG( "The regex must be in compiled state." );
		RETURN( ERR_FAILURE );
	}

	memset( &dfa, 0, sizeof( pregex_dfa ) );

	/* Perform subset construction algorithm */
	if( ( ret = pregex_dfa_from_nfa( &dfa, &( regex->machine.nfa ) ) )
			< ERR_OK )
	{
		MSG( "Subset construction failed" );
		pregex_dfa_free( &dfa );

		RETURN( ret );
	}

	/* Perform DFA minimization */
	if( ( ret = pregex_dfa_minimize( &dfa ) ) != ERR_OK )
	{
		MSG( "DFA minimization failed" );
		pregex_dfa_free( &dfa );

		RETURN( ret );
	}

	/* Delete NFA */
	if( regex->flags & PREGEX_MOD_DEBUG )
		pregex_nfa_print( &( regex->machine.nfa ) );

	pregex_nfa_free( &( regex->machine.nfa ) );

	/* Set new regex status */
	if( regex->flags & PREGEX_MOD_DEBUG )
		pregex_dfa_print( stderr, &dfa );

	memcpy( &( regex->machine.dfa ), &dfa, sizeof( pregex_dfa ) );
	regex->stat = PREGEX_STAT_DFA;

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_match_next()

	Author:			Jan Max Meyer

	Usage:			The function pregex_match_next() is used to run a regular
					expression object on a string, to match patterns. The
					function is called as long as no more matches are be
					found.

					The function is used similar to strtok(). The first call
					requires a pointer to the string where the regular
					expression will tested against. Any subsequent calls to
					pregex_match_next() must provide a (uchar*)NULL pointer as
					string, so that the previous string position will be
					re-used.

	Parameters:		pregex*			regex		Pointer to a pre-compiled
												regex state regex.
					uchar*			str			Pointer to input string where
												the pattern will be run against.
												This shall be set at the first
												call of pregex_match() and later
												on as (uchar*)NULL.

	Returns:		pregex_range*				Returns a pointer to a pregex-
												structure describing the matched
												area in case of a successful
												match, else a pointer to
												(pregex_range*)NULL.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
	15.04.2012	Jan Max Meyer	Changed the entire function to the new concept
								using a pregex-object.
----------------------------------------------------------------------------- */
pregex_range* pregex_match_next( pregex* regex, uchar* str )
{
	int				match	= PREGEX_ACCEPT_NONE;
	int				anchors;
	psize			len;
	uchar*			pstr	= str;

	PROC( "pregex_match_next" );
	PARMS( "regex", "%p", regex );
	PARMS( "str", "%p", str );

#ifdef __WITH_TRACE
	if( str )
	{
		if( regex->flags & PREGEX_MOD_WCHAR )
			PARMS( "str", "%ls", str );
		else
			PARMS( "str", "%s", str );
	}
#endif

	if( !( regex ) )
	{
		regex->last_err = ERR_MEM;
		WRONGPARAM;
		RETURN( (pregex_range*)NULL );
	}

	if( !IS_EXECUTABLE( regex->stat ) )
	{
		regex->last_err = ERR_UNIMPL;
		MSG( "This regex-object can't be executed." );
		RETURN( (pregex_range*)NULL );
	}

	/* Reset accept structure */
	memset( &( regex->range ), 0, sizeof( pregex_range ) );
	regex->range.accept = PREGEX_ACCEPT_NONE;

	regex->last_err = ERR_OK;

	/* Is this an initial or subsequent call? */
	if( !str )
	{
		if( regex->age != regex->last_age )
		{
			regex->last_err = ERR_OTHER;

			MSG( "The regular expression object was modified during last call."
					" It must be reset first." );
			RETURN( (pregex_range*)NULL );
		}

		pstr = regex->last_pos;

		/* This pregex will not match globally */
		if( !( regex->flags & PREGEX_MOD_GLOBAL ) )
		{
			MSG( "pregex will only match globally" );
			RETURN( (pregex_range*)NULL );
		}
	}
	else
	{
		pregex_reset( regex );
		regex->last_str = str;
		regex->last_age = regex->age;
	}

	/* Search for matching patterns */
	while( pstr && *pstr )
	{
#ifdef __WITH_TRACE
		if( regex->flags & PREGEX_MOD_WCHAR )
			PARMS( "pstr", "%ls", pstr );
		else
			PARMS( "pstr", "%s", pstr );
#endif

		if( regex->stat == PREGEX_STAT_NFA )
			match = pregex_nfa_match( &( regex->machine.nfa ), pstr,
						&len, &anchors, &( regex->refs ), &( regex->refs_cnt ),
							regex->flags );
		else
			match = pregex_dfa_match( &( regex->machine.dfa ), pstr,
						&len, &anchors, &( regex->refs ), &( regex->refs_cnt ),
							regex->flags );

		if( match > PREGEX_ACCEPT_NONE &&
				pregex_check_anchors( str, pstr, len,
					anchors, regex->flags ) )
		{
			MSG( "pregex_nfa_match found a match!" );
			VARS( "match", "%d", match );
			VARS( "len", "%ld", len );

			/* Fill the match structure */
			regex->range.accept = match;
			regex->range.begin = pstr;
			regex->range.pbegin = (pchar*)pstr;

			regex->range.end = pstr + len;
			regex->range.pend = (pchar*)pstr + len;

			if( regex->flags & PREGEX_MOD_WCHAR )
				regex->range.pos = (pchar*)pstr - (pchar*)regex->last_str;
			else
				regex->range.pos = pstr - regex->last_str;

			regex->range.len = len;
			VARS( "len of result", "%ld", regex->range.len );

			if( regex->match_fn )
			{
				MSG( "Calling match-function" );
				match = (*regex->match_fn)( regex, &( regex->range ) );
			}

			VARS( "match", "%d", match );
			if( match >= 0 )
			{
				regex->range.accept = match;

				/* accept and len could have been changed by
						the callback function */
				if( regex->range.len > len )
					len = regex->range.len;

				/* Move pstr to len characters forward */
				if( regex->flags & PREGEX_MOD_WCHAR )
					pstr += len * sizeof( pchar );
				else
				{
#ifdef UTF8
					for( ; len > 0; len-- )
						pstr += u8_seqlen( pstr );
#else
					pstr += len;
#endif
				}

				regex->range.end = pstr;
#ifdef UTF8
				if( !( regex->flags & PREGEX_MOD_WCHAR ) )
					regex->range.len = pstr - regex->range.begin;
#endif

				/* Update pregex object runtime values */
				regex->last_pos = pstr;
				regex->match_count++;

				break;
			}
		}

		/* Move one character forward to try out next match */
		if( regex->flags & PREGEX_MOD_WCHAR )
			pstr += sizeof( pchar );
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}
	}

	VARS( "match", "%d", match );
	VARS( "RETURN", "%p", match >= 0 ?
			&( regex->range ) : (pregex_range*)NULL );

	RETURN( match >= 0 ? &( regex->range ) : (pregex_range*)NULL  );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_match()

	Author:			Jan Max Meyer

	Usage:			Runs a regular expression match on a string as long as
					matches are found. All matches will be collected into
					return array results, if a pointer is provided.

	Parameters:		pregex*			regex		The regular expression
												object pointer.
					uchar*			str			Searchstring the pattern
												will be ran on.
					pregex_range**	results		Return pointer for an array of
												ranges to the matched substrings
												within str.
												The pointer ranges must be freed
												after usage. The parameter
												can be left (pregex_range**)
												NULL, so only the number of
												matches will be returned by the
												function.

	Returns:		int							Returns the total number of
												matches. If the value is
												negative, it is an error
												code. The returned number
												is the number of elements in
												the results-array.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	25.04.2012	Jan Max Meyer	Changed the function to work with the new
								pregex-object structures.
----------------------------------------------------------------------------- */
int pregex_match( pregex* regex, uchar* str, pregex_range** results )
{
	pregex_range*	range;
	int				matches		= 0;

	PROC( "pregex_match" );
	PARMS( "regex", "%p", regex );
	PARMS( "str", "%s", pgetstr( str ) );
	PARMS( "results", "%p", results );

	if( !( regex && str ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	if( results )
		*results = (pregex_range*)NULL;

	for( range = pregex_match_next( regex, str ); range;
			range = pregex_match_next( regex, (uchar*)NULL ), matches++ )
	{
		if( results )
		{
			if( ( matches % PREGEX_ALLOC_STEP ) == 0 )
			{
				*results = (pregex_range*)prealloc( *results,
							( regex->match_count + PREGEX_ALLOC_STEP ) *
								sizeof( pregex_range ) );

				if( ! *results )
				{
					OUTOFMEM;
					RETURN( ERR_MEM );
				}
			}

			memcpy( &( ( *results )[ matches ] ),
				range, sizeof( pregex_range ) );
		}
	}

	/* If an error occured, free results array and return error code */
	if( !range && regex->last_err < ERR_OK )
	{
		if( results );
			*results = pfree( *results );
		RETURN( regex->last_err );
	}

	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_split_next()

	Author:			Jan Max Meyer

	Usage:			The function pregex_split_next() is used to run a regular
					expression object on a string, and split this string
					when a pattern is matched. The function is called as long
					as no more matches are be found.

					The function is used similar to strtok(). The first call
					requires a pointer to the string where the regular
					expression will tested against. Any subsequent calls to
					pregex_split_next() must provide a (uchar*)NULL pointer as
					string, so that the previous string position will be
					re-used.

	Parameters:		pregex*			regex		Pointer to a pre-compiled
												regex state regex.
					uchar*			str			Pointer to input string where
												the pattern will be run against.
												This shall be set at the first
												call of pregex_match() and later
												on as (uchar*)NULL.

	Returns:		pregex_range*				Returns a pointer to a pregex-
												structure describing the
												splitted area in case of a
												successful match, else a pointer
												to (pregex_range*)NULL if no
												more splits are found or an
												error occured.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
	15.04.2012	Jan Max Meyer	Changed the entire function to the new concept
								using a pregex-object.
----------------------------------------------------------------------------- */
pregex_range* pregex_split_next( pregex* regex, uchar* str )
{
	uchar*			last;
	pregex_range	range;
	pregex_range*	match;

	PROC( "pregex_split_next" );
	PARMS( "regex", "%p", regex );
	PARMS( "str", "%p", str );

#ifdef __WITH_TRACE
	if( str )
	{
		if( regex->flags & PREGEX_MOD_WCHAR )
			PARMS( "str", "%ls", str );
		else
			PARMS( "str", "%s", str );
	}
#endif

	if( !( regex ) )
	{
		regex->last_err = ERR_PARMS;
		WRONGPARAM;
		RETURN( (pregex_range*)NULL );
	}

	if( !IS_EXECUTABLE( regex->stat ) )
	{
		regex->last_err = ERR_UNIMPL;
		MSG( "This regex-object can't be executed." );
		RETURN( (pregex_range*)NULL );
	}

	/* Reset accept structure */
	memset( &( regex->split ), 0, sizeof( pregex_range ) );

	/* Is this an initial or a subsequent call? */
	if( !str )
	{
		if( regex->age != regex->last_age )
		{
			regex->last_err = ERR_OTHER;

			MSG( "The regular expression object was modified during last call."
					" It must be reset first." );
			RETURN( (pregex_range*)NULL );
		}

		if( !( regex->flags & PREGEX_MOD_GLOBAL ) )
		{
			regex->last_err = ERR_FAILURE;

			MSG( "pregex will only match globally" );
			RETURN( (pregex_range*)NULL );
		}
	}

	if( !( last = regex->last_pos ) )
		last = str;

	if( ( match = pregex_match_next( regex, str ) ) )
	{
		VARS( "match->accept", "%d", match->accept );

		/* We dont't have any match here! */
		regex->split.accept = PREGEX_ACCEPT_NONE;

		regex->split.begin = last;
		regex->split.pbegin = (pchar*)last;

		regex->split.end = match->begin;
		regex->split.pend = match->pbegin;

		if( regex->flags & PREGEX_MOD_WCHAR )
		{
			regex->split.pos = regex->split.pbegin
									- (pchar*)regex->last_str;
			regex->split.len = (pchar*)regex->split.end
									- (pchar*)regex->split.begin;
		}
		else
		{
			regex->split.pos = regex->split.pbegin - (pchar*)regex->last_str;
			regex->split.len = regex->split.end - regex->split.begin;
		}

		RETURN( &( regex->split ) );
	}

	/* Put last one if required! */
	if( *last )
	{
		/* We dont't have any match here! */
		regex->split.accept = PREGEX_ACCEPT_NONE;

		regex->split.begin = last;
		regex->split.pbegin = (pchar*)last;

		while( *last )
		{
			/* Move one character forward to try out next match */
			if( regex->flags & PREGEX_MOD_WCHAR )
				last += sizeof( pchar );
			else
			{
#ifdef UTF8
				last += u8_seqlen( last );
#else
				last++;
#endif
			}
		}

		regex->split.end = last;
		regex->split.pend = (pchar*)last;

		if( regex->flags & PREGEX_MOD_WCHAR )
		{
			regex->split.pos = regex->split.pbegin
									- (pchar*)regex->last_str;
			regex->split.len = (pchar*)regex->split.end
									- (pchar*)regex->split.begin;
		}
		else
		{
			regex->split.pos = regex->split.pbegin - (pchar*)regex->last_str;
			regex->split.len = regex->split.end - regex->split.begin;
		}

		regex->last_pos = last;
		RETURN( &( regex->split ) );
	}

	MSG( "No more matches for split" );
	RETURN( (pregex_range*)NULL );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_split()

	Author:			Jan Max Meyer

	Usage:			Runs a regular expression split on a string as long as
					the string can be splitted at the given compiled pattern.
					All matches will be collected into return array results,
					if a pointer is provided.

	Parameters:		pregex*			regex		The regular expression
												object pointer.
					uchar*			str			Searchstring the pattern
												will be ran on.
					pregex_range**	results		Return pointer for an array of
												ranges to the split substrings
												within str.
												The pointer ranges must be freed
												after usage. The parameter
												can be left (pregex_range**)
												NULL, so only the number of
												splits will be returned by the
												function.

	Returns:		int							Returns the total number of
												splits. If the value is
												negative, it is an error
												code. The returned number
												is the number of elements in
												the results-array.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	25.04.2012	Jan Max Meyer	Changed the function to work with the new
								pregex-object structures.
----------------------------------------------------------------------------- */
int pregex_split( pregex* regex, uchar* str, pregex_range** results )
{
	int				matches	= 0;
	pregex*			re;
	pregex_range*	range;

	PROC( "pregex_split" );
	PARMS( "regex", "%p", regex );
	PARMS( "str", "%s", pgetstr( str ) );
	PARMS( "results", "%p", results );

	if( !( regex && str ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	if( results )
		*results = (pregex_range*)NULL;

	for( range = pregex_split_next( regex, str ); range;
			range = pregex_split_next( regex, (uchar*)NULL ), matches++ )
	{
		if( results )
		{
			if( ( matches % PREGEX_ALLOC_STEP ) == 0 )
			{
				*results = (pregex_range*)prealloc( *results,
							( matches + PREGEX_ALLOC_STEP ) *
								sizeof( pregex_range ) );

				if( ! *results )
				{
					OUTOFMEM;
					RETURN( ERR_MEM );
				}
			}

			memcpy( &( ( *results )[ matches ] ),
				range, sizeof( pregex_range ) );
		}
	}

	/* If an error occured, free results array and return error code */
	if( !range && regex->last_err < ERR_OK )
	{
		if( results );
			*results = pfree( *results );
		RETURN( regex->last_err );
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

	Parameters:		pregex*			regex		The regular expression
												pattern
					uchar*			str			String the pattern
												will be ran on.
					uchar*			replacement	String that will be inserted
												as replacement for each pattern
												match. $x backreferences
												can be used.
	Returns:		int							Returns the amount of matches.
												If the value is negative,
												it is an error define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
	25.04.2012	Jan Max Meyer	Changed the function to work with the new
								pregex-object usability and behaviors.
----------------------------------------------------------------------------- */
uchar* pregex_replace( pregex* regex, uchar* str, uchar* replacement )
{
	pregex_range*	match;
	pregex_range*	reference;
	int				matches		= 0;
	int				charsize	= sizeof( uchar );
	int				ref;
	int				anchors;
	psize			len;
	uchar*			pstr;
	uchar*			start;
	uchar*			end;
	uchar*			rpstr;
	uchar*			rbegin;
	uchar*			rprev;
	uchar*			replace;
	uchar*			use_replacement;

	PROC( "pregex_replace" );
	PARMS( "regex", "%p", regex );

#ifdef __WITH_TRACE
	if( regex && regex->flags & PREGEX_MOD_WCHAR )
	{
		PARMS( "str", "%ls", pgetstr( str ) );
		PARMS( "replacement", "%ls", pgetstr( replacement ) );
	}
	else
	{
		PARMS( "str", "%s", pgetstr( str ) );
		PARMS( "replacement", "%s", pgetstr( replacement ) );
	}
#endif

	if( !( str ) )
	{
		regex->last_err = ERR_PARMS;
		WRONGPARAM;
		RETURN( (uchar*)NULL );
	}

	if( regex->flags & PREGEX_MOD_WCHAR )
		charsize = sizeof( pchar );

	regex->tmp_str = pfree( regex->tmp_str );

	for( start = str, match = pregex_match_next( regex, str );
			/* no condition in here is correct! */ ;
			match = pregex_match_next( regex, (uchar*)NULL ) )
	{
#ifdef __WITH_TRACE
		if( regex->tmp_str )
		{
			if( regex->flags & PREGEX_MOD_WCHAR )
				VARS( "regex->tmp_str", "%ls", regex->tmp_str );
			else
				VARS( "regex->tmp_str", "%s", regex->tmp_str );
		}
#endif

		if( match )
			end = match->begin;
		else
		{
			if( regex->flags & PREGEX_MOD_WCHAR )
				end = (uchar*)( (pchar*)start + Pstrlen( (pchar*)start ) );
			else
				end = start + pstrlen( start );
		}

#ifdef __WITH_TRACE
		PARMS( "start", "%p", start );
		PARMS( "end", "%p", end );

		if( regex->flags & PREGEX_MOD_WCHAR )
		{
			PARMS( "start", "%ls", start );
			PARMS( "end", "%ls", end );
		}
		else
		{
			PARMS( "start", "%s", start );
			PARMS( "end", "%s", end );
		}
#endif

		if( start < end )
		{
			MSG( "Extending string" );
			if( regex->flags & PREGEX_MOD_WCHAR )
			{
				if( !( regex->tmp_str = (uchar*)Pstrncatstr(
						(pchar*)regex->tmp_str, (pchar*)start,
							( (pchar*)end - (pchar*)start ) ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (uchar*)NULL );
				}
			}
			else
			{
				if( !( regex->tmp_str = pstrncatstr(
						regex->tmp_str, start,
							end - start ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (uchar*)NULL );
				}
			}
		}

		if( !match )
			break;

		if( match->user )
			use_replacement = match->user;
		else
			use_replacement = replacement;

		if( regex->flags & PREGEX_MOD_NO_REF )
		{
			MSG( "No references wanted by caller" );
			replace = use_replacement;
		}
		else
		{
			MSG( "Constructing replacement" );
			for( rprev = rpstr = use_replacement, replace = (uchar*)NULL;
					*rpstr; )
			{
				VARS( "*rpstr", "%c", *rpstr );
				if( *rpstr == '$' )
				{
					rbegin = rpstr;

					if( regex->flags & PREGEX_MOD_WCHAR )
					{
						pchar*		end;
						pchar*		_rpstr = (pchar*)rpstr;

						MSG( "Switching to wide-character mode" );

						if( Pisdigit( *( ++_rpstr ) ) )
						{
							ref = Pstrtol( _rpstr, &end, 0 );

							VARS( "ref", "%d", ref );
							VARS( "end", "%ls", end );

							/* Skip length of the number */
							_rpstr = end;

							/* Extend first from prev of replacement */
							if( !( replace = (uchar*)Pstrncatstr(
									(pchar*)replace, (pchar*)rprev,
										(pchar*)rbegin - (pchar*)rprev ) ) )
							{
								regex->last_err = ERR_MEM;
								OUTOFMEM;
								RETURN( (uchar*)NULL );
							}

							VARS( "replace", "%ls", replace );
							VARS( "ref", "%d", ref );

							if( ( reference = pregex_get_ref( regex, ref ) ) )
							{
								MSG( "There is a reference!" );
								VARS( "reference->pbegin", "%ls",
										reference->pbegin );
								VARS( "len", "%d", reference->len );

								if( !( replace = (uchar*)Pstrncatstr(
										(pchar*)replace, reference->pbegin,
											reference->len ) ) )
								{
									regex->last_err = ERR_MEM;
									OUTOFMEM;
									RETURN( (uchar*)NULL );
								}
							}

							VARS( "replace", "%ls", (pchar*)replace );
							rprev = rpstr = (uchar*)_rpstr;
						}
						else
							rpstr = (uchar*)_rpstr;
					}
					else
					{
						uchar*		end;

						MSG( "Byte-character mode (Standard)" );

						if( pisdigit( *( ++rpstr ) ) )
						{
							ref = pstrtol( rpstr, &end, 0 );

							VARS( "ref", "%d", ref );
							VARS( "end", "%s", end );

							/* Skip length of the number */
							rpstr = end;

							/* Extend first from prev of replacement */
							if( !( replace = pstrncatstr( replace,
										rprev, rbegin - rprev ) ) )
							{
								regex->last_err = ERR_MEM;
								OUTOFMEM;
								RETURN( (uchar*)NULL );
							}

							VARS( "replace", "%s", replace );
							VARS( "ref", "%d", ref );

							/* Obtain reference information */
							if( ( reference = pregex_get_ref( regex, ref ) ) )
							{
								MSG( "There is a reference!" );
								VARS( "reference->pbegin", "%s",
										reference->begin );
								VARS( "len", "%d", reference->len );

								if( !( replace = pstrncatstr(
										replace, reference->begin,
											reference->len ) ) )
								{
									regex->last_err = ERR_MEM;
									OUTOFMEM;
									RETURN( (uchar*)NULL );
								}
							}

							VARS( "replace", "%s", replace );
							rprev = rpstr;
						}
					}
				}
				else
					rpstr += charsize;
			}

			VARS( "rpstr", "%p", rpstr );
			VARS( "rprev", "%p", rprev );

#ifdef __WITH_TRACE
			if( regex->flags & PREGEX_MOD_WCHAR )
			{
				VARS( "rpstr", "%ls", replace );
				VARS( "rprev", "%ls", rprev );
			}
			else
			{
				VARS( "rpstr", "%s", replace );
				VARS( "rprev", "%s", rprev );
			}
#endif

			if( regex->flags & PREGEX_MOD_WCHAR )
			{
				if( rpstr != rprev &&
						!( replace = (uchar*)Pstrcatstr(
								(pchar*)replace, (pchar*)rprev, FALSE ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (uchar*)NULL );
				}
			}
			else
			{
				if( rpstr != rprev && !( replace = pstrcatstr(
											replace, rprev, FALSE ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (uchar*)NULL );
				}
			}
		}

		if( replace )
		{
#ifdef __WITH_TRACE
			if( regex->flags & PREGEX_MOD_WCHAR )
				VARS( "replace", "%ls", replace );
			else
				VARS( "replace", "%s", replace );
#endif
			if( regex->flags & PREGEX_MOD_WCHAR )
			{
				if( !( regex->tmp_str = (uchar*)Pstrncatstr(
						(pchar*)regex->tmp_str, (pchar*)replace,
							Pstrlen( (pchar*)replace ) ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (uchar*)NULL );
				}
			}
			else
			{
				if( !( regex->tmp_str = pstrncatstr(
						regex->tmp_str, replace,
							pstrlen( replace ) ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (uchar*)NULL );
				}
			}

			if( !( regex->flags & PREGEX_MOD_NO_REF ) )
				pfree( replace );
		}

		start = match->end;
	}

#ifdef __WITH_TRACE
	if( regex->flags & PREGEX_MOD_WCHAR )
		VARS( "regex->tmp_str", "%ls", regex->tmp_str );
	else
		VARS( "regex->tmp_str", "%s", regex->tmp_str );
#endif
	RETURN( regex->tmp_str );
}

/*
 * Get- and Set-Functions
 */

/* GET ONLY! */
pregex_range* pregex_get_range( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return (pregex_range*)NULL;
	}

	/* range will only be returned when it is a match. */
	if( regex->range.accept > PREGEX_ACCEPT_NONE )
		return &regex->range;

	return (pregex_range*)NULL;
}

/* GET ONLY! */
pregex_range* pregex_get_split( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return (pregex_range*)NULL;
	}

	/* split will only be returned if it is NOT a match!! */
	if( regex->split.accept == PREGEX_ACCEPT_NONE )
		return &regex->split;

	return (pregex_range*)NULL;
}

/* GET ONLY! */
pregex_range* pregex_get_ref( pregex* regex, int offset )
{
	if( !( regex && offset >= 0 ) )
	{
		WRONGPARAM;
		return (pregex_range*)NULL;
	}

	/* ref will only be returned when it is a match. */
	if( regex->range.accept > PREGEX_ACCEPT_NONE
			&& offset < regex->refs_cnt )
	{
		/* test reference for validity - some references are incomplete,
			and therefore invalid. */
		if( regex->refs[ offset ].begin && regex->refs[ offset ].end )
			return &( regex->refs[ offset ] );
	}

	return (pregex_range*)NULL;
}

/* GET ONLY! */
int pregex_get_match_count( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return 0;
	}

	return regex->match_count;
}

int pregex_get_flags( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return PREGEX_MOD_NONE;
	}

	return regex->flags;
}

BOOLEAN pregex_set_flags( pregex* regex, int flags )
{
	if( !( regex && flags >= 0 ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( pregex_get_flags( regex ) == flags )
		return FALSE;

	regex->flags = flags;
	return TRUE;
}

pregex_fn pregex_get_match_fn( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return PREGEX_MOD_NONE;
	}

	return regex->match_fn;
}

BOOLEAN pregex_set_match_fn( pregex* regex, pregex_fn match_fn )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	regex->match_fn = match_fn;
	return TRUE;
}
