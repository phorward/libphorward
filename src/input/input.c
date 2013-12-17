/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	input.c
Usage:	Implements the Phorward Input iaer (pia)
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Construction */

/** Creates a standard pia object configured to read from stdin. */
pia* pia_create( void )
{
	pia* 	ia;

	ia = (pia*)pmalloc( sizeof( pia ) );
	ia->type = PIATYPE_FILE;
	ia->src.file = stdin;
	ia->eof = EOF;
	ia->flags = PIA_MOD_UTF8;

	return ia;
}

/** Creates a pia object reading from file //f//. */
pia* pia_create_from_file( FILE* f )
{
	pia*	ia;

	if( !( f ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	ia = pia_create();
	ia->src.file = f;

	return ia;
}

/** Creates a pia object reading from a string //s//. */
pia* pia_create_from_str( char* s )
{
	pia*	ia;

	if( !( s ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	ia = pia_create();

	ia->type = PIATYPE_STR;
	ia->src.str = s;
	ia->eof = 0;

	return ia;
}

/** Creates a pia object reading from a wide-character string //ws//. */
pia* pia_create_from_wstr( wchar_t* ws )
{
	pia*	ia;

	if( !( ws ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	ia = pia_create_from_str( (char*)ws );
	ia->flags = PIA_MOD_WCHAR;

	return ia;
}

/** Free pia object. */
pia* pia_free( pia* ia )
{
	if( !( ia ) )
		return (pia*)NULL;

	if( ia->flags & PIA_RELEASE )
	{
		switch( ia->type )
		{
			case PIATYPE_FILE:
				fclose( ia->src.file );
				break;

			case PIATYPE_STR:
				pfree( ia->src.str );
				break;

			default:
				break;
		}
	}

	pfree( ia );

	return (pia*)NULL;
}

/* flags */

/** Sets flags for //ia//. */
pboolean pia_set_flags( pia* ia, int flags )
{
	if( !( ia ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Remove UTF-8 flag if wide-character mode is provided */
	if( flags & PIA_MOD_UTF8 && flags & PIA_MOD_WCHAR )
		flags &= ~PIA_MOD_UTF8;

	ia->flags = flags;
	return TRUE;
}

/** Returns flag configuration of //ia//. */
int pia_get_flags( pia* ia )
{
	if( !( ia ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return ia->flags;
}
