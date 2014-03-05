/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	asttype.c
Usage:	AST node type
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgasttype* pg_asttype_create( pggrammar* g, char* name )
{
	pgasttype	t;
	plistel*	e;

	if( !( g && name && *name ) )
	{
		WRONGPARAM;
		return (pgasttype*)NULL;
	}

	if( pg_asttype_get_by_name( g, name ) )
	{
		PGERR( g, __FILE__, __LINE__,
			"This grammar already contains a type named '%s'", name );

		return (pgasttype*)NULL;
	}

	memset( &t, 0, sizeof( pgasttype ) );
	psetstr( &t.name, name );

	if( !( e = plist_insert( g->asttypes, (plistel*)NULL, t.name, &t ) ) )
	{
		pfree( t.name );
		return (pgasttype*)NULL;
	}

	return (pgasttype*)plist_access( e );
}

/* Retrieve by name */
pgasttype* pg_asttype_get_by_name( pggrammar* g, char* name )
{
	if( !( g && name && *name ) )
	{
		WRONGPARAM;
		return (pgasttype*)NULL;
	}

	return (pgasttype*)plist_access( plist_get_by_key( g->asttypes, name ) );
}

/* Attribute: name */

/* GET ONLY! */
char* pg_asttype_get_name( pgasttype* asttype )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return (char*)NULL;
	}

	return pgetstr( asttype->name );
}

/* Attribute: size */

size_t pg_asttype_get_size( pgasttype* asttype )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return 0;
	}

	return asttype->size;
}

pboolean pg_asttype_set_size( pgasttype* asttype, size_t size )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	asttype->size = size;
	return TRUE;
}

/* Attribute: topdown */

pgastfn pg_asttype_get_topdown( pgasttype* asttype )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return asttype->topdown;
}

pboolean pg_asttype_set_topdown( pgasttype* asttype, pgastfn topdown )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	asttype->topdown = topdown;
	return TRUE;
}

pboolean pg_asttype_call_topdown( pgasttype* asttype, pgastnode* node )
{
	if( !( asttype && node ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( asttype->topdown )
	{
		(*asttype->topdown)( node );
		return TRUE;
	}

	return FALSE;
}

/* Attribute: passover */

pgastfn pg_asttype_get_passover( pgasttype* asttype )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return asttype->passover;
}

pboolean pg_asttype_set_passover( pgasttype* asttype, pgastfn passover )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	asttype->passover = passover;
	return TRUE;
}

pboolean pg_asttype_call_passover( pgasttype* asttype, pgastnode* node )
{
	if( !( asttype && node ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( asttype->passover )
	{
		(*asttype->passover)( node );
		return TRUE;
	}

	return FALSE;
}

/* Attribute: bottomup */

pgastfn pg_asttype_get_bottomup( pgasttype* asttype )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	return asttype->bottomup;
}

pboolean pg_asttype_set_bottomup( pgasttype* asttype, pgastfn bottomup )
{
	if( !( asttype ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	asttype->bottomup = bottomup;
	return TRUE;
}

pboolean pg_asttype_call_bottomup( pgasttype* asttype, pgastnode* node )
{
	if( !( asttype && node ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( asttype->bottomup )
	{
		(*asttype->bottomup)( node );
		return TRUE;
	}

	return FALSE;
}
