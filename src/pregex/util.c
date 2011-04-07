/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009, 2010 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

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
 
 /*NO_DOC*/

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

	nfa_st->accept = acc;
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_check_anchors()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs an anchor checking within a string.
					
	Parameters:		uchar*			all			The entire string. This can
												be equal to str, but is
												required to perform valid
												line-begin anchor checking.
												If 'all' is (uchar*)NULL,
												'str' is assumed as 'all'.
					uchar*			str			Pointer of the current match
												within entire.
					psize			len			Length of the matched string,
												in characters.
					int				anchors		Anchor configuration to be
												checked for the string.
					int				flags		Flags configuration, e. g.
												for wide-character enabled
												anchor checking.
																	
	Returns:		pboolean					TRUE, if all anchors match,
												FALSE else.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean pregex_check_anchors( uchar* all, uchar* str, psize len,
										int anchors, int flags )
{
	pchar	ch;
	int		charsize = sizeof( uchar );

	PROC( "pregex_check_anchors" );
	if( flags & REGEX_MOD_WCHAR )
	{
		PARMS( "all", "%ls", all );
		PARMS( "str", "%ls", str );
	}
	else
	{
		PARMS( "all", "%s", all );
		PARMS( "str", "%s", str );
	}
	PARMS( "anchors", "%d", anchors );
	PARMS( "flags", "%d", flags );

	/* Perform anchor checkings? */
	if( flags & REGEX_MOD_NO_ANCHORS || anchors == REGEX_ANCHOR_NONE )
	{
		MSG( "Anchor checking is disabled trough flags, or not required" );
		RETURN( TRUE );
	}

	/* Check parameter integrity */
	if( !( str || len ) )
	{
		MSG( "Not enough or wrong parameters!" );
		RETURN( FALSE );
	}

	if( !all )
		all = str;

	if( flags & REGEX_MOD_WCHAR )
		charsize = sizeof( pchar );

	/* Begin of line anchor */
	if( anchors & REGEX_ANCHOR_BOL )
	{
		MSG( "Begin of line anchor is set" );
		if( all < str )
		{
			if( flags & REGEX_MOD_WCHAR )
			{
				VARS( "str-1", "%ls", (pchar*)( str - 1 ) );
				ch = *( (pchar*)( str - 1 ) );
			}
			else
			{
				VARS( "str-1", "%s", str-1 );
				ch = *( str - 1 );
			}

			VARS( "ch", "%lc", ch );
			if( ch != '\n' && ch != '\r' )
				RETURN( FALSE );
		}
	}

	/* End of Line anchor */
	if( anchors & REGEX_ANCHOR_EOL )
	{
		MSG( "End of line anchor is set" );
		if( ( ch = *( str + ( len * charsize ) ) ) )
		{
			VARS( "ch", "%lc", ch );
			if( ch != '\n' && ch != '\r' )
				RETURN( FALSE );
		}
	}

	/* Begin of word anchor */
	if( anchors & REGEX_ANCHOR_BOW )
	{
		MSG( "Begin of word anchor is set" );
		if( all < str )
		{
			if( flags & REGEX_MOD_WCHAR )
			{
				VARS( "str-1", "%ls", (pchar*)( str - 1 ) );
				ch = *( (pchar*)( str - 1 ) );
			}
			else
			{
				VARS( "str-1", "%s", str-1 );
				ch = *( str - 1 );
			}

			VARS( "ch", "%lc", ch );
			if( Pisalnum( ch ) || ch == '_' )
				RETURN( FALSE );
		}
	}

	/* End of word anchor */
	if( anchors & REGEX_ANCHOR_EOW )
	{
		MSG( "End of word anchor is set" );
		if( ( ch = *( str + ( len * charsize ) ) ) )
		{
			VARS( "ch", "%lc", ch );
			if( Pisalnum( ch ) || ch == '_' )
				RETURN( FALSE );
		}
	}

	MSG( "All anchor tests where fine!" );
	RETURN( TRUE );
}

/*COD_ON*/

