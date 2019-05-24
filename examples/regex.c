#include <phorward.h>

int main( int argc, char** argv )
{
    pregex* re;
    char*	s;
    char*	e;
    char*	p;

    p = s = "{{{}} und {{{}}} bis {{}";
    re = pregex_create(
            "{(?R)?}", 						/* Regular expression */
                0 );                /* Handling options */

    while( ( p = pregex_find( re, p, &e ) ) )               /* Dump matches */
    {
        printf( "%d >%.*s<\n", p - s, e - p, p );
        p = e;
    }


    return 0;
}
