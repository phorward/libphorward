#include <phorward.h>

static int  stack[100];                             /* Stack for calculations */
static int* tos = &stack[0];                        /* Top-of-stack pointer */

void calc( ppasteval type, ppast* node )            /* AST evaluation */
{
    if( type != PPAST_EVAL_BOTTOMUP )
        return;

    if( !strcmp( node->emit, "Int" ) )
        *(tos++) = atoi( node->start );
    else if( !strcmp( node->emit, "add" ) )
        *((tos--) - 2) = *(tos - 2) + *(tos - 1);
    else if( !strcmp( node->emit, "mul" ) )
        *((tos--) - 2) = *(tos - 2) * *(tos - 1);
}

int main()
{
    ppgram* grm;
    pppar*  par;
    ppast*  ast;

    grm = pp_gram_create();                         /* Create grammar */
    pp_gram_from_pbnf( grm,                         /* Describe grammar */
         "Int  := /[0-9]+/ ;"
         "fact : Int | '(' expr ')' ;"
         "term : term '*' fact = mul | fact ;"
         "expr : expr '+' term = add | term ;" );

    par = pp_par_create( grm );                     /* Construct parser on it */

    if( !pp_par_parse( &ast, par, "1+2*(3+4)+8" ) ) /* Parse an input string, */
        return 1;                                   /* exit on parse error */

    pp_ast_eval( ast, calc );                       /* Evaluate parsed AST */
    printf( "%d\n", stack[0] );                     /* Dump stacked result */

    return 0;
}
