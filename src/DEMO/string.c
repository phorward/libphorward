/*
	The pstring-object is a newly established library and should serve an
	object-oriented function set to work with Unicode-enabled, dynamic strings.
	It was established during the developments on the RapidBATCH 6 scripting
	language, and will be enhanced as soon as new requests are available.
	The string object's memory is managed by the pstring-functions, so the
	objects user doesn't need to take care of it.
*/

#include <pbasis.h>
#include <pstring.h>

int main( int argc, char** argv )
{
	pstring	s1, s2;
	uchar*	cp;
	int		i;

	setlocale( LC_ALL, "" );

	s1 = pstring_new( L"Hello World" );
	s2 = pstring_new( L"This is quite an example of the new pstring-object" );
	printf( "wide-character:\n  %ls %ls\n",
				pstring_get( s1 ), pstring_get( s2 ) );

	cp = pstring_to_uchar( s1 );
	printf( "UTF-8:\n  %s \n", cp );
	pfree( cp );

	s1 = pstring_append( s1, s2, FALSE );
	printf( "%ls\n", pstring_get( s1 ) );

	pstring_set_int( s1, 12345 );
	s2 = pstring_append( s2, s1, FALSE );

	printf( ">%ls< %d >%ls< %d\n", pstring_get( s1 ), (int)pstring_len( s1 ),
									pstring_get( s2 ), (int)pstring_len( s2 ) );

	pstring_set_double( s1, 1234567.890123456789 );

	printf( ">%ls< %d\n", pstring_get( s1 ), (int)pstring_len( s1 ) );

	pstring_set( s1, L"Hello" );
	pstring_set( s2, L"HEllO" );

	printf( "equal? %s case-insensitive %s\n",
				pstring_compare( s1, s2, FALSE ) == 0 ? "yes" : "no",
						pstring_compare( s1, s2, TRUE ) == 0 ? "yes" : "no" );

	pstring_copy( s2, s1 );

	printf( "equal? %s case-insensitive %s\n",
				pstring_compare( s1, s2, FALSE ) == 0 ? "yes" : "no",
						pstring_compare( s1, s2, TRUE ) == 0 ? "yes" : "no" );

	for( i = 0; i < 10; i++ )
		s1 = pstring_APPEND( s1, pstring_get( s2 ), 0 );

	printf( "%ls\n", pstring_get( s1 ) );

	printf( "empty? %s\n", pstring_empty( s1 ) ? "yes" : "no" ); 
	pstring_reset( s1 );
	printf( "empty? %s\n", pstring_empty( s1 ) ? "yes" : "no" );

	pstring_free( s1 );
	pstring_free( s2 );

	return EXIT_SUCCESS;
}

