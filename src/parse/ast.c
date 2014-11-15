/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ast.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

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
			printf( "%s{ %s (%d) >%.*s<\n",
				gap,
				match->sym->name,
				match->emit,
				match->end - match->start,
				match->start );

			strcat( gap, " " );
		}

		if( match->type & PPMATCH_END )
		{
			if( *gap )
				gap[ strlen( gap ) - 1 ] = '\0';

			printf( "%s} %s (%d) >%.*s<\n",
				gap,
				match->sym->name,
				match->emit,
				match->end - match->start,
				match->start );
		}
	}
}

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
