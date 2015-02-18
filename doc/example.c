#include <phorward.h>

int main()
{
    pparse* p;
    parray* a;
    char*   s = "1+2*(3+4)+5";
    char*   e;

    p = pp_create( 0,
			"%emitall"
			"f: /[0-9]+/ | '(' e ')';"
			"t: t '*' f | f ;"
			"e: e '+' t | t ;" );

    if( !pp_parse_to_ast( &a, p, s, &e ) )
        return 1; /* parse error */

    pp_ast_print( a );
    return 0;
}
