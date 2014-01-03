/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	input.c
Usage:	Implements the Phorward Input iaer (pia)
----------------------------------------------------------------------------- */

#include "phorward.h"

/* UTF-8 skirmish... */

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

/** Initialize a pia object. */
pia* pia_init( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	memset( in, 0, sizeof( pia ) );

	in->flags = PIA_MOD_UTF8;
	pia_set_stream( in, stdin );

	return in;
}

/** Creates a standard pia object configured to read from stdin. */
pia* pia_create( void )
{
	pia* 	in;

	in = (pia*)pmalloc( sizeof( pia ) );
	pia_init( in );

	return in;
}

/** Creates a pia object reading from a file specified by //path//. */
pia* pia_create_from_file( char* path )
{
	pia*	in;
	FILE*	f;

	if( !( path && *path ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	if( !( f = fopen( path, "rb" ) ) )
		return (pia*)NULL;

	in = pia_create();
	pia_set_stream( in, f );
	in->flags |= PIA_MOD_RELEASE;

	return in;
}

/** Creates a pia object reading from open file stream //f//. */
pia* pia_create_from_stream( FILE* f )
{
	pia*	in;

	if( !( f ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	in = pia_create();
	pia_set_stream( in, f );

	return in;
}

/** Creates a pia object reading from a string //s//. */
pia* pia_create_from_str( char* str )
{
	pia*	in;

	if( !( str ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	in = pia_create();
	pia_set_str( in, str );

	return in;
}

/** Creates a pia object reading from a wide-character string //ws//. */
pia* pia_create_from_wstr( wchar* wstr )
{
	pia*	in;

	if( !( wstr ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	in = pia_create();
	pia_set_wstr( in, wstr );

	return in;
}

/* Destruction */

/** Close a pia object. The objects data source is closed or freed
(if required and possible), the object itself remains as default object
reading from stdin. */
pia* pia_close( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return (pia*)NULL;
	}

	if( in->flags & PIA_MOD_RELEASE )
	{
		switch( in->type )
		{
			case PIATYPE_STREAM:
				if( pia_get_stream( in ) != stdin )
					fclose( pia_get_stream( in ) );
				break;

			case PIATYPE_STR:
				pfree( in->src.str );
				break;

			default:
				break;
		}
	}

	in->type = PIATYPE_STREAM;
	in->src.file = stdin;

	in->flags &= ~PIA_MOD_IS_EOF;

	return in;
}

/** Frees a pia object and closes its source (if required). */
pia* pia_free( pia* in )
{
	if( !in )
		return (pia*)NULL;

	pia_close( in );

	pfree( in->bufbeg );
	pfree( in );

	return (pia*)NULL;
}

/* Runtime */

/** Clears or resets the current input buffer. */
pboolean pia_bufreset( pia* in )
{
	if( !in )
	{
		WRONGPARAM;
		return FALSE;
	}

	in->bufend = in->bufbeg;

	return TRUE;
}

/** Read a character from input and buffer it. */
unsigned int pia_getchar( pia* in )
{
	unsigned int	ch;
	int				ubytes	= 0;
	int				ucount	= 0;

	if( !in )
	{
		WRONGPARAM;
		return 0;
	}

	if( in->flags & PIA_MOD_IS_EOF )
		return in->ch;

	in->ch = 0;

	do
	{
		switch( in->type )
		{
			case PIATYPE_STREAM:
				if( in->flags & PIA_MOD_WCHAR )
				{
					if( fread( &ch, sizeof( wchar ), 1, in->src.file )
							!= sizeof( wchar ) )
						ch = in->eof;
				}
				else
					ch = fgetc( in->src.file );
				break;

			case PIATYPE_STR:
				if( in->flags & PIA_MOD_WCHAR )
				{
					ch = (unsigned int) *( (wchar*)( in->ptr ) );
					in->ptr += sizeof( wchar );
				}
				else
					ch = (unsigned char)( *( in->ptr++ ) );
				break;

			case PIATYPE_FUNC:
				ch = (*(in->src.func))();
				break;

			default:
				MISSINGCASE;
				break;
		}

		if( ch == in->eof )
		{
			in->flags |= PIA_MOD_IS_EOF;
			in->ch = ch;
			return ch;
		}

		if( !( in->flags & PIA_MOD_WCHAR ) && in->flags & PIA_MOD_UTF8 )
		{
			if( ucount == 0 )
				ubytes = ucount = trailbyte_utf8[ ch ];
			else
				ucount--;

			in->ch += ch;

			if( ucount )
				in->ch <<= 6;
		}
		else
			in->ch = ch;
	}
	while( ucount );

	in->ch -= offsets_utf8[ ubytes ];

	/* Buffer reallocation required? */
	if( ( in->bufbeg - in->bufend ) == in->bufsiz  )
	{
		in->bufbeg = prealloc( in->bufbeg,
						( in->bufsiz + BUFSIZ ) * sizeof( unsigned int ) );

		in->bufend = in->bufbeg + in->bufsiz;
		in->bufsiz += PIA_BUFSIZ;
	}

	*( in->bufend++ ) = in->ch;

	return in->ch;
}

/** Returns TRUE if the input stream read the end-of-file. */
pboolean pia_is_eof( pia* in )
{
	if( !in )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( ( in->flags & PIA_MOD_IS_EOF ) )
		return TRUE;

	return FALSE;
}

/* File */

/** Set file //f// as input source for //in//. */
pboolean pia_set_stream( pia* in, FILE* f )
{
	if( !( in && f ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	pia_close( in );
	in->src.file = f;
	in->eof = EOF;

	return TRUE;
}

/** Returns the stream source FILE-pointer of //in//, if given.

If the adapter is not configured as a stream source, (FILE*)NULL
will be returned. */
FILE* pia_get_stream( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return (FILE*)NULL;
	}

	if( in->type == PIATYPE_STREAM )
		return in->src.file;

	return (FILE*)NULL;
}

/* String */

/** Set byte-character string //str// as input source for //in//. */
pboolean pia_set_str( pia* in, char* str )
{
	if( !( in && str ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	pia_close( in );

	in->type = PIATYPE_STR;

	in->flags &= ~PIA_MOD_WCHAR;
	in->flags |= PIA_MOD_UTF8;

	in->ptr = in->src.str = str;
	in->eof = 0;

	return TRUE;
}

/** Returns the byte-character source of //in//, if given.

If the adapter is not configured as byte-character source, (char*)NULL
will be returned. */
char* pia_get_str( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return (char*)NULL;
	}

	if( in->type == PIATYPE_STR && !( in->flags & PIA_MOD_WCHAR ) )
		return in->src.str;

	return (char*)NULL;
}

/* Wide-character string */

/** Set wide-character string //wstr// as input source for //in//. */
pboolean pia_set_wstr( pia* in, wchar* wstr )
{
	if( !( in && wstr ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	pia_close( in );

	in->type = PIATYPE_STR;

	in->flags &= ~PIA_MOD_UTF8;
	in->flags |= PIA_MOD_WCHAR;

	in->ptr = in->src.str = (char*)wstr;
	in->eof = 0;

	return TRUE;
}

/** Returns the wide-character source of //in//, if given.

If the adapter is not configured for wide-character source, (wchar*)NULL
will be returned. */
wchar* pia_get_wstr( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return (wchar*)NULL;
	}

	if( in->type == PIATYPE_STR && ( in->flags & PIA_MOD_WCHAR ) )
		return (wchar*)in->src.str;

	return (wchar*)NULL;
}


/* Flags */

/** Sets //flags// for //in//. */
pboolean pia_set_flags( pia* in, int flags )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Remove UTF-8 flag if wide-character mode is provided */
	if( flags & PIA_MOD_UTF8 && flags & PIA_MOD_WCHAR )
		flags &= ~PIA_MOD_UTF8;

	in->flags = flags;
	return TRUE;
}

/** Returns flag configuration of //in//. */
int pia_get_flags( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return 0;
	}

	return in->flags;
}

/* EOF */

/** Configures an end-of-file charater for //in//. */
pboolean pia_set_eof( pia* in, unsigned int eof )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	in->eof = eof;
	return TRUE;
}

/** Returns the configured end-of-file charater of //in//. */
unsigned int pia_get_eof( pia* in )
{
	if( !( in ) )
	{
		WRONGPARAM;
		return 0;
	}

	return in->eof;
}
