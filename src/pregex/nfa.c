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
#define PREGEX_LOCAL
#include "pregex.h"

/*
 * Global variables
 */
#define NFA_ST				pregex_nfa_st
#define INC( i )			(i)++
#define VALID_CHAR( ch )	!pstrchr( "|()[]*+?", (ch) )

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
	ptr->accept = REGEX_ACCEPT_NONE;
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
	Function:		pregex_nfa_print()

	Author:			Jan Max Meyer

	Usage:			Prints an NFA for debug purposes on a output stream.

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

		fprintf( stderr, "#% 2d % 4d % 5d  % 5d  % 3d % 6d\n",
			list_find( nfa->states, (void*)s ),
			list_find( nfa->states, (void*)s->next ),
			list_find( nfa->states, (void*)s->next2 ),
			s->accept, s->ref, s->anchor );
		
		if( s->ccl )
			ccl_print( stderr, s->ccl, 0 );
		fprintf( stderr, "\n\n" );
	}

	fprintf( stderr, "---------------------\n" );
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

	Parameters:		pregex_nfa*	nfa				NFA state machine
					LIST*		input			List of input NFA states
					int*		accept			Return-pointer to a variable
												to retrieve a possible
												accept-id. Can be left
												(int*)NULL, so accept will
												not be returned.
					int*		anchors			Return-pointer to a variable
												to retrieve a possible
												anchor-configuration. This
												pointer is associated with
												accept, so it can even be left
												(int*)NULL, to not be returned.

	Returns:		LIST*		Pointer to the result of the epsilon closure
								(a new set of NFA states)
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
	18.11.2010	Jan Max Meyer	Program halt if character-node with two
								outgoing transitions recognized (this may
								happen if the NFA machine was constructed
								manually and incorrectly).
----------------------------------------------------------------------------- */
LIST* pregex_nfa_epsilon_closure( pregex_nfa* nfa, LIST* input,
			int* accept, int* anchors )
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
	PARMS( "anchors", "%p", anchors );

	if( accept )
		*accept = REGEX_ACCEPT_NONE;

	if( anchors )
		*anchors = REGEX_ANCHOR_NONE;

	stack = list_dup( input );


	/* Loop trough the items */
	while( stack != (LIST*)NULL )
	{
		stack = list_pop( stack, (void**)&top );

		if( accept && top->accept != REGEX_ACCEPT_NONE )
		{
			if( !last_accept || last_accept < top )
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
		*accept = last_accept->accept;
		VARS( "*accept", "%d", *accept );
	}

	if( anchors && last_accept )
	{
		*anchors = last_accept->anchor;
		VARS( "*anchors", "%d", *anchors );
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
	LIST*		res			= (LIST*)NULL;
	LIST*		l;
	NFA_ST*		st;
	uchar*		pstr		= str;
	psize		plen		= 0;
	int			accept		= REGEX_ACCEPT_NONE;
	int			last_accept = REGEX_ACCEPT_NONE;
	int			rc;
	pchar		ch;

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
		res = pregex_nfa_epsilon_closure( nfa, res, &accept, anchors );
		
		MSG( "Handling References" );
		if( ref && nfa->ref_count )
		{
			LISTFOR( res, l )
			{
				st = (NFA_ST*)list_access( l );
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

		VARS( "accept", "%d", accept );
		if( accept > REGEX_ACCEPT_NONE )
		{
			MSG( "New accepting state takes place!" );
			last_accept = accept;
			*len = plen;

			VARS( "last_accept", "%d", last_accept );
			VARS( "*len", "%d", *len );
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
	NFA_ST*	estart;
	NFA_ST*	eend;
	NFA_ST*	first;
	NFA_ST*	last;
	NFA_ST*	nfirst;
	uchar*	pstr;

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
	for( nfirst = (NFA_ST*)list_access( nfa->states );
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
	if( ( ret = parse_alter( &pstr, nfa, &estart, &eend, flags ) )
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

	/* Build references; If eend has already one reference, 
			append one more epsilon node */
	/*
	if( eend->ref != ( first->ref = nfa->ref_cur ) )
	{
		if( !( last = pregex_nfa_create_state( nfa,
				(uchar*)NULL, flags ) ) )
		{
			pfree( str );
			return ERR_MEM;
		}
		
		eend->next = last;
		eend = last;
	}
	*/

	/* Accept */
	eend->accept = accept;
	eend->anchor = anchor;

	/* Chaining into big machine */
	if( nfirst )
		nfirst->next2 = first;
		
	/* Free copied string */
	pfree( str );
	return ERR_OK;
}

/*COD_ON*/

/******************************************************************************
 * RECURSIVE DESCENT PARSER FOR REGULAR EXPRESSIONS FOLLOWS HERE...           *
 ******************************************************************************/

PRIVATE int parse_char( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end, int flags )
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

			if( ( ret = parse_alter( pstr, nfa, start, end, flags ) )
					!= ERR_OK )
				return ret;

			/* Patch last transition to previous reference */
			(*end)->ref = --nfa->ref_cur;

			if( **pstr != ')' && !( flags & REGEX_MOD_NO_ERRORS ) )
				return 1;

			INC( *pstr );

			/*

			if( !( *start = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;
				
			(*start)->next = cstart;
			(*start)->ref = ref;
				
			if( !( *end = pregex_nfa_create_state(
					nfa, (uchar*)NULL, flags ) ) )
				return ERR_MEM;
				
			cend->next = *end;
			(*end)->ref = ref;
			*/
			break;

		case '.':
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

PRIVATE int parse_factor( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end, int flags )
{
	int		ret;
	NFA_ST*	fstart;
	NFA_ST*	fend;

	if( ( ret = parse_char( pstr, nfa, start, end, flags ) )
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

		default:
			break;
	}

	return ERR_OK;
}

PRIVATE int parse_sequence( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end, int flags )
{
	int		ret;
	NFA_ST*	sstart;
	NFA_ST*	send;

	if( ( ret = parse_factor( pstr, nfa, start, end, flags ) )
			!= ERR_OK )
		return ret;


	while( !( **pstr == '|' || **pstr == ')' || **pstr == '\0' ) )
	{
		if( !( flags & REGEX_MOD_NO_ANCHORS ) )
		{
			if( !pstrcmp( *pstr, "$" ) || !pstrcmp( *pstr, "\\>" ) )
				break;
		}

		if( ( ret = parse_factor( pstr, nfa, &sstart, &send, flags ) )
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

PRIVATE int parse_alter( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end, int flags )
{
	int		ret;
	NFA_ST*	astart;
	NFA_ST*	aend;
	NFA_ST*	alter;

	if( ( ret = parse_sequence( pstr, nfa, start, end, flags ) )
			!= ERR_OK )
		return ret;

	while( **pstr == '|' )
	{
		INC( *pstr );

		if( ( ret = parse_sequence( pstr, nfa, &astart, &aend, flags ) )
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

