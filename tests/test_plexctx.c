#include <phorward.h>

int main()
{
    char*   tok[] = { "keyword", "literal", "identifier", "operator", "other" };
    int     ch;     /* Current character */
    plex*   l;      /* Lexer */
    plexctx c;      /* Context */
    int     i;      /* Position */
    int     w;      /* Current width of token */
    int     s;      /* Current start of token */

    /* Set up a lexer & context */
    l = plex_create( 0 );
	plexctx_init( &c, l );

    /* Define tokens */
    plex_define( l, "if|else|while|continue|break", 1, 0 );
    plex_define( l, "\\d+|\\d*\\.\\d+|\\d+\\.\\d*|true|false", 2, 0 );
    plex_define( l, "\\w+", 3, 0 );
    plex_define( l, "=|\\+|-|\\*|/|^|>|<|==|>=|<=|!=", 4, 0 );
    plex_define( l, ";|:|\\(|\\)|{|}|\\[\\]", 5, 0 );

    /* Prepare for execution */
    plex_prepare( l );

    /* Tokenize a string */
    for( i = w = s = 0; ( ch = getc( stdin ) ) != EOF; i++, w++ )
    {
        if( !plexctx_lex( &c, ch ) )
        {
            if( c.handle )
            {
                ungetc( ch, stdin );
                printf( "%-10s %d,%d\n", tok[ c.handle - 1 ], s, w );
            }

			plexctx_reset( &c );
            w = -1;
            s = i;
        }

        if( ch == '\n' )
            break;
    }

    printf( "DONE!\n" );
}
