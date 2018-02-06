#include "phorward.h"

int main( int argc, char** argv )
{
	parray* 	m;
	prange*		r;

	pregex_qmatch("@\\w*", "@tralala", PREGEX_RUN_DEBUG, &m );

	for( r = parray_first( m ); r; r = parray_next( m, r ) )
		printf( "%d >%.*s<\n", r->id, r->end - r->begin, r->begin );

	parray_free( m );

	return 0;
}
