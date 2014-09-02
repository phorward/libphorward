/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	main.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Main */

int main( int argc, char** argv )
{
	ppgram*	g;
	char*	s;
	char*	e;

	if( argc > 1 )
	{
		g = pp_gram_create( argv[ 1 ] );

		pp_gram_print( g );

		if( argc > 2 )
		{
			s = e = argv[2];
			if( pp_lr_parse( (plist*)NULL, g, s, &e ) )
				printf( ">%.*s<\n", e - s, s );
			else
				printf( "FAIL\n" );
		}
	}

	return 0;
}
