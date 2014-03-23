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
	pgastnode*		node;
	int				offset;

	pgproduction*	left;		/* Marker for a left-recursive production */
} pgllse;

static pgtoken* lookahead( pgparser* parser )
{
	pgtoken*		la;

	if( !( la = pg_lexer_fetch( parser->lexer ) ) )
	{
		la = pg_token_create( pg_grammar_get_eoi(
				pg_parser_get_grammar( parser ) ), (pgvalue*)NULL );
	}

	return la;
}

pboolean pg_parser_ll_parse( pgparser* parser, pgast* ast )
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
	pgastnode*		prev;

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
		/* DEBUG: print stack */
		fprintf( stderr, "--\n" );

		for( i = 0; i < pstack_count( stack ); i++ )
		{
			se = (pgllse*)pstack_access( stack, i );

			if( se->symbol )
				fprintf( stderr, "%02d: %d %s%s\n",
					i,
					se->offset,
					pg_symbol_is_terminal( se->symbol ) ? "@" : "",
					pg_symbol_get_name( se->symbol ) );
			else
				fprintf( stderr, "%02d: %d (node) %p\n",
					i,
					se->offset,
					se->node );
		}

		/* If this is a AST node, chain it */
		if( tos->node )
		{
			for( i = 1, prev = (pgastnode*)NULL;
					se = (pgllse*)pstack_raccess( stack, i ); i++ )
			{
				if( se->node )
				{
					prev = se->node;
					break;
				}
			}

			if( prev )
			{
				if( prev->child )
				{
					for( prev = prev->child; prev->next; prev = prev->next )
						;

					prev->next = tos->node;
					tos->node->prev= prev->next;
				}
				else
					prev->child = tos->node;
			}
			else if( root )
			{
				for( prev = root; prev->next; prev = prev->next )
					;

				prev->next = tos->node;
				tos->node->prev = prev->next;
			}
			else
				root = tos->node;

			pstack_pop( stack );
		}
		/* Terminal */
		else if( pg_symbol_is_terminal( tos->symbol ) )
		{
			fprintf( stderr, "tos is terminal\n" );

			/* Fetch current lookahead */
			if( pg_token_get_symbol( la ) != tos->symbol )
			{
				fprintf( stderr, "Syntax error\n" );
				break;
			}

			/* If an AST node for the terminal shall be created,
				replace the current tos with an astnode. */
			if( ast && ( pg_ast_get_mode( ast ) == PGASTMODE_SYNTAX
						/* TEST TEST TEST */

						|| ( pg_ast_get_mode( ast ) == PGASTMODE_AST
							&& ( isupper( *pg_symbol_get_name( tos->symbol ) )
								|| *pg_symbol_get_name( tos->symbol ) == '@' )
							)
						) )
			{
				tos->offset = -1;
				tos->node = pg_astnode_create( (pgasttype*)NULL );
				tos->node->symbol = tos->symbol;
				tos->node->token = la;

				tos->symbol = (pgsymbol*)NULL;
			}
			else if( pg_ast_get_mode( ast ) == PGASTMODE_AST )
			{
				TODO;
			}
			else
				pstack_pop( stack );

			/* get next lookahead */
			la = lookahead( parser );
		}
		/* Nonterminal */
		else if( tos->symbol )
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

			/* Test for left-recursive grammars. */
			/*
			{
				plistel*	e;

				e = plist_get_by_key( trans, (char*)pg_token_get_symbol( la ) );

				while( ( p = (pgproduction*)plist_access( e ) ) )
				{
					fprintf( stderr, "handle >%s<\n",
						pg_production_to_string( p ) );

					e = plist_hashnext( e );
					if( plist_key( e ) != (char*)pg_token_get_symbol( la ) )
						break;

				}

				getchar();
			}
			*/

			/* if there is a AST node for the production, push this first */
			if( ast && pg_ast_get_mode( ast ) == PGASTMODE_AST &&
					pg_production_get_asttype( p ) )
			{
				tos->node = pg_astnode_create( pg_production_get_asttype( p ) );
				tos->symbol = (pgsymbol*)NULL;
				tos->offset = -1;
			}
			/* On syntax tree mode, always push a node */
			else if(  ast && pg_ast_get_mode( ast ) == PGASTMODE_SYNTAX )
			{
				tos->node = pg_astnode_create( (pgasttype*)NULL );
				tos->node->symbol = pg_production_get_lhs( p );
				tos->symbol = (pgsymbol*)NULL;
				tos->offset = -1;
			}
			else
				pstack_pop( stack );

			/* push symbols in reverse order */
			for( i = pg_production_get_rhs_length( p ) - 1;
					i >= 0 && ( sym = pg_production_get_rhs( p, i ) ); i-- )
			{
				tos = (pgllse*)pstack_push( stack, (void*)NULL );

				tos->symbol = sym;
				tos->node = (pgastnode*)NULL;
				tos->offset = i;
			}
		}
		/* Anything else (empty) */
		else
			pstack_pop( stack );
	}

	pstack_free( stack );

	if( !tos )
		fprintf( stderr, "Parse complete\n" );

	if( ast && root )
		pg_ast_set_root( ast, root );

	RETURN( tos ? FALSE : TRUE );
}
