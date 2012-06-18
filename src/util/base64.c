/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	base64.c
Author:	Bob Trower, contributions by Jan Max Meyer
		(see below for license)
Usage:	Functions for base64-string encoding and decoding
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */
static uchar* cb64	= "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					  "abcdefghijklmnopqrstuvwxyz"
					  "0123456789+/";

static uchar* cd64	= "|$$$}rstuvwxyz{$$$$$$$>?@"
					  "ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ"
					  "[\\]^_`abcdefghijklmnopq";

/*
 * Defines
 */
#define size_4byte( size ) 		( ( ( (long)( size ) / 3 + \
									( ( size ) % 3 ? 1 : 0 ) ) * 4 ) + 1 )
#define size_3byte( size ) 		( ( ( (long)( size ) / 4 ) * 3 ) + 1 )

/*
 * Functions
 */

static void encodeblock( uchar in[3], uchar out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (uchar) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) |
								((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (uchar) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

static void decodeblock( uchar in[4], uchar out[3] )
{
    out[ 0 ] = (uchar) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (uchar) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (uchar) (((in[2] << 6) & 0xc0) | in[3]);
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		to_base64()
	
	Author:			Jan Max Meyer
	
	Usage:			Encodes a data stream into a base64-encoded string.
					
	Parameters:		uchar**		outstream		Pointer to return the allocated
												pointer to the Base64-string,
												which is the result of the
												function.
					uchar*		instream		Pointer to the data stream to
												be encoded to Base64.
					psize		size			The size of the buffer instream.
	
	Returns:		pint						Returns ERR_OK on success,
												else an adequate error-code.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint to_base64( uchar** outstream, uchar* instream, psize size )
{
    uchar 	in		[3];
	uchar	out		[4];
	uchar*	ptr;
	uchar*	output;
	uchar*	inptr	= instream;
	uchar*	max		= instream + size;
	pint	i;
	pint	j;

	PROC( "to_base64" );
	PARMS( "outstream", "%p", outstream );
	PARMS( "instream", "%s", instream );
	PARMS( "size", "%ld", size );

	if( !instream )
		RETURN( ERR_PARMS );

	if( !( ptr = output = (uchar*)pmalloc( size_4byte( size )
								* sizeof( uchar ) ) ) )
	{
		RETURN( ERR_MEM );
	}

	while( inptr <= max )
	{
		for( i = 0, j = 0; i < 3; i++ )
		{
			in[i] = (uchar)*( inptr++ );
			if( inptr <= max )
				j++;
			else
				in[i] = 0;
		}

		if( j )
		{
            encodeblock( in, out, j );

			for( i = 0; i < 4; i++ )
			{
				*( ptr++ ) = out[i];
			}
		}
	}
	*ptr = '\0';

	*outstream = output;
	VARS( "*outstream", "%s", *outstream );
	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		from_base64()
	
	Author:			Jan Max Meyer
	
	Usage:			Decodes a base64-encoded stream and returns the binary
					data and its size.
					
	Parameters:		uchar**		outstream		Returns the pointer of the
												decoded data; This pointer is
												allocated and must be freed
												by the caller.
					psize*		outsize			A pointer to psize which
												receives the buffer size of
												outstream.
					uchar*		instream		The base64-encoded String to
												be decoded.
	
	Returns:		pint						Returns ERR_OK on success,
												else an adequate error-code.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pint from_base64( uchar** outstream, psize* outsize, uchar* instream )
{
    uchar	in		[4];
	uchar 	out		[3];
	uchar	v;

	uchar*	output;
	uchar*	ptr;
	uchar*	inptr	= instream;
	psize	size;
	psize	i;
	pint	j;

	PROC( "from_base64" );
	PARMS( "outstream", "%p", outstream );
	PARMS( "outsize", "%p", outsize );
	PARMS( "instream", "%s", instream );

	if( !instream )
		RETURN( ERR_PARMS );

	size = pstrlen( instream );
	if( !( ptr = output = (uchar*)pmalloc( size_3byte( size )
									* sizeof( uchar ) ) ) )
	{
		RETURN( ERR_MEM );
	}

	while( *inptr )
	{
		for( i = 0, j = 0; i < 4 && *inptr; i++ )
		{
			v = 0;
			while( *inptr && !v )
			{
				v = (uchar)( *(inptr++) );
				v = (uchar)( ( v < 43 || v > 122 ) ? 0 : cd64[ v - 43 ] );

				if( v )
					v = (uchar)( ( v == '$' ) ? 0 : v - 61 );
			}

			if( *inptr )
			{
				if( v )
					in[ i ] = (uchar)( v - 1 );

				j++;
			}
			else
				in[ i ] = 0;
		}

		if( j )
		{
            decodeblock( in, out );
			for( i = 0; i < j - 1; i++ )
				*(ptr++) = out[i];
		}
	}

	*outstream = output;
	*outsize = (psize)( ptr - output );

	VARS( "*outsize", "%ld", *outsize );
	RETURN( ERR_OK );
}

/*********************************************************************\

AUTHOR:         Bob Trower 08/04/01

PROJECT:        Crypt Data Packaging

COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001

NOTE:           This source code may be used as you wish, subject to
                the MIT license.  See the LICENCE section below.

DESCRIPTION:	This little utility implements the Base64
                Content-Transfer-Encoding standard described in
                RFC1113 (http://www.faqs.org/rfcs/rfc1113.html).

                This is the coding scheme used by MIME to allow
                binary data to be transferred by SMTP mail.

                Groups of 3 bytes from a binary stream are coded as
                groups of 4 bytes in a text stream.

                The input stream is 'padded' with zeros to create
                an input that is an even multiple of 3.

                A special character ('=') is used to denote padding so
                that the stream can be decoded back to its exact size.

                Encoded output is formatted in lines which should
                be a maximum of 72 characters to conform to the
                specification.  This program defaults to 72 characters,
                but will allow more or less through the use of a
                switch.  The program enforces a minimum line size
                of 4 characters.

                Example encoding:

                The stream 'ABCD' is 32 bits long.  It is mapped as
                follows:

                ABCD

                 A (65)     B (66)     C (67)     D (68)   (None) (None)
                01000001   01000010   01000011   01000100

                16 (Q)  20 (U)  9 (J)   3 (D)    17 (R) 0 (A)  NA (=) NA (=)
                010000  010100  001001  000011   010001 000000 000000 000000


                QUJDRA==

                Decoding is the process in reverse.  A 'decode' lookup
                table has been created to avoid string scans.

DESIGN GOALS:	Specifically:
		Code is a stand-alone utility to perform base64 
		encoding/decoding. It should be genuinely useful 
		when the need arises and it meets a need that is 
		likely to occur for some users.  
		Code acts as sample code to show the author's 
		design and coding style.  

		Generally: 
		This program is designed to survive:
		Everything you need is in a single source file.
		It compiles cleanly using a vanilla ANSI C compiler.
		It does its job correctly with a minimum of fuss.  
		The code is not overly clever, not overly simplistic 
		and not overly verbose. 
		Access is 'cut and paste' from a web page.  
		Terms of use are reasonable.  

DEPENDENCIES:   None

LICENCE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

                Permission is hereby granted, free of charge, to any person
                obtaining a copy of this software and associated
                documentation files (the "Software"), to deal in the
                Software without restriction, including without limitation
                the rights to use, copy, modify, merge, publish, distribute,
                sublicense, and/or sell copies of the Software, and to
                permit persons to whom the Software is furnished to do so,
                subject to the following conditions:

                The above copyright notice and this permission notice shall
                be included in all copies or substantial portions of the
                Software.

                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
                KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
                WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
                PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
                OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
                OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
                OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
                SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

VERSION HISTORY:
                Bob Trower 08/04/01 -- Create Version 0.00.00B

\******************************************************************* */

