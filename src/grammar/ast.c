/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ast.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pgast* pg_ast_create( pggrammar* g, pgastmode mode )
{
	pgast*		ast;

	if( !( g ) )
	{
		WRONGPARAM;
		return (pgast*)NULL;
	}

	ast = (pgast*)pmalloc( sizeof( pgast ) );
	ast->grammar = g;
	ast->mode = mode;

	return ast;
}

pgast* pg_ast_free( pgast* ast )
{
	if( !ast )
		return (pgast*)NULL;

	pg_astnode_free( ast->root );

	return (pgast*)NULL;
}


/* Print the AST */
static void print_ast( int cnt, pgastnode* node )
{
	int			i;
	pgastnode*	child;

	while( node )
	{
		for( i = 0; i < cnt; i++ )
			fprintf( stderr, " " );

		if( node->type )
			fprintf( stderr, "%s\n",
				pg_asttype_get_name( node->type ) );
		else if( node->token )
			fprintf( stderr, "%s = >%s<\n",
				pg_symbol_get_name( node->symbol ),
					pg_value_get_string( pg_token_get_lexem( node->token ) ) );
		else
			fprintf( stderr, "%s\n",
				pg_symbol_get_name( node->symbol ) );

		print_ast( cnt + 1, node->child );

		node = node->next;
	}
}

void pg_ast_print( pgast* ast )
{
	if( !( ast ) )
	{
		WRONGPARAM;
		return;
	}

	print_ast( 0, ast->root );
}

/* Attribute: mode */

/* GET ONLY! */
pgastmode pg_ast_get_mode( pgast* ast )
{
	if( !ast )
	{
		WRONGPARAM;
		return PGASTMODE_NONE;
	}

	return ast->mode;
}

/* Attribute: root */

pgastnode* pg_ast_get_root( pgast* ast )
{
	if( !ast )
	{
		WRONGPARAM;
		return (pgastnode*)NULL;
	}

	return ast->root;
}

pboolean pg_ast_set_root( pgast* ast, pgastnode* root )
{
	if( !ast )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( ast->root )
		pg_astnode_free( ast->root );

	ast->root = root;
	return TRUE;
}
