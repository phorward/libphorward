/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ast.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

ppast* pp_ast_create( int emit, char* semit, ppsym* sym, ppprod* prod,
						char* start, char* end, int row, int col,
							ppast* prev, ppast* child )
{
	ppast*	node;

	node = (ppast*)pmalloc( sizeof( ppast ) );

	node->emit = emit;
	node->semit = semit;
	node->sym = sym;
	node->prod = prod;

	node->start = start;
	node->end = end;

	node->row = row;
	node->col = col;

	if( ( node->prev = prev ) )
	{
		if( prev->next )
			prev->next->prev = (ppast*)NULL;

		prev->next = node;
	}

	node->child = child;
	return node;
}


void pp_ast_printnew( ppast* ast )
{
	static int lev		= 0;
	int	i;

	while( ast )
	{
		for( i = 0; i < lev; i++ )
			fprintf( stderr, " " );

		fprintf( stderr, "%s %d >%.*s<\n",
					ast->semit, ast->emit,
						ast->end - ast->start,
							ast->start );

		if( ast->child )
		{
			lev++;
			pp_ast_printnew( ast->child );
			lev--;
		}

		ast = ast->next;
	}
}






/** Retrieves the entry with offset //offset// starting to count from
entry //from//. */
ppmatch* pp_ast_get( parray* ast, ppmatch* from, size_t offset )
{
	return (ppmatch*)parray_get( ast,
				( from ? parray_offset( ast, from ) : 0 ) + offset );
}

/** Queries for the //count//-th element that matches //emit// within the
boundaries of //start//. If //start// is an end-node of a tree item, the
function searches upwards, else downwards. If //start// is NULL, search
begins at the first element.

//depth// specifies an optional, maximum depth of levels to dive in, so a
//depth// of 1 will only match elements in the first level. A //depth// of 0
ignores the deepness.

The function returns the element found. */
ppmatch* pp_ast_query( parray* ast, ppmatch* start,
						int count, int emit, int depth )
{
	ppmatch*	walker;
	int			level	= 0;

	if( !( ast && count >= 0 ) )
	{
		WRONGPARAM;
		return (ppmatch*)NULL;
	}

	if( !start )
		start = pp_ast_get( ast, (ppmatch*)NULL, 0 );

	/* Entry is begin and end? Then fail. */
	if( start->type & PPMATCH_BEGIN && start->type & PPMATCH_END )
		return (ppmatch*)NULL;

	walker = start;

	/* Search down */
	if( start->type & PPMATCH_BEGIN )
	{
		while( ++walker <= (ppmatch*)parray_last( ast ) && level >= 0 )
		{
			if( walker->type & PPMATCH_BEGIN )
			{
				level++;

				if( ( depth <= 0 || level <= depth )
						&& ( emit <= 0 || walker->emit == emit )
							&& !count-- )
					return walker;
			}

			if( walker->type & PPMATCH_END )
				level--;
		}
	}
	/* Search up */
	else if( start->type & PPMATCH_END )
	{
		while( --walker >= (ppmatch*)parray_first( ast ) && level >= 0 )
		{
			if( walker->type & PPMATCH_END )
			{
				level++;

				if( ( depth <= 0 || level <= depth )
						&& ( emit <= 0 || walker->emit == emit )
							&& !count-- )
					return walker;
			}

			if( walker->type & PPMATCH_BEGIN )
				level--;
		}
	}

	/* If this happens, the AST is broken! */
	return (ppmatch*)NULL;
}

/** Returns the pendant entry of //match//.

If //match// is a match begin, it returns the corresponding end, and reverse.*/
ppmatch* pp_ast_pendant( parray* ast, ppmatch* match )
{
	ppmatch*	walker	= match;
	int			level	= 0;

	if( !( ast && match ) )
	{
		WRONGPARAM;
		return (ppmatch*)NULL;
	}

	/* Search down */
	if( match->type & PPMATCH_BEGIN )
	{
		do
		{
			if( walker->emit == match->emit )
			{
				if( walker->type & PPMATCH_BEGIN )
					level++;
				if( walker->type & PPMATCH_END )
					level--;

				if( !level )
					return walker;
			}

			walker++;
		}
		while( walker <= (ppmatch*)parray_last( ast ) );
	}
	/* Search up */
	else if( match->type & PPMATCH_END )
	{
		do
		{
			if( walker->emit == match->emit )
			{
				if( walker->type & PPMATCH_END )
					level++;
				if( walker->type & PPMATCH_BEGIN )
					level--;

				if( !level )
					return walker;
			}

			walker--;
		}
		while( walker >= (ppmatch*)parray_first( ast ) );
	}

	/* If this happens, the AST is broken! */
	return (ppmatch*)NULL;
}

/** Print detailed //ast// to stdout. */
void pp_ast_print( parray* ast )
{
	int			i;
	ppmatch*	match;
	char		gap		[ 80 + 1 ];

	if( !ast )
	{
		WRONGPARAM;
		return;
	}

	*gap = '\0';

	for( i = 0; ( match = (ppmatch*)parray_get( ast, i ) ); i++ )
	{
		if( match->type & PPMATCH_BEGIN )
		{
			printf( "%s{ %s (%d) >%.*s< @%03d:%03d\n",
				gap,
				match->sym->name,
				match->emit,
				match->end - match->start,
				match->start,
				match->row,
				match->col );

			strcat( gap, " " );
		}

		if( match->type & PPMATCH_END )
		{
			if( *gap )
				gap[ strlen( gap ) - 1 ] = '\0';

			printf( "%s} %s (%d) >%.*s< @%03d:%03d\n",
				gap,
				match->sym->name,
				match->emit,
				match->end - match->start,
				match->start,
				match->row,
				match->col );
		}
	}
}

/** Print simplified //ast// to stdout.
Only opening matches are printed. */
void pp_ast_simplify( parray* ast )
{
	int			i;
	ppmatch*	match;
	char		gap		[ 80 + 1 ];

	if( !ast )
	{
		WRONGPARAM;
		return;
	}

	*gap = '\0';

	for( i = 0; ( match = (ppmatch*)parray_get( ast, i ) ); i++ )
	{
		if( match->type & PPMATCH_BEGIN )
		{
			printf( "%s%s", gap, match->sym->name );

			if( match->sym->type != PPSYMTYPE_NONTERM )
				printf( " >%.*s<", match->end - match->start, match->start );

			printf( "\n" );
			strcat( gap, " " );
		}

		if( match->type & PPMATCH_END && *gap )
			gap[ strlen( gap ) - 1 ] = '\0';
	}
}

/** Print //ast// in notation for the tree2svg tool that generates a
graphical view of the parse tree. */
void pp_ast_tree2svg( parray* ast )
{
	int			i;
	ppmatch*	match;

	if( !ast )
	{
		WRONGPARAM;
		return;
	}

	for( i = 0; ( match = (ppmatch*)parray_get( ast, i ) ); i++ )
	{
		if( match->type & PPMATCH_BEGIN )
			printf( "'%s' [ ",
				match->sym->name );

		if( match->sym->type > PPSYMTYPE_NONTERM )
			printf( "'%.*s' ", match->end - match->start, match->start );

		if( match->type & PPMATCH_END )
			printf( "] " );
	}

	printf( "\n" );
}
