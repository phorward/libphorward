/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.c
Usage:	Parser maintainance object.
----------------------------------------------------------------------------- */

#include "phorward.h"

#if 0

/** Creates a new parser object with flags //flags// and the grammar //bnf//. */
pparse* pp_create( int flags, char* bnf )
{
	int			i;
	int			t;
	pparse*		par;
	ppsym*		sym;
	pregex_ptn*	ptn;

	if( !bnf )
	{
		WRONGPARAM;
		return (pparse*)NULL;
	}

	par = (pparse*)pmalloc( sizeof( pparse ) );

	par->gram = pp_gram_create();

	if( !pp_gram_from_bnf( par->gram, bnf ) )
		return pp_free( par ); /* TODO */

	pp_gram_prepare( par->gram );

	return par;
}

/** Free parser //par//. */
pparse* pp_free( pparse* par )
{
	if( !par )
		return (pparse*)NULL;

	pp_gram_free( par->gram );
	pfree( par );

	return (pparse*)NULL;
}

/** Run parser //par// with input //start//.

The function uses the parsing method defined when then parser was created.
//end// receives a pointer to the position where the parser stopped.

It returns a parse-tree to //root// on success. */
pboolean pp_parse_to_ast( ppast** root, pparse* par, char* start, char** end )
{
	if( !( par && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* fixme
	return pp_lr_parse( root, par->gram, start, end );
	*/

	return FALSE;
}
#endif
