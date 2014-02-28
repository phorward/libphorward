/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ll.parse.c
Usage:	LL parser
----------------------------------------------------------------------------- */

#include "phorward.h"
#include "ll.h"

/* Stack Element */
typedef struct
{
	pgsymbol*		symbol;
	pgastnode*		parent;
} pgllse;

static pgtoken* lookahead( pgparser* parser )
{
	pgtoken*		la;

	if( !( la = pg_lexer_fetch( parser->lexer ) ) )
	{
		la = pg_token_create( pg_grammar_get_eoi(
				pg_parser_get_grammar( parser ) ), (char*)NULL );
	}

	return la;
}

pboolean pg_parser_ll_parse( pgparser* parser )
{
	int				i;
	pgsymbol*		sym;
	pstack*			stack;
	pgllse*			tos;
	pgllse*			se;
	pgtoken*		la;
	plist*			trans;
	pgproduction*	p;

	pgastnode*		root		= (pgastnode*)NULL;

	PROC( "pg_parser_ll_parse" );
	PARMS( "parser", "%p", parser );

	if( !( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !pg_parser_is_ll( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	stack = pstack_create( sizeof( pgllse ), 64 );
	tos = (pgllse*)pstack_push( stack, (void*)NULL );

	memset( tos, 0, sizeof( pgllse ) );
	tos->symbol = pg_grammar_get_goal( pg_parser_get_grammar( parser ) );

	/* get initial lookahead */
	la = lookahead( parser );

	while( ( tos = (pgllse*)pstack_top( stack ) ) )
	{
		/* print stack */
		fprintf( stderr, "--\n" );

		for( i = 0; i < pstack_count( stack ); i++ )
		{
			se = (pgllse*)pstack_access( stack, i );

			fprintf( stderr, "%02d: %s%s\n", i,
				pg_symbol_is_terminal( se->symbol ) ?
					"@" : "",
				pg_symbol_get_name( se->symbol ) );
		}

		if( pg_symbol_is_terminal( tos->symbol ) )
		{
			fprintf( stderr, "tos is terminal\n" );

			/* Fetch current lookahead */
			if( pg_token_get_symbol( la ) != tos->symbol )
			{
				fprintf( stderr, "Syntax error\n" );
				break;
			}

			pstack_pop( stack );

			/* get next lookahead */
			la = lookahead( parser );
		}
		else
		{
			fprintf( stderr, "tos is nonterminal\n" );

			/* get production to push from parse table */
			if( !( ( trans = (plist*)plist_access(
									plist_get_by_key( parser->states,
										(char*)tos->symbol ) ) )
					&& ( p = (pgproduction*)plist_access(
									plist_get_by_key( trans,
										(char*)pg_token_get_symbol( la ) ) ) )
				) )
			{
				fprintf( stderr, "Syntax error %p %p\n", trans, p );
				break;
			}

			pstack_pop( stack );

			/* push symbols in reverse order */
			for( i = pg_production_get_rhs_length( p ) - 1;
					i >= 0 && ( sym = pg_production_get_rhs( p, i ) ); i-- )
			{
				tos = (pgllse*)pstack_push( stack, (void*)NULL );
				tos->symbol = sym;
			}
		}
	}

	pstack_free( stack );

	if( !tos )
		fprintf( stderr, "Parse complete\n" );

	RETURN( tos ? FALSE : TRUE );
}
