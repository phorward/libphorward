/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	main.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Main */

void help( char** argv )
{
	printf( "usage: %s OPTIONS grammar\n\n"

	"   -e  --exec   INPUT      Execute string INPUT on grammar.\n"
	"   -h  --help              Show this help, and exit.\n"
	"   -m  --mode   MODE       Use construction mode MODE:\n"
	"                           lr=bottom-up (default), ll=top-down\n"
	"   -r  --render RENDERER   Use AST renderer RENDERER:\n"
	"                           tree (default), tree2svg\n\n", *argv );
}

int main( int argc, char** argv )
{
	pboolean	lr		= TRUE;
	pboolean	as		= FALSE;
	parray*		a		= (parray*)NULL;
	ppgram*		g;
	char*		gstr	= (char*)NULL;
	char*		s		= (char*)NULL;
	char*		e;
	int			i;
	int			rc;
	int			next;
	char		opt		[ 20 + 1 ];
	char*		param;

	for( i = 0; ( rc = pgetopt( opt, &param, &next, argc, argv,
						"e:m:r:",
						"exec: mode: renderer:", i ) ) == 0; i++ )
	{
		if( !strcmp( opt, "exec" ) || *opt == 'e' )
			s = param;
		if( !strcmp( opt, "help" ) || *opt == 'h' )
		{
			help( argv );
			return 0;
		}
		else if( !strcmp( opt, "mode" ) || *opt == 'm' )
		{
			if( pstrcasecmp( param, "ll" ) == 0 )
				lr = FALSE;
		}
		else if( !strcmp( opt, "renderer" ) || *opt == 'r' )
			if( pstrcasecmp( param, "tree2svg" ) == 0 )
				as = TRUE;
	}

	if( rc == 1 )
		gstr = param;

	if( !gstr )
	{
		help( argv );
		return 1;
	}

	if( !( ( g = pp_gram_create() )
				&& pp_gram_from_bnf( g, gstr )
					&& pp_gram_prepare( g ) ) )
	{
		fprintf( stderr, "Parse error in >%s<\n", gstr );
		return 1;
	}

	pp_gram_print( g );

	if( s )
	{
		e = s;

		if( ( !lr && pp_ll_parse( &a, g, s, &e ) )
					|| pp_lr_parse( &a, g, s, &e ) )
		{
			printf( "\n%s SUCCEED >%.*s<\n", lr ? "LR" : "LL", e - s, s );

			if( as )
				pp_ast_tree2svg( a );
			else
				pp_ast_print( a );
		}
		else
			printf( "\n%s FAILED\n", lr ? "LR" : "LL" );

		parray_free( a );
	}

	return 0;
}

