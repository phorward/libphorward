/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	nfa.c
Author:	Jan Max Meyer
Usage:	NFA creation and executable functions
		and simple, independent parser for regular expressions
----------------------------------------------------------------------------- */

#include "phorward.h"

/*NO_DOC*/
/** Creates a new NFA-state within an NFA state machine. The function first
checks if there are recyclable states in //nfa//. If so, the state is re-used
and re-configured, else a new state is allocated in memory.

//nfa// is the structure pointer to the NFA to process.
//chardef// is an optional charset definition for the new state. If this is
(char*)NULL, then a new epsilon state is created.
//flags// defines the modifier flags that belong to the chardef-parameter.

Returns a pointer to pregex_nfa_st, defining the newly created state. The
function returns (pregex_nfa_st*)NULL on error case.
*/
pregex_nfa_st* pregex_nfa_create_state(
	pregex_nfa* nfa, char* chardef, int flags )
{
	pregex_nfa_st* 	ptr;

	PROC( "pregex_nfa_create_state" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "chardef", "%s", chardef ? chardef : "NULL" );

	/* Use state from empty stack? */
	if( nfa->empty )
	{
		MSG( "Re-using existing state" );
		nfa->empty = list_pop( nfa->empty, (void**)&ptr );
	}
	else
	{
		MSG( "Get new state" );
		if( !( ptr = pmalloc( sizeof( pregex_nfa_st ) ) ) )
		{
			MSG( "Out of memory error" );
			RETURN( (pregex_nfa_st*)NULL );
		}
	}

	/* Initialize */
	memset( ptr, 0, sizeof( pregex_nfa_st ) );
	pregex_accept_init( &( ptr->accept ) );
	ptr->ref = nfa->ref_cur;

	/* Put into list of NFA states */
	if( !( nfa->states = list_push( nfa->states, (void*)ptr ) ) )
	{
		pfree( ptr );

		MSG( "Out of memory error!" );
		RETURN( (pregex_nfa_st*)NULL );
	}

	/* Define character edge? */
	if( chardef )
	{
		MSG( "Required to parse chardef" );
		VARS( "chardef", "%s", chardef );

		if( !( ptr->ccl = pregex_ccl_create( -1, -1, chardef ) ) )
		{
			MSG( "Out of memory error" );
			RETURN( (pregex_nfa_st*)NULL );
		}

		/* Is case-insensitive flag set? */
		if( flags & PREGEX_MOD_INSENSITIVE )
		{
			pregex_ccl*	iccl;
			int			i;
			wchar		ch;
			wchar		cch;

			iccl = pregex_ccl_dup( ptr->ccl );

			MSG( "PREGEX_MOD_INSENSITIVE set" );
			for( i = 0; pregex_ccl_get( &ch, (pchar*)NULL, ptr->ccl, i ); i++ )
			{
				VARS( "ch", "%d", ch );
#ifdef UNICODE
				if( iswupper( ch ) )
					cch = towlower( ch );
				else
					cch = towupper( ch );
#else
				if( isupper( ch ) )
					cch = tolower( ch );
				else
					cch = toupper( ch );
#endif
				VARS( "cch", "%d", cch );
				if( ch == cch )
					continue;

				if( !pregex_ccl_add( iccl, cch ) )
					RETURN( (pregex_nfa_st*)NULL );
			}

			pregex_ccl_free( ptr->ccl );
			ptr->ccl = iccl;
		}

		VARS( "ptr->ccl", "%p", ptr->ccl );
	}

	RETURN( ptr );
}

/** Prints an NFA for debug purposes on a output stream.

//stream// is the output stream where to print the NFA to.
//nfa// is the NFA state machine structure to be printed.
*/
void pregex_nfa_print( pregex_nfa* nfa )
{
	LIST*			l;
	pregex_nfa_st*	s;

	fprintf( stderr, " no next next2 accept ref anchor\n" );
	fprintf( stderr, "--------------------------------\n" );

	for( l = nfa->states; l; l = list_next( l ) )
	{
		s = (pregex_nfa_st*)list_access( l );

		fprintf( stderr, "#% 2d % 4d % 5d  % 6d  % 3d % 6d\n",
			list_find( nfa->states, (void*)s ),
			list_find( nfa->states, (void*)s->next ),
			list_find( nfa->states, (void*)s->next2 ),
			s->accept.accept, s->ref, s->accept.anchors );

		if( s->ccl )
			pregex_ccl_print( stderr, s->ccl, 0 );

		fprintf( stderr, "\n\n" );
	}

	fprintf( stderr, "-----------------------------------\n" );
}

/** Allocates an initializes a new pregex_nfa-object for a nondeterministic
finite state automata that can be used for pattern matching or to construct
a determinisitic finite automata out of.

The function pregex_nfa_free() shall be used to destruct a pregex_dfa-object. */
pregex_nfa* pregex_nfa_create( void )
{
	pregex_nfa*		nfa;

	nfa = (pregex_nfa*)pmalloc( sizeof( pregex_nfa ) );

	return nfa;
}

/** Releases a pregex_nfa state machine.

//nfa// is the pointer to the NFA state machine structure. All allocated
elements of this structure as well as the structure itself will be freed.

The function always returns (pregex_nfa*)NULL.
*/
pregex_nfa* pregex_nfa_free( pregex_nfa* nfa )
{
	LIST*			l;
	pregex_nfa_st*	nfa_st;

	PROC( "pregex_nfa_free" );
	PARMS( "nfa", "%p", nfa );

	if( !nfa )
		RETURN( (pregex_nfa*)NULL );

	MSG( "Clearing states" );
	for( l = nfa->states; l; l = list_next( l ) )
	{
		nfa_st = (pregex_nfa_st*)list_access( l );
		if( nfa_st->ccl )
			pregex_ccl_free( nfa_st->ccl );

		pfree( nfa_st );
	}

	MSG( "Clearing empty state stack" );
	for( l = nfa->empty; l; l = list_next( l ) )
	{
		nfa_st = (pregex_nfa_st*)list_access( l );
		pregex_ccl_free( nfa_st->ccl );

		pfree( nfa_st );
	}

	list_free( nfa->states );
	list_free( nfa->empty );

	pfree( nfa );

	RETURN( (pregex_nfa*)NULL );
}

/** Performs a move operation on a given input character from a set of NFA
states.

//input// is the list of input states (pregex_nfa_st*).
//from// is the character-range begin from which the move-operation should be
processed on.
//to// is the character-range end until the move-operation should be processed.

Returns a linked-list (LIST*) to the result of the move operation. If this is
(LIST*)NULL, there is no possible transition on the given input character.
*/
LIST* pregex_nfa_move( pregex_nfa* nfa, LIST* input, pchar from, pchar to )
{
	LIST*			hits	= (LIST*)NULL;
	pregex_nfa_st*	test;

	PROC( "pregex_nfa_move" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "input", "%p", input );
	PARMS( "from", "%d", from );
	PARMS( "to", "%d", to );

	/* Loop trough the input items */
	while( input != (LIST*)NULL )
	{
		input = list_pop( input, (void**)&test );
		VARS( "input", "%p", input );
		VARS( "test", "%p", test );

		/* Not an epsilon edge? */
		if( test->ccl )
		{
			/* Test for range */
			if( pregex_ccl_testrange( test->ccl, from, to ) )
			{
				MSG( "State matches range!" );
				hits = list_push( hits, test->next );
			}
		}
	}

	VARS( "hits", "%p", hits );
	VARS( "hits count", "%d", list_count( hits ) );

	RETURN( hits );
}

/** Performs an epsilon closure from a set of NFA states.

//nfa// is the NFA state machine
//input// is the list of input NFA states
//accept// is the match information structure, which recieves possible
information about a pattern match. This parameter is optional, and can be
left empty by providing (pregex_accept*)NULL.

Returns a linked list (LIST*) to the result of the epsilon closure, which
defines a new set of NFA states.
*/
LIST* pregex_nfa_epsilon_closure( pregex_nfa* nfa, LIST* input,
			pregex_accept* accept )
{
	pregex_nfa_st*	top;
	pregex_nfa_st*	next;
	pregex_nfa_st*	last_accept	= (pregex_nfa_st*)NULL;
	LIST*			stack;
	short			i;

	PROC( "pregex_nfa_epsilon_closure" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "input", "%p", input );
	PARMS( "accept", "%p", accept );

	if( accept )
		pregex_accept_init( accept );

	stack = list_dup( input );

	/* Loop trough the items */
	while( stack != (LIST*)NULL )
	{
		stack = list_pop( stack, (void**)&top );

		if( accept && top->accept.accept != PREGEX_ACCEPT_NONE )
		{
			if( !last_accept || last_accept->accept.accept >
									top->accept.accept )
				last_accept = top;
		}

		if( !top->ccl )
		{
			for( i = 0; i < 2; i++ )
			{
				next = ( !i ? top->next : top->next2 );
				if( next && list_find( input, (void*)next ) == -1 )
				{
					input = list_push( input, (void*)next );
					stack = list_push( stack, (void*)next );
				}
			}
		}
		else if( top->next2 )
		{
			/* This may not happen! */
			fprintf( stderr,
				"%s, %d: FATAL :: Character-node with two outgoing"
						" transitions!\n", __FILE__, __LINE__ );
			exit( 1 );
		}
	}

	if( accept && last_accept )
	{
		accept->accept = last_accept->accept.accept;
		VARS( "accept->accept", "%d", accept->accept );

		accept->greedy = last_accept->accept.greedy;
		VARS( "accept->greedy", "%s", BOOLEAN_STR( accept->greedy ) );

		accept->anchors = last_accept->accept.anchors;
		VARS( "accept->anchors", "%d", accept->anchors );
	}

	RETURN( input );
}

/** Tries to match a pattern using a NFA state machine.

//nfa// is the NFA state machine to be executed.
//str// is a test string where the NFA should work on.
//len// receives the length of the match, -1 on error or no match.

//anchors// receives the anchor configuration of the matching state, if it
provides anchors. If this is (int*)NULL, anchors will be ignored.

//ref// receives a return array of references; If this pointer is not NULL, the
function will allocate memory for a reference array. This array is only
allocated if the following dependencies are met:
# The NFA has references
# //ref_count// is zero
# //ref// points to a pregex_range*

//ref_count// receives the number of references. This value MUST be zero, if the
function should allocate refs. A positive value indicates the number of elements
in //ref//, so the array can be re-used in multiple calls.

//flags// are the flags to modify the NFA state machine matching behavior.

Returns PREGEX_ACCEPT_NONE, if no match was found, else the number of the match
that was found relating to a pattern in //nfa//.
*/
int pregex_nfa_match( pregex_nfa* nfa, char* str, psize* len, int* anchors,
		pregex_range** ref, int* ref_count, int flags )
{
	LIST*			res			= (LIST*)NULL;
	LIST*			l;
	pregex_nfa_st*	st;
	char*			pstr		= str;
	psize			plen		= 0;
	int				last_accept = PREGEX_ACCEPT_NONE;
	int				rc;
	pchar			ch;
	pregex_accept	accept;

	PROC( "pregex_nfa_match" );
	PARMS( "nfa", "%p", nfa );
	if( flags & PREGEX_MOD_WCHAR )
		PARMS( "str", "%ls", str );
	else
		PARMS( "str", "%s", str );
	PARMS( "len", "%p", len );
	PARMS( "anchors", "%p", anchors );
	PARMS( "ref", "%p", ref );
	PARMS( "ref_count", "%p", ref_count );
	PARMS( "flags", "%d", flags );

	/* Initialize */
	pregex_accept_init( &accept );

	if( ( rc = pregex_ref_init( ref, ref_count, nfa->ref_count, flags ) )
			!= ERR_OK )
		RETURN( rc );

	*len = 0;
	if( anchors )
		*anchors = PREGEX_ANCHOR_NONE;

	res = list_push( res, list_access( nfa->states ) );

	/* Run the engine! */
	while( res )
	{
		MSG( "Performing epsilon closure" );
		res = pregex_nfa_epsilon_closure( nfa, res, &accept );

		MSG( "Handling References" );
		if( ref && nfa->ref_count )
		{
			LISTFOR( res, l )
			{
				st = (pregex_nfa_st*)list_access( l );
				if( st->ref > -1 )
				{
					MSG( "Reference found" );
					VARS( "State", "%d", list_find(
									nfa->states, (void*)st ) );
					VARS( "st->ref", "%d", st->ref );

					pregex_ref_update( &( ( *ref )[ st->ref ] ), pstr, plen );
				}
			}
		}

		VARS( "accept.accept", "%d", accept.accept );
		if( accept.accept > PREGEX_ACCEPT_NONE )
		{
			if( flags & PREGEX_MOD_DEBUG )
			{
				if( flags & PREGEX_MOD_WCHAR )
					fprintf( stderr, "accept %d, len %d >%.*ls<\n",
						accept.accept, plen, plen, (pchar*)str );
				else
					fprintf( stderr, "accept %d, len %d >%.*s<\n",
						accept.accept, plen, plen, str );
			}

			MSG( "New accepting state takes place!" );
			last_accept = accept.accept;
			*len = plen;

			if( anchors )
				*anchors = accept.anchors;

			VARS( "last_accept", "%d", last_accept );
			VARS( "*len", "%d", *len );

			if( !( flags & PREGEX_MOD_GREEDY ) )
			{
				VARS( "accept.greedy", "%s", BOOLEAN_STR( accept.greedy ) );
				if(	!accept.greedy || ( flags & PREGEX_MOD_NONGREEDY ) )
				{
					if( flags & PREGEX_MOD_DEBUG )
						fprintf( stderr, "greedy set, match terminates\n" );

					MSG( "Greedy is set, will stop recognition with "
							"this match" );
					break;
				}
			}
		}

		if( flags & PREGEX_MOD_WCHAR )
		{
			VARS( "pstr", "%ls", (pchar*)pstr );
			ch = *((pchar*)pstr);

			if( flags & PREGEX_MOD_DEBUG )
				fprintf( stderr, "reading >%lc< %d\n", ch, ch );
		}
		else
		{
			VARS( "pstr", "%s", pstr );
#ifdef UTF8
			ch = u8_char( pstr );
#else
			ch = *pstr;
#endif

			if( flags & PREGEX_MOD_DEBUG )
				fprintf( stderr, "reading >%c< %d\n", ch, ch );
		}

		VARS( "ch", "%d", ch );
		VARS( "ch", "%lc", ch );

		res = pregex_nfa_move( nfa, res, ch, ch );

		if( flags & PREGEX_MOD_WCHAR )
		{
			pstr += sizeof( pchar );
		}
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}

		plen++;
		VARS( "plen", "%ld", plen );
	}

	VARS( "*len", "%d", *len );
	VARS( "last_accept", "%d", last_accept );
	RETURN( last_accept );
}

/** Builds or extends a NFA state machine from a string. The string is simply
converted into a state machine that exactly matches the desired string.

//nfa// is the pointer to the NFA state machine to be extended.
//str// is the sequence of characters to be converted one-to-one into //nfa//.
//flags// are flags for regular expresson processing.
//acc// is the acceping identifier that is returned on pattern match.

Returns ERR_OK on success, ERR... error code else
*/
int pregex_nfa_from_string( pregex_nfa* nfa, char* str, int flags, int acc )
{
	pregex_nfa_st*	nfa_st;
	pregex_nfa_st*	append_to;
	pregex_nfa_st*	first_nfa_st;
	pregex_nfa_st*	prev_nfa_st;
	char*			pstr;
	wchar			ch;

	PROC( "pregex_nfa_from_string" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );
	PARMS( "acc", "%d", acc );

	if( !( nfa && str ) )
		RETURN( ERR_PARMS );

	/* For wide-character execution, copy string content */
	if( flags & PREGEX_MOD_WCHAR )
	{
		if( !( str = wchar_to_u8( (pchar*)str, FALSE ) ) )
			return ERR_MEM;
	}

	/* Find node to integrate into existing machine */
	for( append_to = (pregex_nfa_st*)list_access( nfa->states );
		append_to && append_to->next2; append_to = append_to->next2 )
			; /* Find last first node ;) ... */

	/* References */
	nfa->ref_cur = nfa->ref_count++;

	/* Create first state - this is an epsilon node */
	if( !( first_nfa_st = prev_nfa_st =
			pregex_nfa_create_state( nfa, (char*)NULL, flags ) ) )
		RETURN( ERR_MEM );

	for( pstr = str; *pstr; )
	{
		/* Then, create all states that form the string */
		MSG( "Adding new state" );
		VARS( "pstr", "%s", pstr );
		if( !( nfa_st = pregex_nfa_create_state(
				nfa, (char*)NULL, flags ) ) )
			RETURN( ERR_MEM );

		ch = u8_parse_char( &pstr );
		VARS( "ch", "%d", ch );

		nfa_st->ccl = pregex_ccl_create( -1, -1, (char*)NULL );

		if( !( nfa_st->ccl && pregex_ccl_add( nfa_st->ccl, ch ) ) )
			RETURN( ERR_MEM );

		/* Is case-insensitive flag set? */
		if( flags & PREGEX_MOD_INSENSITIVE )
		{
#ifdef UTF8
			MSG( "UTF-8 mode, trying to convert" );
			if( iswupper( ch ) )
				ch = towlower( ch );
			else
				ch = towupper( ch );
#else
			MSG( "non UTF-8 mode, trying to convert" );
			if( isupper( ch ) )
				ch = (char)tolower( (int)ch );
			else
				ch = (char)toupper( (int)ch );
#endif

			MSG( "Case-insensity set, new character evaluated is:" );
			VARS( "ch", "%d", ch );
			if( !pregex_ccl_add( nfa_st->ccl, ch ) )
				RETURN( ERR_MEM );
		}

		prev_nfa_st->next = nfa_st;
		prev_nfa_st = nfa_st;
	}

	/* Add accepting node */
	VARS( "acc", "%d", acc );
	if( !( nfa_st = pregex_nfa_create_state( nfa,
			(char*)NULL, flags ) ) )
		RETURN( ERR_MEM );

	nfa_st->accept.accept = acc;
	prev_nfa_st->next = nfa_st;

	/* Append to existing machine, if required */
	VARS( "append_to", "%p", append_to );
	if( append_to )
		append_to->next2 = first_nfa_st;

	/* Free copied string, in wide character mode */
	if( flags & PREGEX_MOD_WCHAR )
		pfree( str );

	RETURN( ERR_OK );
}

/*COD_ON*/
