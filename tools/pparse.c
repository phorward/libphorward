/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pparse.c
Usage:	A pparse/ppast object demonstration suite.
----------------------------------------------------------------------------- */

#include "local.h"

/* Main */

void help( char** argv )
{
	printf( "Usage: %s OPTIONS grammar [input [input ...]]\n\n"

	"   grammar                   Grammar to create a parser from.\n"
	"   input                     Input to be processed by the parser.\n\n"

	"   -G                        Dump constructed grammar\n"
	"   -h  --help                Show this help, and exit.\n"
	"   -r  --render  RENDERER    Use AST renderer RENDERER:\n"
	"                             short (default), full, json, tree2svg\n"
	"   -v  --verbose             Print processing information.\n"
	"   -V  --version             Show version info and exit.\n"

	"\n", *argv );

}

int main( int argc, char** argv )
{
	pboolean	verbose	= FALSE;
	pboolean	lm		= FALSE;
	pboolean	dg		= FALSE;
	int			r		= 0;
	ppast*		a		= (ppast*)NULL;
	ppgram*		g;
	pppar*		p;
	char*		gfile	= "grammar";
	char*		gstr	= (char*)NULL;
	char*		ifile;
	char*		istr	= (char*)NULL;
	char*		s		= (char*)NULL;
	char*		e;
	int			i;
	int			rc;
	int			next;
	char		opt		[ 20 + 1 ];
	char		line	[ 1024 + 1 ];
	char*		param;

	for( i = 0; ( rc = pgetopt( opt, &param, &next, argc, argv,
						"Ghr:vV",
						"renderer: help verbose version", i ) )
							== 0; i++ )
	{
		if( !strcmp(opt, "G" ) )
			dg = TRUE;
		else if( !strcmp( opt, "help" ) || !strcmp( opt, "h" ) )
		{
			help( argv );
			return 0;
		}
		else if( !strcmp( opt, "renderer" ) || !strcmp( opt, "r" ) )
		{
			if( pstrcasecmp( param, "full" ) == 0 )
				r = 1;
			else if( pstrcasecmp( param, "json" ) == 0 )
				r = 2;
			else if( pstrcasecmp( param, "tree2svg" ) == 0 )
				r = 3;
		}
		else if( !strcmp( opt, "verbose" ) || !strcmp( opt, "v" ) )
			verbose = TRUE;
		else if( !strcmp( opt, "version" ) || !strcmp( opt, "V" ) )
		{
			version( argv, "Parser construction command-line utility" );
			return 0;
		}
	}

	if( rc == 1 && param )
	{
		if( pfiletostr( &gstr, param ) )
			gfile = param;
		else
			gstr = param;

		next++;
	}

	if( !gstr )
	{
		help( argv );
		return 1;
	}

	if( verbose )
		printf( "Parsing grammar from '%s'\n", gfile );

	if( !( ( g = pp_gram_create() ) && pp_gram_from_bnf( g, gstr ) ) )
	{
		fprintf( stderr, "%s: Parse error in >%s<\n", gfile, gstr );
		return 1;
	}

	if( !pp_gram_prepare( g ) )
	{
		fprintf( stderr, "%s: Unable to prepare grammar\n", gfile );
		return 1;
	}

	if( dg )
		pp_gram_dump( stdout, g );

	p = pp_par_create( g );
	pp_par_auto_token( p );

	lm = argc == next;
	i = 0;

	while( TRUE )
	{
		ifile = (char*)NULL;

		if( lm )
		{
			fgets( line, sizeof( line ) - 1, stdin );
			s = pstrtrim( line );

			if( !*s )
				break;

		}
		else
		{
			if( next == argc )
				break;

			if( pfiletostr( &istr, argv[ next ] ) )
			{
				ifile = argv[ next ];
				s = istr;
			}
			else
				s = argv[ next ];

			next++;
		}

		if( !ifile )
		{
			sprintf( opt, "input.%d", i );
			ifile = opt;
		}

		if( s )
		{
			e = s;

			if( pp_par_parse( &a, p, s, &e ) )
			{
				switch( r )
				{
					case 0:
						pp_ast_dump_short( stdout, a );
						break;

					case 1:
						pp_ast_dump( stdout, a );
						break;

					case 2:
						pp_ast_dump_json( stdout, a );
						printf( "\n" );
						break;

					case 3:
						pp_ast_dump_tree2svg( stdout, a );
						printf( "\n" );
						break;

					default:
						MISSINGCASE;
						break;
				}
			}
			else
				fprintf( stderr, "%s: Parse error in >%s<\n", ifile, s );

			a = pp_ast_free( a );
		}

		istr = pfree( istr );
		i++;
	}

	return 0;
}

