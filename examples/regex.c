#include <phorward.h>

int main( int argc, char** argv )
{
    pregex* re;
    char*   s;
    char*   ptr;

    if( argc < 2 || !pfiletostr( &s, argv[ 1 ] ) )      /* Load file into str */
        return 1;

    ptr = s;
    re = pregex_create(
            "(href|src)=\"((https://|http://|//).*)\"", /* Regular expression */
                PREGEX_COMP_NONGREEDY );                /* Handling options */

    while( pregex_find( re, ptr, &ptr ) )               /* Dump matches */
        printf( "%.*s\n", re->ref[2].end - re->ref[2].start, re->ref[2].start );

    return 0;
}
