/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pregex.c
Author:	Jan Max Meyer
Usage:	The pregex object functions.
----------------------------------------------------------------------------- */

#include <phorward.h>

#define IS_EXECUTABLE( stat )	( (stat) == PREGEX_STAT_NFA || \
									(stat) == PREGEX_STAT_DFA )

/** Constructor function to create a new pregex object.

Returns an initialized instance of a new pregex-object.
This object can be used and modified with subsequent pregex functions.

The object needs to be released using pregex_free() after its existence is not
longer required.
*/
pregex* pregex_create( void )
{
	pregex*		regex;

	PROC( "pregex_create" );

	regex = (pregex*)pmalloc( sizeof( pregex ) );
	pregex_set_flags( regex, PREGEX_MOD_GLOBAL );

	RETURN( regex );
}

/** Destructor function for a pregex-object.

//regex// is the pointer to a pregex-structure that will be released.

Returns always (pregex*)NULL.
*/
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

/** Resets all runtime and ephemerial elements of a pregex-object, so it can be
used for another regular expression match task.

//regex// is the pregex-object to be reset.

The function returns //regex//.
*/
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
	regex->last_str = (char*)NULL;
	regex->last_pos = (char*)NULL;

	memset( &( regex->range ), 0, sizeof( pregex_range ) );
	regex->range.accept = PREGEX_ACCEPT_NONE;

	pfree( regex->refs );
	regex->refs_cnt = 0;

	RETURN( regex );
}

/** Compiles a regular expression into a pattern and nondeterministic finite
automata (NFA) within a regex object.

//regex// is the pregex-object that will receive the new pattern.
//pattern// is the pattern to be compiled (UTF-8 string).
//accept// is the accepting ID to identify the pattern when it is matched.
//accept// must be >= 0.

Returns a ERR_OK on success, ERR_FAILURE if the regex is not initialized or
already turned into a different state. Another adequate ERR-define will be
returned for other errors.
*/
int pregex_compile( pregex* regex, char* pattern, int accept )
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

/** Finalizes a pregex-object into a minimized deterministic finite automata
(DFA). After finalization, new patterns can't be added anymore to the object.

//regex// is the pregex-object to be finalized.

Returns a ERR_OK on success, ERR_FAILURE if the regex is not in compiled-state,
and any other ERR-define else.
*/
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

/** The function pregex_match_next() is used to run a regular expression object
on a string, to match patterns. The function is called as long as no more
matches are found.

The function is used in a similar way as strtok(). The first call requires a
pointer to the string where the regular expression will be run against. Any
subsequent calls to pregex_match_next() must provide a (char*)NULL pointer as
//str//, so that pattern matching continues behind the previous matching
position. The function is thread-safe, because the previous matching position is
hold within the provided pregex-object.

//regex// is the pre-compiled or finalized pregex-object.
//str// is the pointer to input string where the pattern will be run against.
This shall be provided at the first call of pregex_match() and later on as
(char*)NULL.

Returns a pointer to a valid pregex_range structure describing the matched area
in case of a successful match, else a pointer to (pregex_range*)NULL.
*/
pregex_range* pregex_match_next( pregex* regex, char* str )
{
	int				match	= PREGEX_ACCEPT_NONE;
	int				anchors;
	psize			len;
	char*			pstr	= str;

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
				pregex_check_anchors( regex->last_str, pstr, len,
					anchors, regex->flags ) )
		{
			MSG( "pregex_nfa_match found a match!" );
			VARS( "match", "%d", match );
			VARS( "len", "%ld", len );

			/* Fill the match structure */
			regex->range.accept = match;

			if( !( regex->flags & PREGEX_MOD_WCHAR ) )
			{
				regex->range.begin = pstr;
				regex->range.end = pstr + len;
				regex->range.pos = (pchar*)pstr - (pchar*)regex->last_str;
			}
			else
			{
				regex->range.pbegin = (pchar*)pstr;
				regex->range.pend = (pchar*)pstr + len;
				regex->range.pos = pstr - regex->last_str;
			}

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
				/* accept and len could have been changed by
						the callback function */
				regex->range.accept = match;

				if( regex->range.len > len )
					len = regex->range.len;

				/* Move pstr to len characters forward */
				if( regex->flags & PREGEX_MOD_WCHAR )
				{
					pstr += len * sizeof( pchar );
					regex->range.pend = (pchar*)pstr;
					regex->range.len = (pchar*)pstr - regex->range.pbegin;
				}
				else
				{
#ifdef UTF8
					for( ; len > 0; len-- )
						pstr += u8_seqlen( pstr );
#else
					pstr += len;
#endif
					regex->range.end = pstr;
					regex->range.len = pstr - regex->range.begin;
				}

				/* Update pregex object runtime values */
				regex->last_pos = pstr;
				regex->match_count++;

				return &( regex->range );
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

	regex->last_pos = (char*)NULL;
	RETURN( (pregex_range*)NULL  );
}

/** Runs a regular expression match on a string as long as matches are found.
All matches will be collected into the return array //results//, if a pointer is
provided.

//regex// is the pregex-object to be used for matching.
//str// is the string on which the pattern will be executed.
//results// is the return pointer to an array of ranges to the matches within
//str//. The pointer ranges must be released after usage using pfree().
//results// can be left (pregex_range**) NULL, so only the number of matches
will be returned by the function.

The function returns the total number of matches, which is the number of entries
in the returned array //results//. If the value is negative, it is an error
code.
*/
int pregex_match( pregex* regex, char* str, pregex_range** results )
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
			range = pregex_match_next( regex, (char*)NULL ), matches++ )
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

/** The function pregex_split_next() is used to run a regular expression object
on a string, and split this string when a pattern is matched. The function is
called as long as no more matches are be found.

The function is used as the opposite of pregex_match_next(), it matches not the
patterns but the substrings between the patterns.

The function is used in a similar way as strtok(). The first call requires a
pointer to the string where the regular expression will be run against. Any
subsequent calls to pregex_match_next() must provide a (char*)NULL pointer as
//str//, so that pattern matching continues behind the previous matching
position. The function is thread-safe, because the previous matching position is
hold within the provided pregex-object.

//regex// is the pre-compiled or finalized pregex-object.
//str// is the pointer to input string where the pattern will be run against.
This shall be provided at the first call of pregex_match() and later on as
(char*)NULL.

Returns a pointer to a valid pregex_range structure describing the area in front
of or (in case of the last substring) behind the matched pattern, in case of a
successful match. Else a pointer to (pregex_range*)NULL is returned.
*/
pregex_range* pregex_split_next( pregex* regex, char* str )
{
	char*			last;
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
	else
		regex->last_pos = (char*)NULL;

	if( !( last = regex->last_pos ) )
		last = str;

	if( ( match = pregex_match_next( regex, str ) ) )
	{
		VARS( "match->accept", "%d", match->accept );

		/* We dont't have any match here! */
		regex->split.accept = PREGEX_ACCEPT_NONE;

		if( !( regex->flags & PREGEX_MOD_WCHAR ) )
		{
			regex->split.begin = last;
			regex->split.end = match->begin;
			regex->split.pos = regex->split.pbegin - (pchar*)regex->last_str;
			regex->split.len = regex->split.end - regex->split.begin;
		}
		else
		{
			regex->split.pbegin = (pchar*)last;
			regex->split.pend = match->pbegin;
			regex->split.pos = regex->split.pbegin
									- (pchar*)regex->last_str;
			regex->split.len = (pchar*)regex->split.end
									- (pchar*)regex->split.begin;
		}

		RETURN( &( regex->split ) );
	}

	/* Put last one if required! */
	if( last && *last )
	{
		/* We dont't have any match here! */
		regex->split.accept = PREGEX_ACCEPT_NONE;

		if( !( regex->flags & PREGEX_MOD_WCHAR ) )
			regex->split.begin = last;
		else
			regex->split.pbegin = (pchar*)last;

		while( *last )
		{
			/* Move one character forward */
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

		if( !( regex->flags & PREGEX_MOD_WCHAR ) )
		{
			regex->split.end = last;
			regex->split.pos = regex->split.pbegin - (pchar*)regex->last_str;
			regex->split.len = regex->split.end - regex->split.begin;
		}
		else
		{
			regex->split.pend = (pchar*)last;
			regex->split.pos = regex->split.pbegin
									- (pchar*)regex->last_str;
			regex->split.len = (pchar*)regex->split.end
									- (pchar*)regex->split.begin;
		}

		regex->last_pos = (char*)NULL;
		RETURN( &( regex->split ) );
	}

	MSG( "No more matches for split" );
	RETURN( (pregex_range*)NULL );
}

/** Runs a regular expression split on a string as long as matches are found.
All split matches will be collected into the return array //results//, if a
pointer is provided.

//regex// is the pregex-object to be used for matching.
//str// is the string on which the pattern will be executed.
//results// is the return pointer to an array of ranges to the matches within
//str//. The pointer ranges must be released after usage using pfree().
//results// can be left (pregex_range**) NULL, so only the number of matches
will be returned by the function.

The function returns the total number of matches, which is the number of entries
in the returned array //results//. If the value is negative, it is an error
code.
*/
int pregex_split( pregex* regex, char* str, pregex_range** results )
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
			range = pregex_split_next( regex, (char*)NULL ), matches++ )
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

/** Replaces all matches of a regular expression object within a string //str//
with //replacement//. Backreferences in //replacement// can be used with //$x//
for each opening bracket within the regular expression.

//regex// is the pregex-object used for pattern matching.
//str// is the string on which //regex// will be executed.
//replacement// is the string that will be inserted as the replacement for each
match of a pattern described in //regex//. The notation //$x// can be used for
backreferences, where x is the offset of opening brackets in the pattern,
beginning at 1.

Returns the number of matches (= replacements). If the value is negative, it is
an error define.
*/
char* pregex_replace( pregex* regex, char* str, char* replacement )
{
	pregex_range*	match;
	pregex_range*	reference;
	int				matches		= 0;
	int				charsize	= sizeof( char );
	int				ref;
	int				anchors;
	psize			len;
	char*			pstr;
	char*			start;
	char*			end;
	char*			rpstr;
	char*			rbegin;
	char*			rprev;
	char*			replace;
	char*			use_replacement;

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
		RETURN( (char*)NULL );
	}

	if( regex->flags & PREGEX_MOD_WCHAR )
		charsize = sizeof( pchar );

	regex->tmp_str = pfree( regex->tmp_str );

	for( start = str, match = pregex_match_next( regex, str );
			/* no condition in here is correct! */ ;
			match = pregex_match_next( regex, (char*)NULL ) )
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
				end = (char*)( (pchar*)start + pwcslen( (pchar*)start ) );
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
				if( !( regex->tmp_str = (char*)pwcsncatstr(
						(pchar*)regex->tmp_str, (pchar*)start,
							( (pchar*)end - (pchar*)start ) ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (char*)NULL );
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
					RETURN( (char*)NULL );
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
			for( rprev = rpstr = use_replacement, replace = (char*)NULL;
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

						if( iswdigit( *( ++_rpstr ) ) )
						{
							ref = wcstol( _rpstr, &end, 0 );

							VARS( "ref", "%d", ref );
							VARS( "end", "%ls", end );

							/* Skip length of the number */
							_rpstr = end;

							/* Extend first from prev of replacement */
							if( !( replace = (char*)pwcsncatstr(
									(pchar*)replace, (pchar*)rprev,
										(pchar*)rbegin - (pchar*)rprev ) ) )
							{
								regex->last_err = ERR_MEM;
								OUTOFMEM;
								RETURN( (char*)NULL );
							}

							VARS( "replace", "%ls", replace );
							VARS( "ref", "%d", ref );

							if( ( reference = pregex_get_ref( regex, ref ) ) )
							{
								MSG( "There is a reference!" );
								VARS( "reference->pbegin", "%ls",
										reference->pbegin );
								VARS( "len", "%d", reference->len );

								if( !( replace = (char*)pwcsncatstr(
										(pchar*)replace, reference->pbegin,
											reference->len ) ) )
								{
									regex->last_err = ERR_MEM;
									OUTOFMEM;
									RETURN( (char*)NULL );
								}
							}

							VARS( "replace", "%ls", (pchar*)replace );
							rprev = rpstr = (char*)_rpstr;
						}
						else
							rpstr = (char*)_rpstr;
					}
					else
					{
						char*		end;

						MSG( "Byte-character mode (Standard)" );

						if( isdigit( *( ++rpstr ) ) )
						{
							ref = strtol( rpstr, &end, 0 );

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
								RETURN( (char*)NULL );
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
									RETURN( (char*)NULL );
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
						!( replace = (char*)pwcscatstr(
								(pchar*)replace, (pchar*)rprev, FALSE ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (char*)NULL );
				}
			}
			else
			{
				if( rpstr != rprev && !( replace = pstrcatstr(
											replace, rprev, FALSE ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (char*)NULL );
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
				if( !( regex->tmp_str = (char*)pwcsncatstr(
						(pchar*)regex->tmp_str, (pchar*)replace,
							pwcslen( (pchar*)replace ) ) ) )
				{
					regex->last_err = ERR_MEM;
					OUTOFMEM;
					RETURN( (char*)NULL );
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
					RETURN( (char*)NULL );
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
/** Returns the last pattern match range of the object //regex//.

It contains the range of the substring of the last pattern match.

The range is only filled with a call to pregex_match_next(). If no matching
has been previously done, the function returns (pregex_range*)NULL. */
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
/** Returns the last pattern split range of the object //regex//.

It contains the range between the previous and the last pattern match, or the
range from the begin of the string before the first match or the position from
the last match to the end of the string, if no more matches where found.

The range is only filled with a call to pregex_match_next(). If no matching
has been previously done, the function returns (pregex_range*)NULL. */
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
/** Returns a reference from a match. References are available if the regular
expression object is configured without PREGEX_MOD_NO_REF. A reference (or
backreference) is a substring from the matched pattern which is created for
each opening bracket.

//regex// is the pregex-object from which to obtain the match reference.
//offset// is the offset of the desired reference. If the offset is higher
than the number of available references, the function returns
(pregex_range*)NULL.

As an example, a regular expression to find C function names is constructed
as ``[a-zA-Z_][a-zA-Z0-9_]\(\)``, its single name can be matched as a reference
by describing the pattern as ``([a-zA-Z_][a-zA-Z0-9_])\(\)``. Each opening
bracket defines a new reference level. If this pattern is now executed on a
string and finds a match, e.g. //&atoi()//, the whole string can be obtained by
the pregex_range-structure returned by pregex_get_range() (which is the string
"&atoi()"), but the function name can be directly accessed by obtaining the
first reference (//offset// == 0) which contains a range describing only
"atoi".

The reference ranges are only available with a preceeding and successfull call
to pregex_match_next(). */
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
/** Returns the number of matches found by //regex// so far. */
int pregex_get_match_count( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return 0;
	}

	return regex->match_count;
}

/** Returns the flags configuration of the object //regex//. */
int pregex_get_flags( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return PREGEX_MOD_NONE;
	}

	return regex->flags;
}

/** Sets a flag configuration for the object //regex// to //flags//.

Possible flags are:

|| Flag | Meaning |
| PREGEX_MOD_NONE | No modification (for the sake of completeness) |
| PREGEX_MOD_WCHAR | Regular expression and/or search string for direct \
pattern executions are handled (=casted) as type pchar (wide character, \
if UNICODE is flagged!) |
| PREGEX_MOD_INSENSITIVE | Regular expression is parsed case insensitive |
| PREGEX_MOD_GLOBAL | The regular expression execution is run globally, not \
only for the first match |
| PREGEX_MOD_STATIC | The regular expression passed for to the compiler \
should be converted 1:1 as it where a string-constant. Any regex-specific \
symbol will be ignored. |
| PREGEX_MOD_NO_REF | Don't create references |
| PREGEX_MOD_NO_ERRORS | Don't report errors, and try to compile as much as \
possible |
| PREGEX_MOD_NO_ANCHORS | Ignore anchor tokens, handle them as normal \
characters |
| PREGEX_MOD_GREEDY | Run regular expression in greedy-mode |
| PREGEX_MOD_NONGREEDY | Run regular expression in nongreedy-mode |
| PREGEX_MOD_DEBUG | Debug mode; output some debug to stderr |


All //flags// can be combined with logical or.

If //flags// are fundamentally changed between the compilation and execution
process of the //pregex// object, it may run into unwanted behaviors, so their
modification should be handled with care! */
pboolean pregex_set_flags( pregex* regex, int flags )
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

/** Returns a match function that is set for object //regex//. */
pregex_fn pregex_get_match_fn( pregex* regex )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return PREGEX_MOD_NONE;
	}

	return regex->match_fn;
}

/** Sets a match function for object //regex//.

A match function is a callback-function that is called for every match. The
match function can be used to filter-out special match constructors or, in case
of pregex_replace(), generate a context-dependent replacement string. */
pboolean pregex_set_match_fn( pregex* regex, pregex_fn match_fn )
{
	if( !( regex ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	regex->match_fn = match_fn;
	return TRUE;
}

