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

	/*
	pregex_nfa_print( &( regex->machine.nfa ) );
	*/

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

	/* This will be enabled later! */

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
	/*
	pregex_nfa_print( &( regex->machine.nfa ) );
	*/
	pregex_nfa_free( &( regex->machine.nfa ) );

	/* Set new regex status */
	/*
	pregex_dfa_print( stderr, &dfa );
	*/
	memcpy( &( regex->machine.dfa ), &dfa, sizeof( pregex_dfa ) );
	regex->stat = PREGEX_STAT_DFA;

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_match()

	Author:			Jan Max Meyer

	Usage:			Performs a regular expression search with a pre-compiled
					regex on a string, and returns an array of matches via a
					pregex_result-structure, which holds pointers to the begin-
					and end-addresses of the matches.

	Parameters:		pregex*			regex		Pointer to a pre-compiled
												regex state regex.
					uchar*			str			Pointer to input string where
												the pattern will be executed on.
					pregex_fn		fn			A callback-function which
												is called for each match.
												The returned value is set for
												the match within the results
												array, if it is greater or equal
												to 0. If the return value is
												negative, the match will be
												ignored. Use the macro
												PREGEX_FN_NULL to disable a
												callback-function usage.
					pregex_result**	results		Array of results to the
												matched substrings within
												str. results must be freed
												after usage. It will even hold
												the accepting IDs of the
												matching expressions.

	Returns:		int							Returns the amount of matches,
												which is the amount of items
												within the returned
												results-array. If the
												value is negative,
												it is an error define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
----------------------------------------------------------------------------- */
int pregex_match( pregex* regex, uchar* str, pregex_fn fn,
							pregex_result** results )
{
	int				match;
	int				matches	= 0;
	int				anchors;
	psize			len;
	uchar*			pstr;

	PROC( "pregex_match" );
	PARMS( "regex", "%p", regex );

#ifdef __WITH_TRACE
	if( regex->flags & PREGEX_MOD_WCHAR )
		PARMS( "str", "%ls", str );
	else
		PARMS( "str", "%s", str );
#endif

	PARMS( "fn", "%p", fn );
	PARMS( "results", "%p", results );

	if( !IS_EXECUTABLE( regex->stat ) )
		RETURN( ERR_UNIMPL );

	if( results )
		*results = (pregex_result*)NULL;

	for( pstr = str; *pstr; )
	{
		if( regex->flags & PREGEX_MOD_WCHAR )
			PARMS( "pstr", "%ls", pstr );
		else
			PARMS( "pstr", "%s", pstr );

		if( regex->stat == PREGEX_STAT_NFA )
			match = pregex_nfa_match( &( regex->machine.nfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							regex->flags );
		else
			match = pregex_dfa_match( &( regex->machine.dfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							regex->flags );

		if( match >= 0 && pregex_check_anchors( str, pstr, len,
							anchors, regex->flags ) )
		{
			MSG( "pregex_nfa_match found a match!" );
			VARS( "match", "%d", match );
			VARS( "len", "%ld", len );

			if( results || fn )
			{
				memset( &( regex->last_match ), 0, sizeof( pregex_result ) );

				regex->last_match.accept = match;
				regex->last_match.begin = pstr;
				regex->last_match.end = pstr + len;
				regex->last_match.pbegin = (pchar*)pstr;
				regex->last_match.pend = (pchar*)pstr + len;

				if( regex->flags & PREGEX_MOD_WCHAR )
					regex->last_match.pos = (pchar*)pstr - (pchar*)str;
				else
					regex->last_match.pos = pstr - str;

				regex->last_match.len = len;
				VARS( "len of result", "%ld", regex->last_match.len );
			}

			if( fn )
			{
				MSG( "Calling callback-function" );
				match = (*fn)( regex, &( regex->last_match ) );
			}

			VARS( "match", "%d", match );
			if( match >= 0 )
			{
				if( results )
				{
					regex->last_match.accept = match;

					/* len could have be changed by the callback function */
					if( fn && regex->last_match.len > len )
						len = regex->last_match.len;

					if( !matches )
						*results = (pregex_result*)pmalloc( PREGEX_ALLOC_STEP
										* sizeof( pregex_result ) );
					else if( !( matches % PREGEX_ALLOC_STEP ) )
						*results = (pregex_result*)prealloc(
										(pregex_result*)*results,
										( ( ( matches / PREGEX_ALLOC_STEP ) + 1 )
											* PREGEX_ALLOC_STEP )
												*  sizeof( pregex_result ) );

					if( !*results )
						RETURN( ERR_MEM );

					memcpy( *results + matches, &( regex->last_match ),
								sizeof( pregex_result ) );
				}

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

				if( results )
				{
					(*results)[ matches ].end = pstr;
#ifdef UTF8
					if( !( regex->flags & PREGEX_MOD_WCHAR ) )
						(*results)[ matches ].len =
								pstr - (*results)[ matches ].begin;
#endif
				}

				matches++;

				if( !( regex->flags & PREGEX_MOD_GLOBAL ) )
					break;

				continue;
			}
		}

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

	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_split()

	Author:			Jan Max Meyer

	Usage:			Performs a regular expression with a pre-compiled regex
					on a string and uses the expression as separator; All
					strings that where split are returned as results-array.


	Parameters:		pregex*			regex		Pointer to a pre-compiled
												regex state regex.
					uchar*			str			Searchstring the pattern
												will be ran on.
					pregex_fn		fn			A callback-function which
												is called for each split match.
												If the return value is negative,
												the match will be ignored.
												Use the macro PREGEX_FN_NULL
												to disable a callback-function
												usage.
					pregex_result**	results		Array of results to the
												split substrings. Each element
												of this array contains begin-
												and end-pointer to the
												related strings within the
												input-string str.

	Returns:		int							Returns the amount of matches,
												which is the amount of items
												within the returned
												results-array. If the
												value is negative,
												it is an error define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
----------------------------------------------------------------------------- */
int pregex_split( pregex* regex, uchar* str, pregex_fn fn,
							pregex_result** results )
{
	int				match;
	int				matches	= 0;
	int				anchors;
	psize			len;
	uchar*			pstr;
	uchar*			prev;

	PROC( "pregex_split" );
	PARMS( "regex", "%p", regex );
#ifdef __WITH_TRACE
	if( regex->flags & PREGEX_MOD_WCHAR )
		PARMS( "str", "%ls", str );
	else
		PARMS( "str", "%s", str );
#endif
	PARMS( "fn", "%p", fn );
	PARMS( "results", "%p", results );

	if( !IS_EXECUTABLE( regex->stat ) )
		RETURN( ERR_UNIMPL );

	*results = (pregex_result*)NULL;

	for( prev = pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );

		if( regex->stat == PREGEX_STAT_NFA )
			match = pregex_nfa_match( &( regex->machine.nfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							regex->flags );
		else
			match = pregex_dfa_match( &( regex->machine.dfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							regex->flags );

		if( match >= 0 && pregex_check_anchors( str, pstr, len,
							anchors, regex->flags ) )
		{
			MSG( "Write information into temporary result structure" );

			if( fn )
			{
				memset( &( regex->last_match ), 0, sizeof( pregex_result ) );

				regex->last_match.accept = match;
				regex->last_match.begin = pstr;
				regex->last_match.end = pstr + len;
				regex->last_match.pbegin = (pchar*)pstr;
				regex->last_match.pend = (pchar*)pstr + len;
				regex->last_match.len = len;

				if( regex->flags & PREGEX_MOD_WCHAR )
					regex->last_match.pos = (pchar*)pstr - (pchar*)str;
				else
					regex->last_match.pos = pstr - str;

				MSG( "Calling callback-function" );
				match = (*fn)( regex, &( regex->last_match ) );
			}

			VARS( "match", "%d", match );
			if( match >= 0 )
			{
				/* len could have be changed by the callback function */
				if( fn && regex->last_match.len > len )
					len = regex->last_match.len;

				if( !matches )
					*results = (pregex_result*)pmalloc(
									PREGEX_ALLOC_STEP *
										sizeof( pregex_result ) );
				else if( !( matches % PREGEX_ALLOC_STEP ) )
					*results = (pregex_result*)prealloc(
									(pregex_result*)*results,
										( ( ( matches / PREGEX_ALLOC_STEP ) + 1 )
											* PREGEX_ALLOC_STEP )
												*  sizeof( pregex_result ) );
				if( !*results )
					RETURN( ERR_MEM );

				(*results)[ matches ].accept = match;
				(*results)[ matches ].begin = prev;
				(*results)[ matches ].end = pstr;
				(*results)[ matches ].pbegin = (pchar*)prev;
				(*results)[ matches ].pend = (pchar*)pstr;
				if( regex->flags & PREGEX_MOD_WCHAR )
				{
					(*results)[ matches ].pos = (pchar*)prev - (pchar*)str;
					(*results)[ matches ].len = (pchar*)pstr - (pchar*)prev;
				}
				else
				{
					(*results)[ matches ].pos = prev - str;
					(*results)[ matches ].len = pstr - prev;
				}
				(*results)[ matches ].user = regex->last_match.user;

				matches++;

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
				prev = pstr;

				if( !( regex->flags & PREGEX_MOD_GLOBAL ) )
				{
					pstr += pstrlen( pstr );
					break;
				}

				continue;
			}
		}

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

	/* Put last one if required! */
	if( prev != pstr )
	{
		if( !matches )
			*results = (pregex_result*)pmalloc(
							PREGEX_ALLOC_STEP * sizeof( pregex_result ) );
		else if( !( matches % PREGEX_ALLOC_STEP ) )
			*results = (pregex_result*)prealloc(
							(pregex_result*)*results,
								( ( ( matches / PREGEX_ALLOC_STEP ) + 1 )
									* PREGEX_ALLOC_STEP )
										*  sizeof( pregex_result ) );

		if( !*results )
			RETURN( ERR_MEM );

		(*results)[ matches ].accept = -1;
		(*results)[ matches ].begin = prev;
		(*results)[ matches ].end = pstr;
		(*results)[ matches ].pbegin = (pchar*)prev;
		(*results)[ matches ].pend = (pchar*)pstr;

		if( regex->flags & PREGEX_MOD_WCHAR )
		{
			(*results)[ matches ].pos = (pchar*)prev - (pchar*)str;
			(*results)[ matches ].len = (pchar*)pstr - (pchar*)prev;
		}
		else
		{
			(*results)[ matches ].pos = prev - str;
			(*results)[ matches ].len = pstr - prev;
		}

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

	Parameters:		pregex*			regex		The regular expression
												pattern
					uchar*			str			String the pattern
												will be ran on.
					uchar*			replacement	String that will be inserted
												as replacement for each pattern
												match. $x backreferences
												can be used
					pregex_fn	fn			An optional callback function.
												If the return value is negative,
												it ignores the current match.
												If there is an alternative
												replacement string wanted, put
												a string pointer in the member
												variable 'user' of the
												pregex_result-object provided.
												The alternative replacement
												string can contain reference
												wildcards, if not disabled.
												Use the macro PREGEX_FN_NULL
												to hand over an empty callback
												function.
					uchar**			result		Returns a pointer to the result
												string. This must be freed by
												the caller after its use.

	Returns:		int							Returns the amount of matches.
												If the value is negative,
												it is an error define.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
----------------------------------------------------------------------------- */
int pregex_replace( pregex* regex, uchar* str, uchar* replacement,
							pregex_fn fn, uchar** result )
{
	pregex_result*	refs		= (pregex_result*)NULL;
	int				refs_cnt	= 0;
	int				matches		= 0;
	int				match;
	int				charsize	= sizeof( uchar );
	int				ref;
	int				anchors;
	psize			len;
	uchar*			pstr;
	uchar*			prev;
	uchar*			rpstr;
	uchar*			rbegin;
	uchar*			rprev;
	uchar*			replace;
	uchar*			use_replacement;

	PROC( "pregex_replace" );
	PARMS( "regex", "%p", regex );
	PARMS( "str", "%s", pgetstr( str ) );
	PARMS( "replacement", "%s", pgetstr( replacement ) );
	PARMS( "result", "%p", result );
	PARMS( "fn", "%p", fn );

	if( !( str ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	if( !IS_EXECUTABLE( regex->stat ) )
		RETURN( ERR_UNIMPL );

	*result = (uchar*)NULL;

	if( regex->flags & PREGEX_MOD_WCHAR )
		charsize = sizeof( pchar );

	for( prev = pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );

		if( regex->stat == PREGEX_STAT_NFA )
			match = pregex_nfa_match( &( regex->machine.nfa ), pstr,
						&len, &anchors,
							( ( regex->flags & PREGEX_MOD_NO_REF ) ?
								(pregex_result**)NULL : &refs ), &refs_cnt,
									regex->flags );
		else
			match = pregex_dfa_match( &( regex->machine.dfa ), pstr,
						&len, &anchors,
							( ( regex->flags & PREGEX_MOD_NO_REF ) ?
								(pregex_result**)NULL : &refs ), &refs_cnt,
									regex->flags );

		if( match >= 0 && pregex_check_anchors( str, pstr, len,
											anchors, regex->flags ) )
		{
			use_replacement = (uchar*)NULL;

			if( fn )
			{
				MSG( "Prepare pregex_result-structure for callback" );

				memset( &( regex->last_match ), 0, sizeof( pregex_result ) );
				regex->last_match.accept = match;
				regex->last_match.begin = pstr;
				regex->last_match.end = pstr + len;
				regex->last_match.pbegin = (pchar*)pstr;
				regex->last_match.pend = (pchar*)pstr + len;
				regex->last_match.len = len;

				if( regex->flags & PREGEX_MOD_WCHAR )
					regex->last_match.pos = (pchar*)pstr - (pchar*)str;
				else
					regex->last_match.pos = pstr - str;

				MSG( "Calling callback-function" );
				if( (*fn)( regex, &( regex->last_match ) ) < 0 )
				{
					MSG( "Callback function returns negative value, will"
							"discard this match" );
#ifdef UTF8
					for( ; len > 0; len-- )
						pstr += u8_seqlen( pstr );
#else
					pstr += len * charsize;
#endif
					continue;
				}

				/* An alternative replacement string can be saved into the
					regex_result-structure's user-pointer */
				use_replacement = (uchar*)regex->last_match.user;
			}

			matches++;

			if( !use_replacement )
				use_replacement = replacement;

			if( regex->flags & PREGEX_MOD_NO_REF )
			{
				MSG( "No references wanted by caller" );
				replace = use_replacement;
			}
			else
			{
				VARS( "refs_cnt", "%d", refs_cnt );
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
									RETURN( ERR_MEM );

								VARS( "replace", "%ls", (pchar*)replace );

								VARS( "refs[ ref ].pbegin", "%p",
										refs[ ref ].pbegin );
								VARS( "refs[ ref ].pend", "%p",
										refs[ ref ].pend );

								VARS( "ref", "%d", ref );

								/* Obtain reference information */
								if( refs[ ref ].pbegin && refs[ ref ].pend )
								{
									MSG( "There is a reference!" );
									VARS( "refs[ ref ].pbegin", "%s",
											refs[ ref ].pbegin );
									VARS( "len", "%d",
										refs[ ref ].pend - refs[ ref ].pbegin );

									if( !( replace = (uchar*)Pstrncatstr(
											(pchar*)replace, refs[ ref ].pbegin,
												refs[ ref ].len ) ) )
										RETURN( ERR_MEM );
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
									RETURN( ERR_MEM );

								VARS( "replace", "%s", replace );

								VARS( "refs[ ref ].begin", "%p",
										refs[ ref ].begin );
								VARS( "refs[ ref ].end", "%p",
										refs[ ref ].end );

								VARS( "ref", "%d", ref );

								/* Obtain reference information */
								if( refs[ ref ].begin && refs[ ref ].end )
								{
									MSG( "There is a reference!" );
									VARS( "refs[ ref ].begin", "%s",
											refs[ ref ].begin );
									VARS( "len", "%d",
										refs[ ref ].end - refs[ ref ].begin );

									if( !( replace = pstrncatstr(
										replace, refs[ ref ].begin,
											refs[ ref ].len ) ) )
										RETURN( ERR_MEM );
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

				if( regex->flags & PREGEX_MOD_WCHAR )
				{
					if( rpstr != rprev &&
							!( replace = (uchar*)Pstrcatstr(
									(pchar*)replace, (pchar*)rprev, FALSE ) ) )
						RETURN( ERR_MEM );
				}
				else
				{
					if( rpstr != rprev && !( replace = pstrcatstr(
												replace, rprev, FALSE ) ) )
						RETURN( ERR_MEM );
				}
			}

			MSG( "Extend result string" );

			if( regex->flags & PREGEX_MOD_WCHAR )
			{
				MSG( "Switching to wide-character mode" );
				VARS( "replace", "%ls", replace );

				/* Extend result */
				if( !( *result = (uchar*)Pstrncatstr(
						(pchar*)*result, (pchar*)prev,
							(pchar*)pstr - (pchar*)prev ) ) )
					RETURN( ERR_MEM );

				if( !( *result = (uchar*)Pstrcatstr( (pchar*)*result,
						(pchar*)replace,( ( replace == use_replacement )
								? FALSE : TRUE ) ) ) )
					RETURN( ERR_MEM );

				VARS( "len", "%d", len );
				VARS( "pstr", "%ls", (pchar*)pstr );

				pstr += len * charsize;
				prev = pstr;

				VARS( "my pstr", "%ls", (pchar*)pstr );
			}
			else
			{
				MSG( "Switching to byte-character mode" );
				VARS( "replace", "%s", replace );

				/* Extend result */
				if( !( *result = pstrncatstr(
						*result, prev, pstr - prev ) ) )
					RETURN( ERR_MEM );

				if( !( *result = pstrcatstr( *result, replace,
						( ( replace == use_replacement ) ? FALSE : TRUE ) ) ) )
					RETURN( ERR_MEM );

				VARS( "len", "%d", len );
				VARS( "pstr", "%s", pstr );

#ifdef UTF8
				for( ; len > 0; len-- )
					pstr += u8_seqlen( pstr );
#else
				pstr += len;
#endif
				prev = pstr;

				VARS( "my pstr", "%s", pstr );
			}

			if( !( regex->flags & PREGEX_MOD_GLOBAL ) )
				break;
		}
		else
		{
			if( regex->flags & PREGEX_MOD_WCHAR )
				pstr += charsize;
			else
#ifdef UTF8
				pstr += u8_seqlen( pstr );
#else
				pstr += charsize;
#endif
		}
	}

	if( refs_cnt )
		pfree( refs );

	if( regex->flags & PREGEX_MOD_WCHAR )
	{
		VARS( "*result", "%s", *result );
		if( prev != pstr && !( *result = (uchar*)Pstrcatstr(
						(pchar*)*result, (pchar*)prev, FALSE ) ) )
			RETURN( ERR_MEM );

		VARS( "*result", "%s", *result );
	}
	else
	{
		if( prev != pstr && !( *result = pstrcatstr(
						*result, prev, FALSE ) ) )
			RETURN( ERR_MEM );
	}

	RETURN( matches );
}

/*
 * Get- and Set-Functions
 */

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

	regex->flags = flags;
	return TRUE;
}
