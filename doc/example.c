#include <phorward.h>

int main()
{
    ppgram* g;
    char*   s = "1+2*(3+4)+5";
    char*   e;
    parray* a;

    g = pp_gram_create(
        "%emitall"
        "f: /[0-9]+/ | '(' e ')';"
        "t: t '*' f | f ;"
        "e: e '+' t | t ;" );

    if( !pp_lr_parse( &a, g, s, &e ) )
        return 1; /* parse error */

    pp_ast_simplify( a );
    return 0;
}
