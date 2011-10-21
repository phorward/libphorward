/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	ref.c
Author:	Jan Max Meyer
Usage:	Utility functions for reference handling in both NFA and DFA state
		machines
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */

/*
 * Functions
 */
 
 /*NO_DOC*/

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_ref_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a reference pointer array, according to the
					maximum count of reference pointers and flags.
					
	Parameters:		pregex_result**	ref		Return array of references; If this
											pointer is not NULL, the function
											will allocate memory for a refer-
											ence array. This array is only
											allocated if the following dependen
											cies are met:
											
											1. ref_all is not zero
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
					int				ref_all Specifies the number of
											references the entire machine
											will contain (dfa->ref_count or
											nfa->ref_count).
					int				flags	Flags 

	Returns:		int						ERR_OK on success,
											ERR... error code else
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_ref_init( pregex_result** ref, int* ref_count,
						int ref_all, int flags )
{
	PROC( "pregex_ref_init" );
	PARMS( "ref", "%p", ref );
	PARMS( "ref_count", "%p", ref_count );
	PARMS( "ref_all", "%d", ref_all );
	PARMS( "flags", "%d", flags );

	/* If ref-pointer is set, then use it! */
	if( ref && ref_all && !( flags & REGEX_MOD_NO_REFERENCES ) )
	{
		VARS( "*ref_count", "%d", *ref_count );
		VARS( "ref_all", "%d", ref_all );

		if( ( *ref_count != ref_all ) )
		{
			MSG( "Allocating reference array" );
			if( !( *ref = (pregex_result*)pmalloc(
						ref_all * sizeof( pregex_result ) ) ) )
			{
				MSG( "Can't allocate references array" );
				RETURN( ERR_MEM );
			}
		}
		
		MSG( "Initalizing reference array to zero" );
		memset( *ref, 0, ref_all * sizeof( pregex_result ) );
		*ref_count = ref_all;
	}

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_ref_update()
	
	Author:			Jan Max Meyer
	
	Usage:			Updates a reference according to provided string pointer
					and offset information.
					
	Parameters:		pregex_result*	ref		Pointer to reference (from the
											array created with above function)
					uchar*			strp	Current string pointer within the
											currently parsed string.
					psize			off		Current offset within the parsed
											string.

	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pregex_ref_update( pregex_result* ref, uchar* strp, psize off )
{
	PROC( "pregex_ref_update" );
	PARMS( "ref", "%p", ref );
	PARMS( "strp", "%s", strp );
	PARMS( "off", "%ld",off );

	VARS( "ref->begin", "%p", ref->begin );
	
	if( !ref->begin )
	{
		ref->begin = strp;
		ref->pbegin = (pchar*)strp;
		ref->pos = off;
	}
	
	ref->end = strp;
	ref->pend = (pchar*)strp;
	ref->len = off - ref->pos + 1;

	VOIDRET;
}

/*COD_ON*/

