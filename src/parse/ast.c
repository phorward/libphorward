/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ast.c
Usage:	Abstract syntax tree construction and traversal functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Creates new abstract syntax tree node. */
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
	node->len = end - start;

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

/** Returns the //n//th element of //node//. */
ppast* pp_ast_get( ppast* node, int n )
{
	if( !( node && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppast*)NULL;
	}

	while( node && n-- > 0 )
		node = node->next;

	return node;
}

/** Returns the //n//th element matching emit //emit// starting at //node//. */
ppast* pp_ast_select( ppast* node, char* emit, int n )
{
	if( !( node && n >= 0 ) )
	{
		WRONGPARAM;
		return (ppast*)NULL;
	}

	while( node )
	{
		if( ( !*pstrget( emit ) || strcmp( node->emit, emit ) == 0 )
				&& n-- == 0 )
			break;

		node = node->next;
	}

	return node;
}

/** Evaluate //ast// using evaluation function //func//.

The evaluation function has the prototype

``` void (*pastevalfn)( ppasteval type, ppast* node )

and retrieves a //type// regarding the position where the evaluation currently
is positioned, and the node pointer.
*/
void pp_ast_eval( ppast* ast, pastevalfn func )
{
	ppast* 	node;

	func( PPAST_EVAL_TOPDOWN, ast );

	for( node = ast->child; node; node = node->next )
	{
		pp_ast_eval( node, func );
		func( PPAST_EVAL_PASSOVER, node );
	}

	func( PPAST_EVAL_BOTTOMUP, ast );
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

		fprintf( stream, "{ %s >%.*s<\n",
			ast->emit, (int)ast->len, ast->start );

		if( ast->child )
		{
			lev++;
			pp_ast_dump( stream, ast->child );
			lev--;
		}

		for( i = 0; i < lev; i++ )
			fprintf( stream, " " );

		fprintf( stream, "} %s >%.*s<\n",
			ast->emit, (int)ast->len, ast->start );

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

		if( PPSYM_IS_TERMINAL( ast->sym ) || ast->sym->flags & PPFLAG_LEXEM )
			fprintf( stream, " (%.*s)\n", (int)ast->len, ast->start );
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

/** Dump AST to trace.

The PP_AST_DUMP-macro is used to dump all relevant contents of a ppast object
into the program trace, for debugging purposes.

PP_AST_DUMP() can only be used when the function was trace-enabled by PROC()
before.
*/
/*MACRO:PP_AST_DUMP( ppast* ast )*/
void _dbg_ast_dump( char* file, int line, char* function,
						char* name, ppast* ast )
{
	if( !_dbg_trace_enabled( file, function ) )
		return;

	_dbg_trace( file, line, "AST", function, "%s = {", name );
	pp_ast_dump_short( stdout, ast );
	_dbg_trace( file, line, "AST", function, "}" );
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
		if( PPSYM_IS_TERMINAL( ast->sym ) || node->sym->flags & PPFLAG_LEXEM )
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
		fprintf( stream, ",\"row\":%ld,\"column\":%ld", node->row, node->col );

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
		if( !PPSYM_IS_TERMINAL( ast->sym ) )
		{
			fprintf( stream, "'%s' [ ", ast->emit );
			pp_ast_dump_tree2svg( stream, ast->child );
			fprintf( stream, "] " );
		}
		else
			fprintf( stream, "'%.*s' ", (int)ast->len, ast->start );

		ast = ast->next;
	}
}

static void eval_emit( pvmprog* prog, char* emit, ppast* ast )
{
	static char				buf		[ BUFSIZ + 1 ];
	char*					ptr;
	char*					code;

	PROC( "eval_emit" );
	PARMS( "prog", "%p", prog );
	PARMS( "emit", "%s", emit );

	ptr = buf;
	if( ast->len < BUFSIZ )
		sprintf( buf, "%.*s", (int)ast->len, ast->start );
	else
		ptr = pstrndup( ast->start, ast->len );

	if( strstr( emit, "$0" ) )
		code = pstrreplace( emit, "$0", ptr );
	else
		code = emit;

	VARS( "code", "%s", code );

	pvm_prog_compile( prog, code );

	if( code != ast->emit )
		pfree( code );

	if( ptr != buf )
		pfree( ptr );

	VOIDRET;
}

/** Dump //ast// into pvm program */
void pp_ast_dump_pvm( pvmprog* prog, ppast* ast )
{
	PROC( "pp_ast_dump_pvm" );
	PARMS( "prog", "%p", prog );
	PARMS( "ast", "%p", ast );

	if( !( prog && ast ) )
	{
		WRONGPARAM;
		VOIDRET;
	}

	while( ast )
	{
		/*
		if( ast->emit && pstrncasecmp( ast->emit, "<<" ) )
			eval_emit( prog, ast->emit + 2 );
		*/

		if( ast->child )
		{
			pp_ast_dump_pvm( prog, ast->child );

			/*
			if( ast->emit && pstrncasecmp( ast->emit, "!!" ) )
				eval_emit( prog, ast->emit + 2 );
			*/
		}

		if( ast->emit /* && (pstrncasecmp( ast->emit, ">>" ) */ )
		{
			eval_emit( prog, ast->emit, ast );
		}

		ast = ast->next;
	}

	VOIDRET;
}
