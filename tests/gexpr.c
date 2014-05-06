/* Random generator for mathematical expressions */

#include <stdlib.h>
#include <stdio.h>

int main()
{
	int		i;
	int		operands_cnt;
	int		operands		[ 20 ];
	char*	op				= "+-*/";
	int		br;

	srand( time( NULL ) );

	operands_cnt = rand() % 20;

	for( i = 0; i < operands_cnt; i++ )
		operands[ i ] = ( rand() % 999 ) + 1;

	operands[ i ] = 0;

	for( i = 0; operands[ i ]; i++ )
	{
		if( br > 0 && rand() % 6 == 0 )
		{
			printf( ") " );
			br--;
		}

		if( i > 0 )
			printf( "%c ", op[ rand() % 4 ] );

		if( rand() % 6 == 1 )
		{
			printf( "( " );
			br++;
		}

		printf( "%d ", operands[ i ] );
	}

	while( br > 0 )
	{
		printf( ") " );
		br--;
	}

	printf( "\n" );

	return 0;

}
