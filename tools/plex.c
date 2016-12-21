/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	plex.c
Usage:	A plex object demonstration suite.
----------------------------------------------------------------------------- */

#include "phorward.h"

int main( int argc, char** argv )
{
	char	buf		[ BUFSIZ + 1 ];
	plex*	lex;
	parray*	a;
	prange*	r;
	int		i;
	int		n;

	lex = plex_create( 0 );
	plex_define( lex, "[0-9]+", 1, 0 );
	plex_define( lex, "[a-zA-Z_][a-zA-Z_0-9]*", 2, 0 );
	plex_define( lex, "'(\\\\'|[^'])*'", 3, 0 );
	plex_define( lex, "\"(\\\\\"|[^\"])*\"", 4, 0 );

	while( fgets( buf, BUFSIZ, stdin ) )
	{
		n = plex_tokenize( lex, buf, &a );

		for( i = 0; n && ( r = (prange*)parray_get( a, i ) ); i++ )
			printf( "%2d: %2d @ >%.*s<\n",
				i, r->id, r->end - r->begin, r->begin );

		parray_free( a );
	}

	plex_free( lex );
	return 0;
}

