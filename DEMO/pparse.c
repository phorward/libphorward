#include "phorward.h"

void ast_walker( parray* ast )
{
	int			i;
	ppmatch*	match;
	char		gap		[ 80 + 1 ];

	if( !ast )
	{
		WRONGPARAM;
		return;
	}

	*gap = '\0';

	for( i = 0; ( match = (ppmatch*)parray_get( ast, i ) ); i++ )
	{
		if( match->type & PPMATCH_BEGIN )
		{
			printf( "%s%s",
				gap,
				match->sym->name
				);

			if( match->sym->type != PPSYMTYPE_NONTERM )
				printf( " >%.*s<", match->end - match->start, match->start );

			printf( "\n" );
			strcat( gap, " " );
		}

		if( match->type & PPMATCH_END && *gap )
			gap[ strlen( gap ) - 1 ] = '\0';
	}
}

int main( int argc, char** argv )
{
	char*	e;
	char*	s;
	parray*	a;
	ppgram*	g;

	g = pp_gram_create( (char*)NULL );
	pp_gram4gram( g );
	pp_gram_prepare( g );
	pp_gram_print( g );

	if( argc > 1 )
	{
		e = s = argv[ 1 ];
		a = parray_create( sizeof( ppmatch ), 0 );

		if( pp_lr_parse( a, g, s, &e ) )
		{
			printf( "\nSUCCEED >%.*s<\n", e - s, s );
			ast_walker( a );
		}
		else
			printf( "\nFAILED\n" );

		parray_free( a );
	}

	return 0;
}
