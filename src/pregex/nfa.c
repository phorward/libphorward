/* -MODULE----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	nfa.c
Author:	Jan Max Meyer
Usage:	NFA creation and executable functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
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

	Returns:		pregex_nfa_st*				Pointer to the created state.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, uchar* chardef )
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

	memset( ptr, 0, sizeof( pregex_nfa_st ) );
	ptr->accept = REGEX_ACCEPT_NONE;

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
			RETURN( 0 );
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

	fprintf( stderr, " no next next2 accept\n" );
	fprintf( stderr, "---------------------\n" );

	for( l = nfa->states; l; l = list_next( l ) )
	{
		s = (pregex_nfa_st*)list_access( l );

		fprintf( stderr, "#% 2d % 4d % 5d  % 5d\n",
			list_find( nfa->states, (void*)s ),
			list_find( nfa->states, (void*)s->next ),
			list_find( nfa->states, (void*)s->next2 ),
			s->accept );
		
		if( s->ccl )
			ccl_print( stderr, s->ccl );
		fprintf( stderr, "\n" );
	}

	fprintf( stderr, "---------------------\n" );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_move()

	Author:			Jan Max Meyer

	Usage:			Performs a move operation on a given input character from a
					set of NFA states.

	Parameters:		LIST*		input			List of input NFA states.
					u_int		ch				Character where move-operation
												should be processed on.

	Returns:		LIST*		Pointer to the result of the move operation.
								If this is (LIST*)NULL, there is no possible
								transition on the given input character.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
----------------------------------------------------------------------------- */
LIST* pregex_nfa_move( pregex_nfa* nfa, LIST* input, u_int ch )
{
	LIST*			hits	= (LIST*)NULL;
	pregex_nfa_st*	test;

	PROC( "pregex_nfa_move" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "input", "%p", input );
	PARMS( "ch", "%d", ch );

	/* Loop trough the input items */
	while( input != (LIST*)NULL )
	{
		input = list_pop( input, (void**)&test );
		VARS( "input", "%p", input );
		VARS( "test", "%p", test );

		/* Not an epsilon edge? */
		if( test->ccl )
		{
			if( ccl_test( test->ccl, ch ) )
			{
				MSG( "State matches character!" );
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
												(u_int*)NULL, so accept will
												not be returned.

	Returns:		LIST*		Pointer to the result of the epsilon closure
								(a new set of NFA states)
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
----------------------------------------------------------------------------- */
LIST* pregex_nfa_epsilon_closure( pregex_nfa* nfa, LIST* input, int* accept )
{
	pregex_nfa_st*	top;
	pregex_nfa_st*	next;
	pregex_nfa_st*	last_accept	= (pregex_nfa_st*)NULL;
	LIST*			stack;
	int				pos			= 0;
	short			i;

	PROC( "pregex_nfa_epsilon_closure" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "input", "%p", input );
	PARMS( "accept", "%p", accept );

	if( accept != (int*)NULL )
		*accept = REGEX_ACCEPT_NONE;

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
	}

	if( accept && last_accept )
	{
		*accept = last_accept->accept;
		VARS( "*accept", "%d", *accept );
	}

	RETURN( input );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_nfa_match()

	Author:			Jan Max Meyer

	Usage:			Tries to match a pattern using an NFA-machine.

	Parameters:		pregex_nfa*	nfa			The NFA machine to be executed.
					uchar*		str			A test string where the NFA should
											work on.
					int*		len			Length of the match, -1 on error or
											no match.

	Returns:		int						REGEX_ACCEPT_NONE, if no match was
											found, else the number of the
											bestmost (=longes) match.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_nfa_match( pregex_nfa* nfa, uchar* str, u_int* len )
{
	LIST*		res			= (LIST*)NULL;
	uchar*		pstr		= str;
	u_int		plen		= 0;
	int			accept		= REGEX_ACCEPT_NONE;
	int			last_accept = REGEX_ACCEPT_NONE;

	PROC( "pregex_nfa_match" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "str", "%s", str );
	PARMS( "len", "%p", len );
	
	*len = 0;
	
	res = list_push( res, list_access( nfa->states ) );

	while( res )
	{
		MSG( "Performing epsilon closure" );
		res = pregex_nfa_epsilon_closure( nfa, res, &accept );

		VARS( "accept", "%d", accept );
		if( accept > REGEX_ACCEPT_NONE )
		{
			MSG( "New accepting state takes place!" );
			last_accept = accept;
			*len = plen;

			VARS( "last_accept", "%d", last_accept );
			VARS( "*len", "%d", *len );
		}

		VARS( "pstr", "%s", pstr );
		VARS( "u8_char( pstr )", "%d", u8_char( pstr ) );

		res = pregex_nfa_move( nfa, res, u8_char( pstr ) );
		pstr += u8_seqlen( pstr );
		plen++;
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

	Returns:		int						1 if a parse error occured.
											ERR_OK on no error.
											ERR_-define else.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_compile_to_nfa( uchar* str, pregex_nfa* nfa, int accept )
{
	int		ret;
	NFA_ST*	estart;
	NFA_ST*	eend;
	NFA_ST*	first;
	uchar*	pstr;

	if( !( str && nfa && accept >= 0 ) )
		return ERR_PARMS;

	if( !( pstr = str = pstrdup( str ) ) )
		return ERR_MEM;

	if( !( first = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
	{
		pfree( str );
		return ERR_MEM;
	}

	if( ( ret = parse_alter( &pstr, nfa, &estart, &eend ) ) != ERR_OK )
	{
		pfree( str );
		return ret;
	}

	first->next = estart;
	eend->accept = accept;

	pfree( str );
	return ERR_OK;
}

/******************************************************************************
 * RECURSIVE DESCENT PARSER FOR REGULAR EXPRESSIONS FOLLOWS HERE...           *
 ******************************************************************************/

PRIVATE int parse_char( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end )
{
	int		ret;
	uchar	restore;
	uchar*	zero;

	switch( **pstr )
	{
		case '(':
			INC( *pstr );

			if( ( ret = parse_alter( pstr, nfa, start, end ) ) != ERR_OK )
				return ret;

			if( **pstr != ')' )
				return 1;

			INC( *pstr );
			break;

		case '[':
			if( ( zero = pstrchr( *(pstr+1), ']' ) ) )
			{
				restore = *zero;
				*zero = '\0';

				if( !( *start = pregex_nfa_create_state( nfa, *(pstr+1) ) ) )
					return ERR_MEM;

				if( !( *end = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
					return ERR_MEM;
				(*start)->next = *end;

				*zero = restore;
				*pstr = zero + 1;
				break;
			}

		default:
			zero = *pstr + u8_seqlen( *pstr );
			restore = *zero;
			*zero = '\0';

			if( !( *start = pregex_nfa_create_state( nfa, *pstr ) ) )
				return ERR_MEM;
			if( !( *end = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
				return ERR_MEM;

			(*start)->next = *end;

			*zero = restore;
			*pstr = zero;
			break;
	}

	return ERR_OK;
}

PRIVATE int parse_factor( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end )
{
	int		ret;
	NFA_ST*	fstart;
	NFA_ST*	fend;

	if( ( ret = parse_char( pstr, nfa, start, end ) ) != ERR_OK )
		return ret;

	switch( **pstr )
	{
		case '*':
		case '+':
		case '?':

			if( !( fstart = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
				return ERR_MEM;
			if( !( fend = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
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
	NFA_ST** start, NFA_ST** end )
{
	int		ret;
	NFA_ST*	sstart;
	NFA_ST*	send;

	if( ( ret = parse_factor( pstr, nfa, start, end ) ) != ERR_OK )
		return ret;

	while( !( **pstr == '|' || **pstr == ')' || **pstr == '\0' ) )
	{
		if( ( ret = parse_factor( pstr, nfa, &sstart, &send ) ) != ERR_OK )
			return ret;

		memcpy( *end, sstart, sizeof( pregex_nfa_st ) );
		memset( sstart, 0, sizeof( pregex_nfa_st ) );
		if( !( nfa->empty = list_push( nfa->empty, (void*)sstart ) ) )
			return ERR_MEM;

		*end = send;
	}

	return ERR_OK;
}

PRIVATE int parse_alter( uchar** pstr, pregex_nfa* nfa,
	NFA_ST** start, NFA_ST** end )
{
	int		ret;
	NFA_ST*	astart;
	NFA_ST*	aend;
	NFA_ST*	alter;

	if( ( ret = parse_sequence( pstr, nfa, start, end ) ) != ERR_OK )
		return ret;

	while( **pstr == '|' )
	{
		INC( *pstr );

		if( ( ret = parse_sequence( pstr, nfa, &astart, &aend ) ) != ERR_OK )
			return ret;

		if( !( alter = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
			return ERR_MEM;
		alter->next = *start;
		alter->next2 = astart;

		*start = alter;

		if( !( alter = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
			return ERR_MEM;
		(*end)->next = alter;
		aend->next2 = alter;

		(*end) = alter;
	}

	return ERR_OK;
}

