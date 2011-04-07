/* -MODULE----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	util.c
Author:	Jan Max Meyer
Usage:	Utility functions for additional usage that belong to the regex library
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
																	
	Returns:		int							ERR_OK on success,
												ERR... error code else
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_nfa_from_string( pregex_nfa* nfa, uchar* str )
{
	pregex_nfa_st*	nfa_st;
	pregex_nfa_st*	first_nfa_st;
	pregex_nfa_st*	prev_nfa_st;
	uchar*			pstr;

	PROC( "pregex_nfa_from_string" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "str", "%s", str );

	if( !( nfa && str ) )
		RETURN( ERR_PARMS );

	/* Create first state - this is an epsilon node */
	if( !( first_nfa_st = prev_nfa_st =
			pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
		RETURN( ERR_MEM );

	for( pstr = str; *pstr; pstr += u8_seqlen( pstr ) )
	{
		/* Then, create all states that form the string */
		MSG( "Adding new state" );
		VARS( "pstr", "%s", pstr );
		if( !( nfa_st = pregex_nfa_create_state( nfa, (uchar*)NULL ) ) )
			RETURN( ERR_MEM );

		VARS( "adding char", "%d", u8_char( pstr ) );
		if( !( nfa_st->ccl = ccl_addrange(
				(CCL)NULL, u8_char( pstr ), u8_char( pstr ) ) ) )
			RETURN( ERR_MEM );

		prev_nfa_st->next = nfa_st;
		prev_nfa_st = nfa_st;
	}

	/* Integrate into existing machine */
	for( nfa_st = (pregex_nfa_st*)list_access( nfa->states );
		nfa_st && nfa_st->next2; nfa_st = nfa_st->next2 )
			/* Find last first node ;) ... */ ;

	if( nfa_st )
		nfa_st->next = first_nfa_st;
		
	RETURN( ERR_OK );
}

