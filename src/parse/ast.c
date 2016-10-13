/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ast.c
Usage:	Abstract syntax tree construction and traversal functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Creates new abstract syntax tree node. */
ppast* pp_ast_create( char* emit, ppsym* sym, ppprod* prod,
						char* start, char* end, int row, int col,
							ppast* child )
{
	ppast*	node;

	node = (ppast*)pmalloc( sizeof( ppast ) );

	node->emit = emit;
	node->sym = sym;
	node->prod = prod;

	node->start = start;
	node->end = end;
	node->length = end - start;

	node->row = row;
	node->col = col;

	node->child = child;
	return node;
}

/* Frees entire //ast// structure and subsequent links.

Always returns (ppast*)NULL. */
ppast* pp_ast_free( ppast* node )
{
	ppast*	child;
	ppast*	next;

	if( !node )
		return (ppast*)NULL;

	next = node->child;

	while( next )
	{
		child = next;
		next = child->next;

		pp_ast_free( child );
	}

	return (ppast*)pfree( node );
}

/** Returns length of //node// chain. */
int pp_ast_len( ppast* node )
{
	int		step	= 0;

	while( node )
	{
		node = node->next;
		step++;
	}

	return step;
}

/** Print detailed //ast// to stdout. */
void pp_ast_print( ppast* ast )
{
	static int lev		= 0;
	int	i;

	while( ast )
	{
		for( i = 0; i < lev; i++ )
			fprintf( stderr, " " );

		fprintf( stderr, "{ %s >%.*s<\n", ast->emit, ast->length, ast->start );

		if( ast->child )
		{
			lev++;
			pp_ast_print( ast->child );
			lev--;
		}

		for( i = 0; i < lev; i++ )
			fprintf( stderr, " " );

		fprintf( stderr, "} %s >%.*s<\n", ast->emit, ast->length, ast->start );

		ast = ast->next;
	}
}

/** Print simplified //ast// to stdout.
Only opening matches are printed. */
void pp_ast_simplify( ppast* ast )
{
	static int lev		= 0;
	int	i;

	while( ast )
	{
		for( i = 0; i < lev; i++ )
			fprintf( stderr, " " );

		fprintf( stderr, "%s >%.*s<\n", ast->emit, ast->length, ast->start );

		if( ast->child )
		{
			lev++;
			pp_ast_simplify( ast->child );
			lev--;
		}

		ast = ast->next;
	}
}

/** Print //ast// in notation for the tree2svg tool that generates a
graphical view of the parse tree. */
void pp_ast_tree2svg( ppast* ast )
{
	static int lev		= 0;

	while( ast )
	{
		if( ast->sym->type == PPSYMTYPE_NONTERM )
		{
			printf( "'%s' [ ", ast->emit );

			lev++;
			pp_ast_tree2svg( ast->child );
			lev--;

			printf( "] " );
		}
		else
			printf( "'%.*s' ", ast->length, ast->start );


		ast = ast->next;
	}

	if( lev == 0 )
		printf( "\n" );
}
