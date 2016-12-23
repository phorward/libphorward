/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pparse.c
Usage:	A pparse/ppast object demonstration suite.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Main */

void help( char** argv )
{
	printf( "usage: %s OPTIONS grammar\n\n"

	"   -e  --exec    INPUT     Execute string INPUT on grammar.\n"
	"   -g  --grammar BNF       Define grammar from BNF.\n"
	"   -G                      Dump constructed grammar\n"
	"   -h  --help              Show this help, and exit.\n"
/*	"   -m  --mode    MODE      Use construction mode MODE:\n"
	"                           lr=bottom-up (default), ll=top-down\n" */
	"   -r  --render  RENDERER  Use AST renderer RENDERER:\n"
	"                           dump (default), short, json, tree2svg\n"
	"   -s  --source  FILENAME  Execute input from FILENAME on grammar.\n"
	"\n", *argv );

}

int main( int argc, char** argv )
{
	pboolean	lr		= TRUE;
	pboolean	dg		= FALSE;
	int			r		= 0;
	ppast*		a		= (ppast*)NULL;
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
						"e:g:Gm:r:s:",
						"exec: grammar: mode: renderer: source:", i ) )
							== 0; i++ )
	{
		if( !strcmp( opt, "exec" ) || *opt == 'e' )
			s = param;
		else if( !strcmp( opt, "grammar" ) || *opt == 'g' )
			gstr = param;
		else if( *opt == 'G' )
			dg = TRUE;
		else if( !strcmp( opt, "help" ) || *opt == 'h' )
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
		{
			if( pstrcasecmp( param, "short" ) == 0 )
				r = 1;
			else if( pstrcasecmp( param, "json" ) == 0 )
				r = 2;
			else if( pstrcasecmp( param, "tree2svg" ) == 0 )
				r = 3;
		}
		else if( !strcmp( opt, "source" ) || *opt == 's' )
		{
			if( !pfiletostr( &s, param ) )
			{
				fprintf( stderr, "Unable to read source file '%s'\n", param );
				return 1;
			}
		}
	}

	if( rc == 1 && param )
	{
		if( !pfiletostr( &gstr, param ) )
		{
			fprintf( stderr, "Unable to read grammar file '%s'\n", param );
			return 1;
		}
	}

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

	if( dg )
		pp_gram_dump( stdout, g );

	if( s )
	{
		e = s;

		if( pp_lr_parse( &a, g, s, &e ) )
		{
			if( !a )
				printf( "%s SUCCEED >%.*s<\n", lr ? "LR" : "LL", e - s, s );
			else
			{
				switch( r )
				{
					case 0:
						pp_ast_dump( stdout, a );
						break;

					case 1:
						pp_ast_dump_short( stdout, a );
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
		}
		else
			printf( "%s FAILED\n", lr ? "LR" : "LL" );

		pp_ast_free( a );
	}

	return 0;
}

/* This is an old test program stub */
#if 0
int main1( int argc, char** argv )
{
	char*		e;
	char*		s;
	parray*		a;
	ppgram*		g;
	ppgram*		g2		= (ppgram*)NULL;
	int			i		= 0;
	char*		str;

	g = pp_gram_create( (char*)NULL );
	pp_gram2gram( g );
	pp_gram_prepare( g );
	pp_gram_print( g );

	if( argc > 1 )
	{
		str = argv[ 1 ];
		while( TRUE )
		{
			e = s = str;

			if( pp_lr_parse( &a, g, s, &e ) )
			{
				printf( "\nSUCCEED >%.*s<\n", e - s, s );

				pp_ast_simplify( a );

				if( parray_count( a ) )
				{
					g2 = pp_ast2gram( a );
					printf( "\n--- final ---\n" );
					pp_gram_print( g2 );

					g = g2;
				}

				i++;

				printf( "BOOTSTRAP %d OK\n", i );
				if( argc > i + 1 )
				{
					if( *argv[ i + 1 ] )
						str = argv[ i + 1 ];plex_lex

					printf( "EXEC >%s<\n", str );
				}

				getchar();
			}
			else
			{
				printf( "\nFAILED\n" );
				break;
			}

			parray_free( a );
		}

		printf( "i = %d\n", i );
	}

	return 0;
}
#endif
