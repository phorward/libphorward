/* -MODULE----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	dfa.c
Author:	Jan Max Meyer
Usage:	DFA creation functions
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

