#include <phorward.h>

int main(int argc, char** argv)
{
	pccl*		a;
	pccl*		b;
	pccl*		c;

	if( argc < 2 )
		return 1;

	a = pccl_create( PCCL_MIN, PCCL_MAX, argv[1] );
	pccl_print( stderr, a, -1 );

	b = pccl_create( -1, -1, "a-z" );
	c = pccl_union( a, b );

	pccl_print( stderr, c, -1 );

	pccl_delrange( c, 'j', 'q');
	pccl_print( stderr, c, -1 );

	pccl_free( a );
	pccl_free( b );
	pccl_free( c );
}
