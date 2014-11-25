#include "phorward.h"


int main( int argc, char** argv )
{
	char*	e;
	char*	s;
	parray*	a;
	ppgram*	g;
	ppgram*	g2;

	g = pp_gram_create( (char*)NULL );
	pp_gram2gram( g );
	pp_gram_prepare( g );
	pp_gram_print( g );

	if( argc > 1 )
	{
		e = s = argv[ 1 ];

		if( pp_lr_parse( &a, g, s, &e ) )
		{
			printf( "\nSUCCEED >%.*s<\n", e - s, s );

			pp_ast_simplify( a );

			g2 = pp_ast2gram( a );
			pp_gram_print( g2 );
		}
		else
			printf( "\nFAILED\n" );

		parray_free( a );
	}

	return 0;
}
