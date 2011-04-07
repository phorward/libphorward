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

/*
 * Functions
 */

/* Creating a new NFA */
pregex_nfa* regex_create_nfa()
{
	regex_nfa* ptr;
	
	if( !( ptr = (pregex_nfa*)MALLOC( sizeof( pregex_nfa ) ) ) )
		return (pregex_nfa*)NULL;
		
	memset( ptr, 0, sizeof( pregex_nfa ) );
	
	if( !( ptr->start = pregex_create_nfa_st( ptr ) ) )
	{
		FREE( ptr );
		return (pregex_nfa*)NULL;
	}
	
	return ptr;
}

/* Freeing a NFA */
void regex_free_nfa( regex_nfa* nfa )
{
	int		i;
	
	for( i = 0; i < nfa->states_cnt; i++ )
		regex_free_nfa_st( &( nfa->states[ i ] ) );
		
	FREE( nfa->states );
	FREE( nfa );
}

/* Creating a new NFA state */
pregex_nfa_st* regex_create_nfa_st( pregex_nfa* nfa )
{
	int 			i;
	regex_nfa_st* 	ptr;
	
	for( i = 0; i < nfa->states_cnt; i++ )
		if( nfa->states[i].edge == REGEX_EDGE_EMPTY )
		{
			nfa->states[i].edge = REGEX_EDGE_EPSILON;
			return &( nfa->states[i] );
		}
	
	if( !( nfa->states_cnt ) )
		nfa->states = (regex_nfa_st*)MALLOC( REGEX_ALLOC_STEP
			* sizeof( regex_nfa_st ) );
	else if( ( nfa->states_cnt % REGEX_ALLOC_STEP ) == 0 )
		nfa->states = (regex_nfa_st*)REALLOC( (regex_nfa_st*)nfa->states,
			( ( nfa->states_cnt / REGEX_ALLOC_STEP ) + REGEX_ALLOC_STEP )
				* sizeof( regex_nfa_st ) );
				
	if( !( nfa->states ) )
		return (regex_nfa_st*)NULL;
	
	ptr = &( nfa->states[ nfa->states_cnt ] );

	memset( ptr, 0, sizeof( regex_nfa_st ) );	
	ptr->edge = REGEX_EDGE_EPSILON;
	ptr->next = -1;
	ptr->next2 = -1;
	
	nfa->states_cnt++;
	
	return ptr;
}

/* Freeing a NFA-state; The state will be marked as EMPTY, so it can be
	re-used within a machine */
void regex_free_nfa_st( pregex_nfa_st* st )
{
	if( !st )
		return;
		
	if( st->edge == REGEX_EDGE_CCL && st->cclass )
		bitset_free( st->cclass );
	
	memset( st, 0, sizeof( regex_nfa_st ) );
	st->edge = REGEX_EDGE_EMPTY;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		regex_move_nfa()

	Author:			Jan Max Meyer

	Usage:			Performs a move operation on a given input character from a
					set of NFA states.

	Parameters:		LIST*		input			List of input NFA states.
					int			ch				Character where move-operation
												should be processed on.

	Returns:		LIST*		Pointer to the result of the move operation.
								If this is (LIST*)NULL, there is no possible
								transition on the given input character.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
----------------------------------------------------------------------------- */
PRIVATE LIST* regex_move_nfa( regex_nfa* nfa, LIST* input, int ch )
{
	LIST*			hits	= (LIST*)NULL;
	regex_nfa_st*	test;

	/* Loop trough the input items */
	while( input != (LIST*)NULL );
	{
		input = list_pop( input, &test );

		if( test->edge != REGEX_EDGE_EPSILON )
		{
			if( ( test->cclass && bitset_get( test->cclass, ch ) )
					|| test->edge == ch )
			{
				hits = list_push( hits, &( nfa->states[ test->next ] ) );
			}
		}
	}
	
	return hits;
}


/* -FUNCTION--------------------------------------------------------------------
	Function:		regex_epsilon_closure_nfa()

	Author:			Jan Max Meyer

	Usage:			Performs an epsilon closure from a set of NFA states.

	Parameters:		regex_nfa*	nfa				NFA state machine
					LIST*		input			List of input NFA states
					int*		accept			Return-pointer to a variable
												to retrieve a possible accept-id.
												Can be (int*)NULL.

	Returns:		LIST*		Pointer to the result of the epsilon closure
								(a new set of NFA states)
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	15.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
----------------------------------------------------------------------------- */
PRIVATE LIST* regex_epsilon_closure_nfa( regex_nfa* nfa, LIST* input, int* accept )
{
	regex_nfa_st*	top;
	regex_nfa_st*	next;
	LIST*			stack;
	int				pos			= 0;
	short			i;

	if( accept != (int*)NULL )
		*accept = REGEX_ACCEPT_NONE;
		
	stack = list_dup( input );

	/* Loop trough the items */
	while( stack != (LIST*)NULL )
	{
		stack = list_pop( stack, &top );

		if( accept != (int*)NULL )
		{
			if( top->accept > REGEX_ACCEPT_NONE && 
				( *accept <= REGEX_ACCEPT_NONE ||
					(int)( top - nfa->states ) < *accept ) )
				*accept = top->accept;
		}

		if( top->edge == REGEX_EDGE_EPSILON )
		{
			for( i = 0; i < 2; i++ )
			{
				next = (regex_nfa_st*)NULL;
				
				if( i == 0 && top->next > -1 )
					next = &( nfa->states[ top->next ] );
				else if( i == 1 && top->next2 > -1 )
					next = &( nfa->states[ top->next2 ] );
					
				if( list_find( input, next ) == -1 )
				{
					input = list_push( input, next );
					stack = list_push( stack, next );
				}
			}
		}
	}

	return input;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		regex_nfa_match()

	Author:			Jan Max Meyer

	Usage:			Tries to match a pattern using an NFA-machine.

	Parameters:		regex_nfa*	nfa				The NFA machine to be executed.
					uchar*		str				A test string where the NFA
												should work on.
					int*		len				Length of the match, -1 on error
												or no match.

	Returns:		int							REGEX_ACCEPT_NONE, if no match was
												found, else the number of the
												bestmost match.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	10.12.2007	Jan Max Meyer	Added new match_len-Parameter.
	16.07.2008	Jan Max Meyer	Copied from old RE-Lib to new Regex-Lib
----------------------------------------------------------------------------- */
int regex_nfa_match( regex_nfa* nfa, uchar* str, int* len )
{
	LIST*		res			= (LIST*)NULL;
	uchar*		ptr			= str;
	int			accept		= REGEX_ACCEPT_NONE;
	int			last_accept = REGEX_ACCEPT_NONE;
	
	*len = 0;
	
	res = list_push( res, nfa->start );
	while( res != (LIST*)NULL )
	{
		res = regex_epsilon_closure_nfa( nfa, res, &accept );

		if( accept > REGEX_ACCEPT_NONE )
		{
			last_accept = accept;
			*len = (int)( ptr - str );
		}

		res = regex_move_nfa( nfa, res, *ptr );
		ptr++;
	}
	
	return last_accept;
}

/* Creating a new DFA */
PRIVATE regex_dfa* regex_create_dfa( regex_nfa* nfa )
{
	regex_dfa* ptr;
	
	if( !nfa )
		return (regex_dfa*)NULL;
	
	if( !( ptr = (regex_dfa*)MALLOC( sizeof( regex_dfa ) ) ) )
		return (regex_dfa*)NULL;
		
	memset( ptr, 0, sizeof( regex_dfa ) );
	ptr->nfa = nfa;
	
	return ptr;
}

/* Dump NFA (for debug purposes!) */
void regex_print_nfa( regex_nfa* nfa )
{
	int		i;
	int		j;
	
	for( i = 0; i < nfa->states_cnt; i++ )
	{
		printf( "%03d: " );
		
		switch( nfa->states[i].edge )
		{
			case REGEX_EDGE_EPSILON:
				printf( "[EPSILON]   " );
				break;

			case REGEX_EDGE_EMPTY:
				printf( "[EMPTY]     " );
				break;

			case REGEX_EDGE_CCL:
				printf( "[CCL]       " );
				
				break;
		}
		
	}
}


/* Freeing a DFA */
void regex_free_dfa( regex_dfa* dfa, BOOLEAN even_nfa )
{
	int		i;
	
	for( i = 0; i < dfa->states_cnt; i++ )
		FREE( dfa->states[ i ].line );
		
	FREE( dfa->states );
	
	if( even_nfa )
		regex_free_nfa( dfa->nfa );

	FREE( dfa );
}

/* Creating a new DFA state */
PRIVATE regex_dfa_st* regex_create_dfa_st( regex_dfa* dfa )
{
	int 			i;
	regex_dfa_st* 	ptr;
		
	if( !( dfa->states_cnt ) )
		dfa->states = (regex_dfa_st*)MALLOC( REGEX_ALLOC_STEP
			* sizeof( regex_dfa_st ) );
	else if( ( dfa->states_cnt % REGEX_ALLOC_STEP ) == 0 )
		dfa->states = (regex_dfa_st*)REALLOC( (regex_dfa_st*)dfa->states,
			( ( dfa->states_cnt / REGEX_ALLOC_STEP ) + REGEX_ALLOC_STEP )
				* sizeof( regex_dfa_st ) );
				
	if( !( dfa->states ) )
		return (regex_dfa_st*)NULL;
	
	ptr = &( dfa->states[ dfa->states_cnt ] );

	memset( ptr, 0, sizeof( regex_dfa_st ) );
	ptr->accept = REGEX_ACCEPT_NONE;
	ptr->done = FALSE;

	if( !( ptr->line = MALLOC( dfa->nfa->alphabet * sizeof( int ) ) ) )
		return (regex_dfa_st*)NULL;

	dfa->states_cnt++;

	return ptr;
}

/* Checks the unfinished DFA state machine for states having the done-flag
	set to FALSE and returns the first occurence. If no more undone states
	are found, (regex_dfa*)NULL is returned. */
PRIVATE regex_dfa_st* regex_get_undone_dfa_st( regex_dfa* dfa )
{
	int		i;

	for( i = 0; i < dfa->states_cnt; i++ )
		if( !( dfa->states[i].done ) )
			return &( dfa->states[i] );

	return (regex_dfa_st*)NULL;
}

/* Checks for DFA-states with same NFA-epsilon transitions than the specified
	one within the DFA-state machine. If an equal item is found, the offset of
		that DFA-state is returned, else -1. */
PRIVATE int regex_dfa_st_with_same_trans( regex_dfa* dfa, LIST* trans )
{
	int		i;
	
	for( i = 0; i < dfa->states_cnt; i++ )
		if( list_diff( dfa->states[i].nfa_set, trans ) )
			return i;

	return -1;
}



/* -FUNCTION--------------------------------------------------------------------
	Function:		regex_nfa_to_dfa()
	
	Author:			Jan Max Meyer
	
	Usage:			Turns a NFA-state machine to a DFA-state machine using
					the subset-construction algorithm.
					
	Parameters:		regex_nfa*	nfa				Pointer to the NFA-Machine
												where the DFA-machine should
												be constructed from.
																	
	Returns:		regex_dfa*					Pointer to dynamically created
												DFA state machine constructed
												from the NFA state machine.
												The NFA becomes part of the
												DFA, but will also exist
												separately.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	03.06.2007	JMM				Parameter characters_cnt inserted
----------------------------------------------------------------------------- */
regex_dfa* regex_nfa_to_dfa( regex_nfa* nfa )
{
	regex_dfa*		dfa;
	LIST*			set;
	LIST*			transitions;
	LIST*			item;
	regex_dfa_st*	current;
	regex_dfa_st*	tmp;
	regex_nfa_st*	nfa_st;
	int				state_next	= 0;
	int				ch			= 0;
	
	if( !( dfa = regex_create_dfa( nfa ) ) )
		return (regex_dfa*)NULL;

	if( !( current = regex_create_dfa_st( dfa ) ) )
	{
		regex_free_dfa( dfa, FALSE );
		return (regex_dfa*)NULL;
	}
	
	/* Seed start set */
	if( set = list_push( (LIST*)NULL, nfa->start ) )
	{
		regex_free_dfa( dfa, FALSE );
		return (regex_dfa*)NULL;
	}

	current->nfa_set = regex_epsilon_closure_nfa( dfa->nfa, set, (int*)NULL );

	/* Perform algorithm until all states are done */
	while( current = regex_get_undone_dfa_st( dfa ) )
	{		
		current->done = TRUE;
		current->accept = REGEX_ACCEPT_NONE;

		for( item = current->nfa_set; item; item = item->next )
		{
			nfa_st = (regex_nfa_st*)( item->pptr );

			if( nfa_st->accept > REGEX_ACCEPT_NONE )
			{
				if( current->accept == REGEX_ACCEPT_NONE
					|| current->accept == nfa_st->accept )
					current->accept = nfa_st->accept;
			}
		}
		
		/* Make transitions on whole alphabet */
		for( ch = 0; ch < dfa->nfa->alphabet; ch++ )
		{
			transitions = list_dup( current->nfa_set );
			
			if( transitions = regex_move_nfa( dfa->nfa, transitions, ch ) )
			{
				transitions = regex_epsilon_closure_nfa( dfa->nfa, transitions , (int*)NULL );
			}
					
			if( !transitions )
				/* There is no move on this character! */
				state_next = -1;
			else if( ( state_next = regex_dfa_st_with_same_trans( dfa, transitions ) ) < 0 )
				/* This transition is already existing in the DFA
					- discard the transition table! */
				transitions = list_free( transitions );
			else
			{				
				/* Create a new DFA as undone with this transition! */
				if( !( tmp = regex_create_dfa_st( dfa ) ) )
				{
					regex_free_dfa( dfa, FALSE );
					return (regex_dfa*)NULL;
				}
				
				tmp->nfa_set = transitions;
				transitions = (LIST*)NULL;
				
				state_next = dfa->states_cnt - 1;
			}

			/* Set the transition into the transition state matrix... */
			
			/* printf( "ch = %d, state_next = %d\n", ch, state_next ); */			
			current->line[ch] = state_next;
		}
	}
	
	return dfa;
}
