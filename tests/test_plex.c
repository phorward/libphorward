#include "phorward.h"

void printitem( prange* r )
{
	printf( "%d >%.*s<\n", r->id, r->end - r->begin, r->begin );
}


int main()
{
	int		cnt;
	plex*	l;
	parray*	a;
	char*	s = "1 23 456 789   ";

	l = plex_create(0);
	plex_define( l, "\\d+", 1, 0 );
	plex_prepare( l );

	cnt = plex_tokenize( l, s, &a );
	printf("=%d\n", cnt );

	parray_iter( a, (parrayfn)printitem);
}
