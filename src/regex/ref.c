/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ref.c
Author:	Jan Max Meyer
Usage:	Utility functions for reference handling in both NFA and DFA state
		machines
----------------------------------------------------------------------------- */

#include <phorward.h>

/*
	The functions in here are internally handled,
	and should not be covered when the documentation
	is generated.
*/

/*NO_DOC*/

/** Initializes a reference pointer array, according to the maximum count of
reference pointers and flags.

//ref// is the return array of references; If this pointer is not NULL, the
function will allocate memory for a reference array. This array is only
allocated if the following dependencies are met:

# //ref_all// is not zero
# //ref_count// is zero
# //ref// points to a pregex_range*-pointer receiving the address
#

//ref_count// is retrieves the number of references. This value MUST be zero,
if the function should allocate refs. A positive value indicates the number of
elements in ref, so the array can be re-used in multiple calls.

//ref_all// is the specifies the number of references the entire machine will
contain (dfa->ref_count or nfa->ref_count).

//flags// are flags for reference-related options (currently only the flag
PREGEX_MOD_NO_REF is tested).

Returns ERR_OK on success, or a standard error define else.
*/
int pregex_ref_init( pregex_range** ref, int* ref_count,
						int ref_all, int flags )
{
	PROC( "pregex_ref_init" );
	PARMS( "ref", "%p", ref );
	PARMS( "ref_count", "%p", ref_count );
	PARMS( "ref_all", "%d", ref_all );
	PARMS( "flags", "%d", flags );

	/* If ref-pointer is set, then use it! */
	if( ref && ref_all && !( flags & PREGEX_MOD_NO_REF ) )
	{
		VARS( "*ref_count", "%d", *ref_count );
		VARS( "ref_all", "%d", ref_all );

		if( ( *ref_count != ref_all ) )
		{
			MSG( "Allocating reference array" );
			if( !( *ref = (pregex_range*)pmalloc(
						ref_all * sizeof( pregex_range ) ) ) )
			{
				MSG( "Can't allocate references array" );
				RETURN( ERR_MEM );
			}
		}

		MSG( "Initalizing reference array to zero" );
		memset( *ref, 0, ref_all * sizeof( pregex_range ) );
		*ref_count = ref_all;
	}

	RETURN( ERR_OK );
}

/** Updates a reference according to provided string pointer and offset
information.

//ref// is the pointer to reference (from the array created with the function
pregex_ref_init()).
//strp// is the current string pointer within the currently parsed string.
//off// is the current offset within the parsed string.
*/
void pregex_ref_update( pregex_range* ref, uchar* strp, psize off )
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

