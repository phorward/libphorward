#include "phorward.h"

int main( int argc, char** argv )
{
	char		line	[ 128 + 1 ];
	char		buf		[ 1024 + 1 ];
	pvm*		vm;
	pvmprog*	p;

	vm = pvm_create();

	while( TRUE )
	{
		*buf = '\0';

		while( TRUE )
		{
			printf( ">" );
			fflush( stdout );

			fgets( line, 128, stdin );
			if( *line == '\n' )
				break;

			strcat( buf, line );
			strcat( buf, "\n" );
		}

		if( !*buf )
			break;

		p = pvm_prog_create( vm, buf );
		pvm_prog_dump( p );

		printf( "Prepare to run (Y/n)?" );
		fflush( stdout);
		if( toupper( getchar() ) == 'N' )
			continue;

		pvm_prog_run( p );
		pvm_prog_free( p );
	}

	return 0;
}
