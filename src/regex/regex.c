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

/** Constructor function to create a new pregex object. */
pregex* pregex_create( char* pat, int flags )
{
	pregex*			regex;
	pregex_ptn*		ptn;
	pregex_dfa*		dfa;

	PROC( "pregex_create" );
	PARMS( "pat", "%s", pat );
	PARMS( "flags", "%d", flags );

	if( !pregex_ptn_parse( &ptn, pat, flags ) )
		RETURN( (pregex*)NULL );

	ptn->accept->accept = 1;

	if( !( dfa = pregex_dfa_create() )
			|| !pregex_ptn_to_dfa( dfa, ptn ) )
	{
		pregex_dfa_free( dfa );
		RETURN( (pregex*)NULL);
	}

	regex = (pregex*)pmalloc( sizeof( pregex ) );
	regex->ptn = ptn;
	regex->flags = flags;

	/* Generate a dfatab */
	pregex_dfa_to_dfatab( (int***)NULL, dfa );

	if( ( regex->trans_cnt = pregex_dfa_to_dfatab( &regex->trans, dfa ) ) < 0 )
	{
		pregex_dfa_free( dfa );
		RETURN( pregex_free( regex ) );
	}

	/* DFA construction object can be freed */
	pregex_dfa_free( dfa );

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

/** Match //regex// at //start//.

Write result position to //end//, when provided. */
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

			if( !( regex->flags & PREGEX_MOD_GREEDY )
					&& regex->trans[ state ][ 2 ] & PREGEX_MOD_NONGREEDY )
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
		if( regex->flags & PREGEX_MOD_WCHAR )
		{
			VARS( "pstr", "%ls", (wchar_t*)ptr );
			ch = *( (wchar_t*)ptr );
			ptr += sizeof( wchar_t );

			if( regex->flags & PREGEX_MOD_DEBUG )
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

			if( regex->flags & PREGEX_MOD_DEBUG )
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

		for( i = 0; i < PREGEX_MAXREF; i++ )
			if( regex->ref[ i ].begin )
				fprintf( stderr, "%2d: >%.*s<\n",
					i, regex->ref[ i ].end - regex->ref[ i ].begin,
						regex->ref[ i ].begin );

		RETURN( TRUE );
	}
	else if( end )
		*end = (char*)NULL;

	RETURN( FALSE );
}

