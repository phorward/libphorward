/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	input.c
Usage:	Implements the Phorward Input iaer (pia)
----------------------------------------------------------------------------- */

#include "phorward.h"


static int offsets_utf8[ 6 ] =
{
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static int trailbyte_utf8[ 256 ] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

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
	ia->ptr = ia->src.str = s;
	ia->eof = 0;

	return ia;
}

/** Creates a pia object reading from a wide-character string //ws//. */
pia* pia_create_from_wstr( wchar* ws )
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

	if( ia->flags & PIA_MOD_RELEASE )
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

/** Get a character from input adapter. */
unsigned int pia_read( pia* ia )
{
	unsigned int	ch;
	int				ubytes	= 0;
	int				ucount	= 0;

	if( !ia )
	{
		WRONGPARAM;
		return EOF;
	}

	if( ia->flags & PIA_MOD_IS_EOF )
		return ia->ch;

	ia->ch = 0;

	do
	{
		switch( ia->type )
		{
			case PIATYPE_FILE:
				if( ia->flags & PIA_MOD_WCHAR )
				{
					if( fread( &ch, sizeof( wchar ), 1, ia->src.file )
							!= sizeof( wchar ) )
						ch = ia->eof;
				}
				else
					ch = fgetc( ia->src.file );
				break;

			case PIATYPE_STR:
				if( ia->flags & PIA_MOD_WCHAR )
				{
					ch = (unsigned int) *( (wchar*)( ia->ptr ) );
					ia->ptr += sizeof( wchar );
				}
				else
					ch = (unsigned char)( *( ia->ptr++ ) );
				break;

			case PIATYPE_FUNC:
				ch = (*(ia->src.func))();
				break;

			default:
				MISSINGCASE;
				break;
		}

		if( ch == ia->eof )
		{
			ia->flags |= PIA_MOD_IS_EOF;
			ia->ch = ch;
			break;
		}

		if( ia->flags & PIA_MOD_UTF8 )
		{
			if( ucount == 0 )
				ubytes = ucount = trailbyte_utf8[ ch ];
			else
				ucount--;

			ia->ch += ch;

			if( ucount )
				ia->ch <<= 6;
		}
		else
			ia->ch = ch;
	}
	while( ucount );

	ia->ch -= offsets_utf8[ ubytes ];
	return ia->ch;
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
