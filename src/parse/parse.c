/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.c
Usage:	Parser maintainance object.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Creates a new parser object with flags //flags// and the grammar //bnf//. */
pparse* pp_create( int flags, char* bnf )
{
	pparse*	par;

	if( !bnf )
	{
		WRONGPARAM;
		return (pparse*)NULL;
	}

	par = (pparse*)pmalloc( sizeof( pparse ) );
	par->gram = pp_gram_create();

	/*
	if( ( par->type = type ) == PPTYPE_LL )
		par->gram->flags &= PPFLAG_PREVENTLREC;
	*/

	if( !pp_gram_from_bnf( par->gram, bnf ) )
		return pp_free( par ); /* TODO */

	/*
	par->dc = plist_create( sizeof( ppdofunc ), PLIST_MOD_UNIQUE );
	pp_do_setup_base( par->dc );
	*/

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

/* Adds a do command. */
/*
pboolean pp_parse_add_do( pparse* par, char* name, ppdofunc func )
{
	static char* invalid[10+1] = { "before", "behind", "emit", "goal",
										"ignore", "lrec", "rrec", "skip",
											"within" };
	int i;

	if( !( par && name && *name && func ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	for( i = 0; i < sizeof( invalid ) / sizeof( *invalid ); i++ )
		if( strcmp( invalid[ i ], name ) == 0 )
		{
			fprintf( stderr, "Do command '%s' is a reserved word\n", name );
			return FALSE;
		}

	if( !plist_insert( par->dc, (plistel*)NULL, name, (void*)&func ) )
	{
		fprintf( stderr, "Do command '%s' already defined\n", name );
		return FALSE;
	}

	return TRUE;
}
*/

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

	/*
	if( par->type == PPTYPE_LL )
		return pp_ll_parse( ast, par->gram, start, end );
	*/

	return pp_lr_parse( root, par->gram, start, end );
}

