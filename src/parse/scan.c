/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	scan.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Checks for a symbol //sym// in //start//.

Marks the end of a possible match by returning TRUE and writing the end position
into //end//.
*/
pboolean pp_sym_in_input( ppsym* sym, char* start, char** end )
{
	if( !( sym && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !sym->type )
		/* Not a valid symbol for recognition */
		return FALSE;

	switch( sym->type )
	{
		case PPSYMTYPE_CCL:
			if( p_ccl_test( sym->ccl, *start ) )
			{
				*end = start + 1;
				return TRUE;
			}
			break;

		case PPSYMTYPE_STRING:
			if( !strncmp( sym->str, start, strlen( sym->str ) ) )
			{
				*end = start + strlen( sym->str );
				return TRUE;
			}
			break;

		case PPSYMTYPE_REGEX:
			if( pregex_match( sym->re, start, end ) )
				return TRUE;

			break;

		case PPSYMTYPE_FUNCTION:
			if( (*sym->sf)( start, end ) )
				return TRUE;

			break;

		case PPSYMTYPE_SPECIAL:
			/* TODO: Other special symbols? */
			if( ! **end )
				return TRUE;

			break;

		default:
			MISSINGCASE;
	}

	return FALSE;
}

/* Skip over whitespace */
pboolean pp_white_in_input( ppgram* grm, char* start, char** end )
{
	plistel*	e;
	ppsym*		sym;

	if( !( grm && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	do
	{
		plist_for( grm->ws, e )
		{
			sym = (ppsym*)plist_access( e );

			if( pp_sym_in_input( sym, start, end ) )
				break;
		}

		start = *end;
	}
	while( e );

	return TRUE;
}

/* Count rows and columns. Returns bytes read. */
size_t pp_pos_in_input( int* row, int* col, char* start, char* end )
{
	char*	ptr	= start;

	if( !( row && col && start && end && start <= end ) )
	{
		WRONGPARAM;
		return 0;
	}

	while( ptr < end )
	{
		if( *ptr == '\n' )
		{
			ptr++;

			(*row)++;
			*col = 1;
		}
		else
		{
			ptr += u8_seqlen( ptr );
			(*col)++;
		}
	}

	return end - start;
}
