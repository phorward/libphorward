#include "phorward.h"

char*	str	= 	"hello 1234 /aa\\/this/ is 634 /abc/ my7world1337a";

void printanddrop( parray* a )
{
	int				i;
	pregex_range*	r;

	for( i = 0; ( r = (pregex_range*)parray_get( a, i ) ); i++ )
		printf( "%2d: %2d @ >%.*s<\n", i, r->id, r->end - r->begin, r->begin );

	parray_free( a );
}

void test_regex()
{
	pregex*	re;
	parray*	a;
	char*	r;

	re = pregex_create( "/(\\\\.|[^\\\\/])*/", PREGEX_COMP_NONGREEDY );

	printf( "str = >%s<\n", str );
	printf( "matches = %d\n", pregex_findall( re, str, &a ) );
	printanddrop( a );
	printf( "splits = %d\n", pregex_splitall( re, str, &a ) );
	printanddrop( a );

	r = pregex_replace( re, str, "int" );
	printf( "r = >%s<\n", r );
	free( r );
}

void test_lex()
{
	plex*	l;
	parray*	a;

	l = plex_create( 0 );
	plex_define( l, "[0-9]+", 1, 0 );
	plex_define( l, "[a-z]+", 2, 0 );

	printf( "str = >%s<\n", str );
	printf( "matches = %d\n", plex_findall( l, str, &a ) );
	printanddrop( a );
}

int main( int argc, char** argv )
{
	/*
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
	*/

	test_regex();
	test_lex();

	return 0;
}
