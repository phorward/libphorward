#include <phorward.h>

int main( int argc, char** argv )
{
    int			i;
    int			rc;
    int			next;
    char		opt			[ 10 + 1 ];
    char*		param;

    for( i = 0; ( rc = pgetopt( opt, &param, &next, argc, argv,
                                "ho:", "help output:", i ) ) == 0; i++ )
    {
        if( !strcmp( opt, "help" ) || !strcmp( opt, "h" ) )
            printf( "Help\n" );
        else if( !strcmp( opt, "output" ) || !strcmp( opt, "o" ) )
            printf( "Output = >%s<\n", param );
    }

    if( rc < 0 && param )
    {
        fprintf( stderr, "Unknown option '%s'\n", param );
        return 1;
    }

    for( i = 0; next < argc; next++, i++ )
        printf( "Parameter %d = >%s<\n", i, argv[ next ] );

    return 0;
}
