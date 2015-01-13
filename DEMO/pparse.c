#include "phorward.h"


int main( int argc, char** argv )
{
	char*	e;
	char*	s;
	parray*	a;
	ppgram*	g;
	ppgram*	g2	= (ppgram*)NULL;
	int		i	= 0;
	char*	str;

	g = pp_gram_create( (char*)NULL );
	pp_gram2gram( g );
	pp_gram_prepare( g );
	pp_gram_print( g );

	if( argc > 1 )
	{
		str = argv[ 1 ];
		while( i < 5 )
		{
			e = s = str;

			if( pp_lr_parse( &a, g, s, &e ) )
			{
				printf( "\nSUCCEED >%.*s<\n", e - s, s );

				pp_ast_simplify( a );

				g2 = pp_ast2gram( a );
				printf( "\n--- final ---\n" );
				pp_gram_print( g2 );

				g = g2;

				i++;

				printf( "BOOTSTRAP %d OK\n", i );
				if( argc > 2 )
				{
					str = argv[ 2 ];
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
