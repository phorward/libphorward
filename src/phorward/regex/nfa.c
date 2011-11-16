/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	nfa.c
Author:	Jan Max Meyer
Usage:	NFA creation and executable functions
		and simple, independent parser for regular expressions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */
#define INC( i )			(i)++
#define VALID_CHAR( ch )	!pstrchr( "|()[]*+?", (ch) )

/*
 * Local prototypes
 */
static int parse_char( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );
static int parse_factor( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );
static int parse_sequence( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );
static int parse_alter( uchar** pstr, pregex_nfa* nfa, pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags );

/*
 * Functions
 */

/*NO_DOC*/
/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_create_state()

	Author:			Jan Max Meyer

	Usage:			Creates a new NFA-state within an NFA state machine.
					The function first checks if there are recyclable states.
					If so, the state is re-used and re-configured, else a new
					state is allocated in memory.

	Parameters:		pregex_nfa*		nfa			NFA to output.
					uchar*			chardef		An optional charset definition
												for the new state. If this is
												(uchar*)NULL, then a new epsi-
												lon state is created.
					int				flags		Modifier flags that belongs to
												the chardef-parameter.

	Returns:		pregex_nfa_st*				Pointer to the created state.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pregex_nfa_st* pregex_nfa_create_state(
	pregex_nfa* nfa, uchar* chardef, int flags )
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

		if( !( ptr->ccl = ccl_create( chardef ) ) )
		{
			MSG( "Out of memory error" );
			RETURN( (pregex_nfa_st*)NULL );
		}
		
		/* Is case-insensitive flag set? */
		if( flags & REGEX_MOD_INSENSITIVE )
		{
			CCL		iccl	= (CCL)NULL;
			CCL		c;
			wchar	ch;
			wchar	cch;
			
			MSG( "REGEX_MOD_INSENSITIVE set" );
			for( c = ptr->ccl; c && c->begin != CCL_MAX; c++ )
			{
				for( ch = c->begin; ch <= c->end; ch++ )
				{
					if( Pisupper( ch ) )
						cch = Ptolower( ch );
					else
						cch = Ptoupper( ch );

					VARS( "cch", "%d", cch );

					if( !( iccl = ccl_addrange( iccl, cch, cch ) ) )
						RETURN( (pregex_nfa_st*)NULL );
				}
			}
			
			if( !( ptr->ccl = ccl_union( ptr->ccl, iccl ) ) )
			{
				ccl_free( iccl );
				RETURN( (pregex_nfa_st*)NULL );
			}
			
			ccl_free( iccl );
		}

		VARS( "ptr->ccl", "%p", ptr->ccl );
	}
	
	RETURN( ptr );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_to_regex()

	Author:			Jan Max Meyer

	Usage:			Turns a NFA state machine back into a well formatted
	 				regular expression string. The functions
	 				pregex_nfa_to_REGEX(), pregex_ccl_to_REGEX() and
	 				pregex_char_to_REGEX() are only internally used for
	 				recursion. The function is currently in testing mode.

	Parameters:		pregex_nfa*		nfa			NFA state machine that should
												be turned back into a regular
												expression string.

	Returns:		uchar*						Returns a well-formatted regular
												expression that can be parsed
												by the pregex regular expression
												parser. The string is allocated
												dynamically and must be freed
												by the function caller.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#if 0
static void pregex_char_to_REGEX( uchar* str, int size,
				pchar ch, pboolean escape )
{
	if( ch == '[' || ch == ']' || ch == '*' || ch == '+' || ch == '?' )
		psprintf( str, "\\%c", (uchar)ch );
	else if( escape )
		u8_escape_wchar( str, size, ch );
	else
		u8_toutf8( str, size, &ch, 1 );
}

static void pregex_ccl_to_REGEX( uchar** str, pregex_ccl ccl, pboolean escape )
{
	pregex_ccl		neg		= (pregex_ccl)NULL;
	pregex_ccl		i;
	uchar			from	[ 40 + 1 ];
	uchar			to		[ 20 + 1 ];
	pboolean		range	= FALSE;
	
	if( ccl_count( ccl ) == CCL_MAX )
	{
		*str = pstr_append_char( *str, '.' );
		return;
	}
	
	/* 
	 * Always duplicate character-class,
	 * we sometimes will modify it
	 */
	ccl = ccl_dup( ccl );

	if( ccl_count( ccl ) > 128 )
		ccl = neg = ccl_negate( ccl );
	
	if( neg || ccl_count( ccl) > 1 )
	{
		range = TRUE;
		*str = pstr_append_char( *str, '[' );
		if( neg )
			*str = pstr_append_char( *str, '^' );
	}
	
	/*
	 * If ccl contains a dash,
	 * it should be printed first!
	 */
	if( ccl_test( ccl, '-' ) )
	{
		*str = pstr_append_char( *str, '-' );
		ccl = ccl_delrange( ccl, '-', '-' );
	}
	
	/* Go trough ccl... */
	for( i = ccl; !ccl_end( i ); i++ )
	{
		pregex_char_to_REGEX( from, (int)sizeof( from ), i->begin, escape );

		if( i->begin != i->end )
		{
			pregex_char_to_REGEX( to, (int)sizeof( to ), i->end, escape );
			psprintf( from + strlen( from ), "-%s", to );
		}
		
		*str = pstr_append_str( *str, from, FALSE );
	}
	
	if( range )
		*str = pstr_append_char( *str, ']' );
		
	ccl_free( ccl );
}

static pregex_nfa_st* pregex_nfa_to_REGEX( uchar** str, pregex_nfa* nfa,
				pregex_nfa_st* state, int rec )
{
	pregex_nfa_st*	end;
	int				i;

#ifdef REGEXGEN_DBG
	uchar*			tmp;
	uchar			gap[80+1];

	
	for( i = 0; i < rec; i++ )
		gap[i] = ' ';
	gap[i] = 0;
#endif
	
	/* Begin */	
	while( state )
	{
#ifdef REGEXGEN_DBG
		fprintf( stderr, "%sCurrent state %d\n",
			gap, list_find( nfa->states, state ) );
#endif
		if( state->brackets > 0 )
			for( i = 0; i < state->brackets; i++ )
				*str = pstr_append_char( *str, '(' );
		else if( state->brackets < 0 )
			for( i = 0; i < ( state->brackets * -1 ); i++ )
				*str = pstr_append_char( *str, ')' );
		
		if( state->operator == '|' )
		{
#ifdef REGEXGEN_DBG
			fprintf( stderr, "%sAlternative 1\n", gap );
#endif
			end = pregex_nfa_to_REGEX( str, nfa, state->next, rec + 1 );

			if( state->next2 && state->operator == '|' )
			{
#ifdef REGEXGEN_DBG
				fprintf( stderr, "%sAlternative 2\n", gap );
#endif
				*str = pstr_append_char( *str, state->operator );
				state = pregex_nfa_to_REGEX( str, nfa, state->next2, rec + 1 );
			}
			else
			{
				state = end;
			}
		}
		/* Node with operator */
		else if( state->operator )
		{
#ifdef REGEXGEN_DBG
			fprintf( stderr, "%sModifier %c\n",
				gap, state->operator );
#endif
			end = pregex_nfa_to_REGEX( str, nfa, state->next, rec + 1 );
			*str = pstr_append_char( *str, state->operator );
			
			state = end;
		}
		/* Character node */
		else if( state->ccl )
		{
#ifdef REGEXGEN_DBG
			tmp = ccl_to_str( state->ccl, TRUE );
			fprintf( stderr, "%sCharclass: %s\n", gap, tmp );
			pfree( tmp );
#endif
			/*
			if( ccl_count( state->ccl ) == 1 )
				*str = pstr_append_str( *str,
						ccl_to_str( state->ccl, TRUE ), TRUE );
			else
			{
				*str = pstr_append_char( *str, '[' );
				*str = pstr_append_str( *str,
						ccl_to_str( state->ccl, TRUE ), TRUE );
				*str = pstr_append_char( *str, ']' );
			}
			*/
			pregex_ccl_to_REGEX( str, state->ccl, TRUE );
		}
		else
			break;
		
		if( !state )
			break;
				
		if( state->next )
			state = state->next;
		else if( state->next2 )
			state = state->next2;
	}
	
	return state;
}

uchar* pregex_nfa_to_regex( pregex_nfa* nfa )
{
	uchar*	str		= (uchar*)NULL;
	if( !nfa )
		return (uchar*)NULL;

	pregex_nfa_to_REGEX( &str, nfa,
		(pregex_nfa_st*)list_access( nfa->states ), 0 );
		
	return str;
}
#endif

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_print()

	Author:			Jan Max Meyer

	Usage:			Prints an NFA for debug purposes on an output stream.

	Parameters:		FILE*		stream		The output stream where to print
											the NFA to.
					pregex_nfa*	nfa			Pointer to the NFA to be output.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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
			ccl_print( stderr, s->ccl, 0 );
		fprintf( stderr, "\n\n" );
	}

	fprintf( stderr, "-----------------------------------\n" );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_free()

	Author:			Jan Max Meyer

	Usage:			Frees and resets a NFA-state machine.

	Parameters:		pregex_nfa*	nfa			A pointer to the NFA-machine to be
											freed and reset.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pregex_nfa_free( pregex_nfa* nfa )
{
	LIST*			l;
	pregex_nfa_st*	nfa_st;

	PROC( "pregex_nfa_free" );
	PARMS( "nfa", "%p", nfa );

	MSG( "Clearing states" );
	for( l = nfa->states; l; l = list_next( l ) )
	{
		nfa_st = (pregex_nfa_st*)list_access( l );
		if( nfa_st->ccl )
			ccl_free( nfa_st->ccl );

		pfree( nfa_st );
	}

	MSG( "Clearing empty state stack" );
	for( l = nfa->empty; l; l = list_next( l ) )
	{
		nfa_st = (pregex_nfa_st*)list_access( l );
		ccl_free( nfa_st->ccl );

		pfree( nfa_st );
	}

	list_free( nfa->states );
	list_free( nfa->empty );
	
	memset( nfa, 0, sizeof( pregex_nfa ) );

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_move()

	Author:			Jan Max Meyer

	Usage:			Performs a move operation on a given input character from a
					set of NFA states.

	Parameters:		LIST*		input			List of input NFA states.
					pchar		from			Character-range begin from which
												the move-operation should be
												processed on.
					pchar		to				Character-range end until
												the move-operation should be
												processed.

	Returns:		LIST*		Pointer to the result of the move operation.
								If this is (LIST*)NULL, there is no possible
								transition on the given input character.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
	30.05.2010	Jan Max Meyer	Switched function to test for a range of
								characters instead of only one. This has been
								done due performance increason of DFA table
								construction.
----------------------------------------------------------------------------- */
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
			if( ccl_testrange( test->ccl, from, to ) )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_epsilon_closure()

	Author:			Jan Max Meyer

	Usage:			Performs an epsilon closure from a set of NFA states.

	Parameters:		pregex_nfa*		nfa			NFA state machine
					LIST*			input		List of input NFA states
					pregex_accept*	accept		Match information structure,
												which can be left empty
												(pregex_accept*)NULL.

	Returns:		LIST*		Pointer to the result of the epsilon closure
								(a new set of NFA states)
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
	18.11.2010	Jan Max Meyer	Program halt if character-node with two
								outgoing transitions recognized (this may
								happen if the NFA machine was constructed
								manually and incorrectly).
	27.08.2011	Jan Max Meyer	Added greedyness parameter.
	11.11.2011	Jan Max Meyer	Merged accept, anchor and greedyness flags
								into a new struct pregex_accept, and using it
								here now.
----------------------------------------------------------------------------- */
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

		if( accept && top->accept.accept != REGEX_ACCEPT_NONE )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_match()

	Author:			Jan Max Meyer

	Usage:			Tries to match a pattern using an NFA-machine.

	Parameters:		pregex_nfa*		nfa		The NFA machine to be executed.
					uchar*			str		A test string where the NFA should
											work on.
					psize*			len		Length of the match, -1 on error or
											no match.
					int*			anchors	Returns the anchor configuration
											of the matching state, if it
											provides anchors. If this is
											(int*)NULL, anchors will be
											ignored.
					pregex_result**	ref		Return array of references; If this
											pointer is not NULL, the function
											will allocate memory for a refer-
											ence array. This array is only
											allocated if the following dependen
											cies are met:
											
											1. The NFA has references
											2. ref_count is zero
											3. ref points to a pregex_result*
											
					int*			ref_count Retrieves the number of
											references.
											This value MUST be zero, if the
											function should allocate refs.
											A positive value indicates the
											number of elements in ref, so the
											array can be re-used in multiple
											calls.
					int				flags	Flags to modify regular expression
											behavior.

	Returns:		int						REGEX_ACCEPT_NONE, if no match was
											found, else the number of the
											bestmost (=longes) match.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_nfa_match( pregex_nfa* nfa, uchar* str, psize* len, int* anchors,
		pregex_result** ref, int* ref_count, int flags )
{
	LIST*			res			= (LIST*)NULL;
	LIST*			l;
	pregex_nfa_st*	st;
	uchar*			pstr		= str;
	psize			plen		= 0;
	int				last_accept = REGEX_ACCEPT_NONE;
	int				rc;
	pchar			ch;
	pregex_accept	accept;

	PROC( "pregex_nfa_match" );
	PARMS( "nfa", "%p", nfa );
	if( flags & REGEX_MOD_WCHAR )
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
		*anchors = REGEX_ANCHOR_NONE;

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
		if( accept.accept > REGEX_ACCEPT_NONE )
		{
			MSG( "New accepting state takes place!" );
			last_accept = accept.accept;
			*len = plen;

			if( anchors )
				*anchors = accept.anchors;

			VARS( "last_accept", "%d", last_accept );
			VARS( "*len", "%d", *len );

			if( !( flags & REGEX_MOD_GREEDY ) )
			{
				VARS( "accept.greedy", "%s", BOOLEAN_STR( accept.greedy ) );
				if(	!accept.greedy || ( flags & REGEX_MOD_NONGREEDY ) )
				{
					MSG( "Greedy is set, will stop recognition with "
							"this match" );
					break;
				}
			}
		}

		if( flags & REGEX_MOD_WCHAR )
		{
			VARS( "pstr", "%ls", (pchar*)pstr );
			ch = *((pchar*)pstr);
		}
		else
		{
			VARS( "pstr", "%s", pstr );
#ifdef UTF8
			ch = u8_char( pstr );
#else
			ch = *pstr;
#endif
		}

		VARS( "ch", "%d", ch );
		VARS( "ch", "%lc", ch );

		res = pregex_nfa_move( nfa, res, ch, ch );

		if( flags & REGEX_MOD_WCHAR )
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_from_string()
	
	Author:			Jan Max Meyer
	
	Usage:			Builds or extends an NFA from a string. The string is
					simply converted into a state machine that exactly matches
					the desired string.
					
	Parameters:		pregex_nfa*		nfa			Pointer to the NFA state machine
												to be extended.
					uchar*			str			The sequence of chatacters to
												be converted one-to-one into an
												NFA.
					int				flags		Flags 
					int				acc			Acceping identifier
																	
	Returns:		int							ERR_OK on success,
												ERR... error code else
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_nfa_from_string( pregex_nfa* nfa, uchar* str, int flags, int acc )
{
	pregex_nfa_st*	nfa_st;
	pregex_nfa_st*	append_to;
	pregex_nfa_st*	first_nfa_st;
	pregex_nfa_st*	prev_nfa_st;
	uchar*			pstr;
	wchar			ch;

	PROC( "pregex_nfa_from_string" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );
	PARMS( "acc", "%d", acc );

	if( !( nfa && str ) )
		RETURN( ERR_PARMS );

	/* For wide-character execution, copy string content */
	if( flags & REGEX_MOD_WCHAR )
	{
		if( !( str = pchar_to_uchar( (pchar*)str, FALSE ) ) )
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
			pregex_nfa_create_state( nfa, (uchar*)NULL, flags ) ) )
		RETURN( ERR_MEM );

	for( pstr = str; *pstr; )
	{
		/* Then, create all states that form the string */
		MSG( "Adding new state" );
		VARS( "pstr", "%s", pstr );
		if( !( nfa_st = pregex_nfa_create_state(
				nfa, (uchar*)NULL, flags ) ) )
			RETURN( ERR_MEM );

		ch = u8_parse_char( &pstr );

		VARS( "ch", "%d", ch );
		if( !( nfa_st->ccl = ccl_addrange( (CCL)NULL, ch, ch ) ) )
			RETURN( ERR_MEM );

		/* Is case-insensitive flag set? */
		if( flags & REGEX_MOD_INSENSITIVE )
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
				ch = (uchar)tolower( (int)ch );
			else
				ch = (uchar)toupper( (int)ch );
#endif

			MSG( "Case-insensity set, new character evaluated is:" );
			VARS( "ch", "%d", ch );
			if( !( nfa_st->ccl = ccl_addrange( nfa_st->ccl, ch, ch ) ) )
				RETURN( ERR_MEM );
		}

		prev_nfa_st->next = nfa_st;
		prev_nfa_st = nfa_st;
	}

	/* Add accepting node */
	VARS( "acc", "%d", acc );
	if( !( nfa_st = pregex_nfa_create_state( nfa,
			(uchar*)NULL, flags ) ) )
		RETURN( ERR_MEM );

	nfa_st->accept.accept = acc;
	prev_nfa_st->next = nfa_st;

	/* Append to existing machine, if required */
	VARS( "append_to", "%p", append_to );
	if( append_to )
		append_to->next2 = first_nfa_st;

	/* Free copied string, in wide character mode */
	if( flags & REGEX_MOD_WCHAR )
		pfree( str );
		
	RETURN( ERR_OK );
}

#if 0
/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_compile_to_nfa()

	Author:			Jan Max Meyer

	Usage:			Compiles a regular expression into a NFA, to be processed
					for pattern matching or DFA generation.

	Parameters:		uchar*		str			The regular expression pattern
											to be compiled into an NFA.
					pregex_nfa*	nfa			A pointer to the NFA-machine to be
											generated and/or extended.
					int			accept		Identifying number for the accepting
											state if the expression is matched.

	Returns:		int						ERR_OK on no error.
											ERR_FAILURE on parse error.
											other ERR_-define else.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_compile_to_nfa( uchar* str, pregex_nfa* nfa, int flags, int accept )
{
	int		ref;
	int		ret;
	int		anchor		= REGEX_ANCHOR_NONE;
	pregex_nfa_st*	estart;
	pregex_nfa_st*	eend;
	pregex_nfa_st*	first;
	pregex_nfa_st*	last;
	pregex_nfa_st*	nfirst;
	uchar*	pstr;
	BOOLEAN	greedy		= TRUE;

	if( !( str && nfa && accept >= 0 ) )
		return ERR_PARMS;
	
	/*  REGEX_MOD_STATIC_STRING is set, convert the entire regex pattern as
		a static string matching pattern into the NFA */
	if( flags & REGEX_MOD_STATIC_STRING )
		return pregex_nfa_from_string( nfa, str, flags, accept );
		
	/* Copy input string - this is required,
		because of memory modification during the parse */
	if( flags & REGEX_MOD_WCHAR )
	{
		if( !( pstr = str = pchar_to_uchar( (pchar*)str, FALSE ) ) )
			return ERR_MEM;
	}
	else
	{
		if( !( pstr = str = pstrdup( str ) ) )
			return ERR_MEM;
	}
	
	/* Find last first node ;) ... */
	for( nfirst = (pregex_nfa_st*)list_access( nfa->states );
		nfirst && nfirst->next2; nfirst = nfirst->next2 )
			;

	/* Reference counter */
	nfa->ref_cur = nfa->ref_count++;
	
	/* Create first epsilon node */
	if( !( first = pregex_nfa_create_state( nfa,
			(uchar*)NULL, flags ) ) )
	{
		pfree( str );
		return ERR_MEM;
	}
	
	/* Parse anchor at begin of regular expression */
	if( !( flags & REGEX_MOD_NO_ANCHORS ) )
	{
		if( *pstr == '^' )
		{
			anchor |= REGEX_ANCHOR_BOL;
			pstr++;
		}
		else if( !pstrncmp( pstr, "\\<", 2 ) )
				/* This is a GNU-like extension */
		{
			anchor |= REGEX_ANCHOR_BOW;
			pstr += 2;
		}
	}

	/* Run the regex parser */
	if( ( ret = parse_alter( &pstr, nfa, &estart, &eend, &greedy, flags ) )
			!= ERR_OK )
	{
		pfree( str );
		return ( ret > ERR_OK ) ? ERR_FAILURE : ret;
	}

	/* Parse anchor at end of regular expression */
	if( !( flags & REGEX_MOD_NO_ANCHORS ) )
	{
		if( !pstrcmp( pstr, "$" ) )
			anchor |= REGEX_ANCHOR_EOL;
		else if( !pstrcmp( pstr, "\\>" ) )
			/* This is a GNU-like extension */
			anchor |= REGEX_ANCHOR_EOW;
	}
	
	/* estart is next of first */
	first->next = estart;

	/* Accept */
	eend->accept = accept;
	eend->anchor = anchor;

	/* Greedyness */
	if( flags & REGEX_MOD_GREEDY )
		eend->greedy = TRUE;
	else if( flags & REGEX_MOD_NONGREEDY )
		eend->greedy = FALSE;
	else
		eend->greedy = greedy;

	/* Chaining into big machine */
	if( nfirst )
		nfirst->next2 = first;
		
	/* Free copied string */
	pfree( str );
	return ERR_OK;
}

/*COD_ON*/

/******************************************************************************
 *      RECURSIVE DESCENT PARSER FOR REGULAR EXPRESSIONS FOLLOWS HERE...      *
 ******************************************************************************/

static int parse_char( uchar** pstr, pregex_nfa* nfa,
	pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags )
{
	int			ret;
	wchar		single;
	uchar		restore;
	uchar*		zero;
	pboolean	neg		= FALSE;
	int			ref		= nfa->ref_cur;
	
	switch( **pstr )
	{
		case '(':
			INC( *pstr );

			nfa->ref_cur++;
			nfa->ref_count++;

			if( ( ret = parse_alter( pstr, nfa, start, end, greedy, flags ) )
					!= ERR_OK )
				return ret;

			/* Patch the last transition to previous reference */
			(*end)->ref = --nfa->ref_cur;

			if( **pstr != ')' && !( flags & REGEX_MOD_NO_ERRORS ) )
				return 1;

			INC( *pstr );

			break;

		case '.':
			/* 
				If ANY_CHAR is used, then greedyness should be set to
				non-greedy by default
			*/
			*greedy = FALSE;

			if( !( *start = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;
				
			if( !( (*start)->ccl = ccl_addrange(
					(*start)->ccl, CCL_MIN + 1, CCL_MAX ) ) )
				return ERR_MEM;

			if( !( *end = pregex_nfa_create_state( nfa,
					(uchar*)NULL, flags ) ) )
				return ERR_MEM;
		
			(*start)->next = *end;
			
			INC( *pstr );
			break;

		case '[':
			if( ( zero = pstrchr( (*pstr)+1, ']' ) ) )
			{
				restore = *zero;
				*zero = '\0';

				if( (*pstr) + 1 < zero && *((*pstr)+1) == '^' )
				{
					neg = TRUE;
					(*pstr)++;
				}

				if( !( *start = pregex_nfa_create_state(
						nfa, (*pstr)+1, flags ) ) )
					return ERR_MEM;

				if( !( *end = pregex_nfa_create_state(
						nfa, (uchar*)NULL, flags ) ) )
					return ERR_MEM;
				(*start)->next = *end;

				if( neg )
					ccl_negate( (*start)->ccl );

				*zero = restore;
				*pstr = zero + 1;
				break;
			}

		default:
			*pstr += pstr_char( &single, *pstr, TRUE );
			
			if( !( *start = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;

			if( !( ( *start )->ccl = ccl_add( ( *start )->ccl, single ) ) )
				return ERR_MEM;
				
			if( !( *end = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;

			(*start)->next = *end;
			break;
	}

	return ERR_OK;
}

static int parse_factor( uchar** pstr, pregex_nfa* nfa,
	pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags )
{
	int				ret;
	pregex_nfa_st*	fstart;
	pregex_nfa_st*	fend;

	if( ( ret = parse_char( pstr, nfa, start, end, greedy, flags ) )
			!= ERR_OK )
		return ret;

	switch( **pstr )
	{
		case '*':
		case '+':
		case '?':

			if( !( fstart = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;
			if( !( fend = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;

			fstart->next = *start;
			(*end)->next = fend;

			switch( **pstr )
			{
				case '*':
					/*
								________________________
							   |                        |
							   |                        v
							fstart -> start -> end -> fend
										^       |
										|_______|
					*/
					fstart->next2 = fend;
					(*end)->next2 = *start;
					break;

				case '+':
					/*
							fstart -> start -> end -> fend
										^       |
										|_______|
					*/
					(*end)->next2 = *start;
					break;

				case '?':
					/*
								________________________
							   |                        |
							   |                        v
							fstart -> start -> end -> fend
					*/
					fstart->next2 = fend;
					break;
				
				default:
					break;
			}

			*start = fstart;
			*end = fend;
			
			INC( *pstr );
			break;

		default:
			break;
	}

	return ERR_OK;
}

static int parse_sequence( uchar** pstr, pregex_nfa* nfa,
	pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags )
{
	int				ret;
	pregex_nfa_st*	sstart;
	pregex_nfa_st*	send;

	if( ( ret = parse_factor( pstr, nfa, start, end, greedy, flags ) )
			!= ERR_OK )
		return ret;


	while( !( **pstr == '|' || **pstr == ')' || **pstr == '\0' ) )
	{
		if( !( flags & REGEX_MOD_NO_ANCHORS ) )
		{
			if( !pstrcmp( *pstr, "$" ) || !pstrcmp( *pstr, "\\>" ) )
				break;
		}

		if( ( ret = parse_factor( pstr, nfa, &sstart, &send, greedy, flags ) )
				!= ERR_OK )
			return ret;
		
		memcpy( *end, sstart, sizeof( pregex_nfa_st ) );	
		memset( sstart, 0, sizeof( pregex_nfa_st ) );

		nfa->states = list_remove( nfa->states, (void*)sstart );
		if( !( nfa->empty = list_push( nfa->empty, (void*)sstart ) ) )
			return ERR_MEM;

		*end = send;
	}
	
	return ERR_OK;
}

static int parse_alter( uchar** pstr, pregex_nfa* nfa,
	pregex_nfa_st** start, pregex_nfa_st** end, BOOLEAN* greedy, int flags )
{
	int				ret;
	pregex_nfa_st*	astart;
	pregex_nfa_st*	aend;
	pregex_nfa_st*	alter;

	if( ( ret = parse_sequence( pstr, nfa, start, end, greedy, flags ) )
			!= ERR_OK )
		return ret;

	while( **pstr == '|' )
	{
		INC( *pstr );

		if( ( ret = parse_sequence( pstr, nfa, &astart, &aend, greedy, flags ) )
				!= ERR_OK )
			return ret;

		if( !( alter = pregex_nfa_create_state(
				nfa, (uchar*)NULL, flags ) ) )
			return ERR_MEM;
		alter->next = *start;
		alter->next2 = astart;

		*start = alter;

		if( !( alter = pregex_nfa_create_state(
				nfa, (uchar*)NULL, flags ) ) )
			return ERR_MEM;
		(*end)->next = alter;
		aend->next2 = alter;

		(*end) = alter;
	}

	return ERR_OK;
}
#endif
