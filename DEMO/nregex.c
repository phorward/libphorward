#include "phorward.h"

int main( int argc, char** argv )
{
	char*	pat		= argv[1];
	char*	start	= argv[2];
	char*	end;
	pregex*	re;

	if( argc < 2 )
		return 1;

	re = pregex_create( pat, PREGEX_MOD_NO_REF );

	if( argc < 3 )
		return 1;

	if( pregex_match( re, start, &end ) )
	{
		printf( "YES, WE CAN >%.*s<\n", end - start, start );
		return 0;
	}

	printf( "Unfortunatelly not!\n" );

	return 1;
}
