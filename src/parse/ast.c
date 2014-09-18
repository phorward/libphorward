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

void pp_ast_print( plist* ast )
{
	plistel*	e;
	ppmatch*	match;
	char		gap		[ 80 + 1 ];

	if( !ast )
	{
		WRONGPARAM;
		return;
	}

	*gap = '\0';

	for( e = plist_first( ast ); e; e = plist_next( e ) )
	{
		match = (ppmatch*)plist_access( e );

		if( match->type == PPMATCH_END && *gap )
			gap[ strlen( gap ) - 1 ] = '\0';

		printf( "%s%s %s (%d) >%.*s<\n",
			gap,
			match->type == PPMATCH_BEGIN ? "{" : "}",
			match->sym->name,
			match->emit_id,
			match->end - match->start,
			match->start );

		if( match->type == PPMATCH_BEGIN )
			strcat( gap, " " );
	}
}
