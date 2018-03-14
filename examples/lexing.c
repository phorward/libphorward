#include <phorward.h>

int main()
{
	char* 	tok[] = { "keyword", "literal", "identifier", "operator", "other" };
    plex*   l;
    parray* a;
    prange* r;

	/* Set up a lexer */
    l = plex_create( 0 );

	/* Define tokens */
    plex_define( l, "if|else|while|continue|break", 1, 0 );
    plex_define( l, "\\d+|\\d*\\.\\d+|\\d+\\.\\d*|true|false", 2, 0 );
    plex_define( l, "\\w+", 3, 0 );
    plex_define( l, "=|\\+|-|\\*|/|^|>|<|==|>=|<=|!=", 4, 0 );
    plex_define( l, ";|:|\\(|\\)|{|}|\\[\\]", 5, 0 );

	/* Prepare for execution */
    plex_prepare( l );

	/* Tokenize a string */
    plex_tokenize( l,
		"a = 12+39.5*7; while( true ) if( a > 0 ) break; else continue;", &a );

	/* Iterate through the result */
    parray_for( a, r )
        printf( "%-10s %.*s\n", tok[r->id - 1], r->end - r->start, r->start );
}
