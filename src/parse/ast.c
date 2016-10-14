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

/** Frees entire //ast// structure and subsequent links.

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

/** Dump detailed //ast// to //stream//. */
void pp_ast_dump( FILE* stream, ppast* ast )
{
	static int lev		= 0;
	int	i;

	while( ast )
	{
		for( i = 0; i < lev; i++ )
			fprintf( stream, " " );

		fprintf( stream, "{ %s >%.*s<\n", ast->emit, ast->length, ast->start );

		if( ast->child )
		{
			lev++;
			pp_ast_dump( stream, ast->child );
			lev--;
		}

		for( i = 0; i < lev; i++ )
			fprintf( stream, " " );

		fprintf( stream, "} %s >%.*s<\n", ast->emit, ast->length, ast->start );

		ast = ast->next;
	}
}

/** Dump simplified //ast// to //stream//.

Only opening matches are printed. */
void pp_ast_dump_short( FILE* stream, ppast* ast )
{
	static int lev		= 0;
	int	i;

	while( ast )
	{
		for( i = 0; i < lev; i++ )
			fprintf( stream, " " );

		fprintf( stream, "%s", ast->emit );

		if( ast->sym->type != PPSYMTYPE_NONTERM
			|| ast->sym->flags & PPFLAG_LEXEM )
			fprintf( stream, " >%.*s<\n", ast->length, ast->start );
		else
			fprintf( stream, "\n" );

		if( ast->child )
		{
			lev++;
			pp_ast_dump_short( stream, ast->child );
			lev--;
		}

		ast = ast->next;
	}
}

/** Dump //ast// to //stream// as JSON-formatted string.

Only opening matches are printed. */
void pp_ast_dump_json( FILE* stream, ppast* ast )
{
	char*	ptr;
	ppast*	node	= ast;

	if( ast && ast->next )
		fprintf( stream, "[" );

	while( node )
	{
		fputc( '{', stream );
		fprintf( stream, "\"emit\":" );

		/* Emit */
		fputc( '"', stream );

		for( ptr = node->emit; *ptr; ptr++ )
		{
			if( *ptr == '\"' )
				fputc( '\\', stream );

			fputc( *ptr, stream );
		}

		fputc( '"', stream );

		/* Match */
		if( node->sym->type != PPSYMTYPE_NONTERM
			|| node->sym->flags & PPFLAG_LEXEM )
		{
			fprintf( stream, ",\"match\":" );

			/* Matched string */
			fputc( '"', stream );

			for( ptr = node->start; *ptr && ptr < node->end; ptr++ )
				if( *ptr == '\"' )
					fprintf( stream, "\\\"" );
				else
					fputc( *ptr, stream );

			fputc( '"', stream );
		}

		/* Position */
		fprintf( stream, ",\"row\":%d,\"column\":%d", node->row, node->col );

		/* Children */
		if( node->child )
		{
			fprintf( stream, ",\"child\":" );
			pp_ast_dump_json( stream, node->child );
		}

		fputc( '}', stream );

		if( ( node = node->next ) )
			fputc( ',', stream );
	}

	if( ast && ast->next )
		fprintf( stream, "]" );
}

/** Dump //ast// in notation for the tree2svg tool that generates a
graphical view of the parse tree. */
void pp_ast_dump_tree2svg( FILE* stream, ppast* ast )
{
	while( ast )
	{
		if( ast->sym->type == PPSYMTYPE_NONTERM )
		{
			fprintf( stream, "'%s' [ ", ast->emit );
			pp_ast_dump_tree2svg( stream, ast->child );
			fprintf( stream, "] " );
		}
		else
			fprintf( stream, "'%.*s' ", ast->length, ast->start );

		ast = ast->next;
	}
}
