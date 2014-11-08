/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pregex.c
Author:	Jan Max Meyer
Usage:	Interface for pregex-objects serving regular expressions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Constructor function to create a new pregex object.

//pat// is a string providing a regular expression pattern.

//flags// can be a combination of compile- and runtime-flags.

*/
pregex* pregex_create( char* pat, int flags )
{
	pregex*			regex;
	pregex_ptn*		ptn;

	PROC( "pregex_create" );
	PARMS( "pat", "%s", pat );
	PARMS( "flags", "%d", flags );

	if( !pregex_ptn_parse( &ptn, pat, flags ) )
		RETURN( (pregex*)NULL );

	ptn->accept->accept = 1;

	regex = (pregex*)pmalloc( sizeof( pregex ) );
	regex->ptn = ptn;
	regex->flags = flags;

	/* Generate a dfatab */
	if( ( regex->trans_cnt = pregex_ptn_to_dfatab( &regex->trans, ptn ) ) < 0 )
		RETURN( pregex_free( regex ) );

	RETURN( regex );
}

/** Destructor function for a pregex-object.

//regex// is the pointer to a pregex-structure that will be released.

Returns always (pregex*)NULL.
*/
pregex* pregex_free( pregex* regex )
{
	int		i;

	PROC( "pregex_free" );
	PARMS( "regex", "%p", regex );

	if( !regex )
		RETURN( (pregex*)NULL );

	/* Freeing the pattern definitions */
	regex->ptn = pregex_ptn_free( regex->ptn );

	/* Drop out the dfatab */
	for( i = 0; i < regex->trans_cnt; i++ )
		pfree( regex->trans[ i ] );

	pfree( regex->trans );
	pfree( regex );

	RETURN( (pregex*)NULL );
}

/** Tries to match the regular expression //regex// at pointer //start//.

If the expression can be matched, the function returns TRUE and //end// receives
the pointer to the last matched character. */
pboolean pregex_match( pregex* regex, char* start, char** end )
{
	int		i;
	int		state		= 0;
	int		next_state;
	char*	match		= (char*)NULL;
	char*	ptr			= start;
	wchar_t	ch;

	PROC( "pregex_match" );
	PARMS( "regex", "%p", regex );
	PARMS( "start", "%s", start );
	PARMS( "end", "%p", end );

	if( !( regex && start ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( regex->ref, 0, PREGEX_MAXREF * sizeof( pregex_range ) );

	while( state >= 0 )
	{
		/* State accepts? */
		if( regex->trans[ state ][ 1 ] )
		{
			MSG( "This state accepts the input" );
			match = ptr;

			if( !( regex->flags & PREGEX_RUN_GREEDY )
					&& regex->trans[ state ][ 2 ] & PREGEX_FLAG_NONGREEDY )
				break;
		}

		/* References */
		if( regex->trans[ state ][ 3 ] )
		{
			for( i = 0; i < PREGEX_MAXREF; i++ )
			{
				if( regex->trans[ state ][ 3 ] & ( 1 << i ) )
				{
					if( !regex->ref[ i ].begin )
						regex->ref[ i ].begin = ptr;

					regex->ref[ i ].end = ptr;
				}
			}
		}

		/* Get next character */
		if( regex->flags & PREGEX_RUN_WCHAR )
		{
			VARS( "pstr", "%ls", (wchar_t*)ptr );
			ch = *( (wchar_t*)ptr );
			ptr += sizeof( wchar_t );

			if( regex->flags & PREGEX_RUN_DEBUG )
				fprintf( stderr, "reading wchar_t %d (>%lc<)\n", ch, ch );
		}
		else
		{
			VARS( "pstr", "%s", ptr );
#ifdef UTF8
			ch = u8_char( ptr );
			ptr += u8_seqlen( ptr );
#else
			ch = *ptr++;
#endif

			if( regex->flags & PREGEX_RUN_DEBUG )
				fprintf( stderr, "reading char %d (>%c<)\n", ch, ch );
		}

		/* Initialize default transition */
		next_state = regex->trans[ state ][ 4 ];

		/* Find transition according to current character */
		for( i = 5; i < regex->trans[ state ][ 0 ]; i += 3 )
		{
			if( regex->trans[ state ][ i ] <= ch
					&& regex->trans[ state ][ i + 1 ] >= ch )
			{
				next_state = regex->trans[ state ][ i + 2 ];
				break;
			}
		}

		if( next_state == regex->trans_cnt )
			break;

		state = next_state;
	}

	if( match )
	{
		if( end )
			*end = match;

		/*
		for( i = 0; i < PREGEX_MAXREF; i++ )
			if( regex->ref[ i ].begin )
				fprintf( stderr, "%2d: >%.*s<\n",
					i, regex->ref[ i ].end - regex->ref[ i ].begin,
						regex->ref[ i ].begin );
		*/

		RETURN( TRUE );
	}

	RETURN( FALSE );
}

/** Find a match for the regular expression //regex// from begin of pointer
//start//.

//start// has to be a zero-terminated string or wide-character string (according
to the configuration of the pregex-object).

If the expression can be matched, the function returns the pointer to the
position where the match begins. //end// receives the end pointer of the match,
when provided.

The function returns (char*)NULL in case that there is no match.
*/
char* pregex_find( pregex* regex, char* start, char** end )
{
	wchar_t		ch;
	char*		ptr 	= start;
	char*		lptr;
	int			i;

	PROC( "pregex_find" );
	PARMS( "regex", "%p", regex );
	PARMS( "start", "%s", start );
	PARMS( "end", "%p", end );

	if( !( regex && start ) )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	do
	{
		lptr = ptr;

		/* Get next character */
		if( regex->flags & PREGEX_RUN_WCHAR )
		{
			ch = *( (wchar_t*)ptr );
			ptr += sizeof( wchar_t );
		}
		else
		{
#ifdef UTF8
			ch = u8_char( ptr );
			ptr += u8_seqlen( ptr );
#else
			ch = *ptr++;
#endif
		}

		if( !ch )
			break;

		/* Find a transition according to current character */
		for( i = 5; i < regex->trans[ 0 ][ 0 ]; i += 3 )
			if( regex->trans[ 0 ][ i ] <= ch
				&& regex->trans[ 0 ][ i + 1 ] >= ch
					&& pregex_match( regex, lptr, end ) )
						RETURN( lptr );
	}
	while( ch );

	RETURN( (char*)NULL );
}

/** Find all matches for the regular expression //regex// from begin of pointer
//start//, and optionally return matches as an array.

//start// has to be a zero-terminated string or wide-character string (according
to the configuration of the pregex-object).

The function fills the array //matches//, if provided, with items of size
pregex_range. It returns the total number of matches.
*/
int pregex_findall( pregex* regex, char* start, parray** matches )
{
	char*			end;
	int				count	= 0;
	pregex_range*	r;

	PROC( "pregex_findall" );
	PARMS( "regex", "%p", regex );
	PARMS( "start", "%s", start );
	PARMS( "matches", "%p", matches );

	if( !( regex && start ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	if( matches )
		*matches = (parray*)NULL;

	while( ( start = pregex_find( regex, start, &end ) ) )
	{
		printf( "FIND: >%.*s<\n", end - start, start );
		if( matches )
		{
			if( ! *matches )
				*matches = parray_create( sizeof( pregex_range ), 0 );

			r = (pregex_range*)parray_malloc( *matches );
			r->id = 1;
			r->begin = start;
			r->end = end;
		}

		start = end;
		count++;
	}

	RETURN( count );
}

/** Returns the range between string //start// and the next match of //regex//.

This function can be seen as a "negative match", so the substrings that are
not part of the match will be returned.

//start// has to be a zero-terminated string or wide-character string (according
to the configuration of the pregex-object).
//end// receives the last position of the string before the regex.
//next// receives the pointer of the next split element behind the matched
substring, so //next// should become the next //start// when pregex_split() is
called in a loop.

The function returns (char*)NULL in case that there is no more string to split,
else it returns //start//.
*/
char* pregex_split( pregex* regex, char* start, char** end, char** next )
{
	wchar_t		ch;
	char*		ptr 	= start;
	char*		lptr;
	int			i;

	PROC( "pregex_split" );
	PARMS( "regex", "%p", regex );
	PARMS( "start", "%s", start );
	PARMS( "end", "%p", end );

	if( !( regex && start ) )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( next )
		*next = (char*)NULL;

	do
	{
		lptr = ptr;

		/* Get next character */
		if( regex->flags & PREGEX_RUN_WCHAR )
		{
			ch = *( (wchar_t*)ptr );
			ptr += sizeof( wchar_t );
		}
		else
		{
#ifdef UTF8
			ch = u8_char( ptr );
			ptr += u8_seqlen( ptr );
#else
			ch = *ptr++;
#endif
		}

		if( !ch )
			break;

		/* Find a transition according to current character */
		for( i = 5; i < regex->trans[ 0 ][ 0 ]; i += 3 )
			if( regex->trans[ 0 ][ i ] <= ch
				&& regex->trans[ 0 ][ i + 1 ] >= ch
					&& pregex_match( regex, lptr, next ) )
						ch = 0;
	}
	while( ch );

	if( lptr > start )
	{
		if( end )
			*end = lptr;

		RETURN( start );
	}

	RETURN( (char*)NULL );
}

/** Split all matches. Blah. */
int pregex_splitall( pregex* regex, char* start, parray** matches )
{
	char*			end;
	char*			next;
	int				count	= 0;
	pregex_range*	r;

	PROC( "pregex_splitall" );
	PARMS( "regex", "%p", regex );
	PARMS( "start", "%s", start );
	PARMS( "matches", "%p", matches );

	if( !( regex && start ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	if( matches )
		*matches = (parray*)NULL;

	while( pregex_split( regex, start, &end, &next ) )
	{
		if( matches )
		{
			if( ! *matches )
				*matches = parray_create( sizeof( pregex_range ), 0 );

			r = (pregex_range*)parray_malloc( *matches );
			r->begin = start;
			r->end = end;
		}

		count++;
		start = next;
	}

	RETURN( count );
}
