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

	/* Find node to integrate into existing machine */
	for( append_to = (pregex_nfa_st*)list_access( nfa->states );
		append_to && append_to->next2; append_to = append_to->next2 )
			/* Find last first node ;) ... */ ;

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

	nfa_st->accept = acc;
	prev_nfa_st->next = nfa_st;

	/* Append to existing machine, if required */
	VARS( "append_to", "%p", append_to );
	if( append_to )
		append_to->next2 = first_nfa_st;
		
	RETURN( ERR_OK );
}

