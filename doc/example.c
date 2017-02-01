#include <phorward.h>

int main()
{
    pparse* parser;
    ppast*  ast;
    char*   input = "1+2*(3+4)+5";
    char*   end;

    parser = pp_create( 0,  "@int /[0-9]+/ ;"
							"fact : int | '(' expr ')' ;"
                            "term : @mul( term '*' fact ) | fact ;"
                            "expr : @add( expr '+' term ) | term ;" );

    if( !pp_parse_to_ast( &ast, parser, input, &end ) )
        return 1; /* parse error */

    pp_ast_dump_short( stdout, ast );
    return 0;
}
