/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	astnode.c
Usage:	AST node
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgastnode* pg_astnode_create( pgasttype* type )
{
	pgastnode*	n;

	if( type )
	{
		n = (pgastnode*)pmalloc( sizeof( pgastnode )
									+ pg_asttype_get_size( type ) );
		n->type = type;
	}
	else
		n = (pgastnode*)pmalloc( sizeof( pgastnode ) );

	return n;
}

/* Destructor */

pgastnode* pg_astnode_free( pgastnode* node )
{
	pgastnode*	next;
	plistel*	e;

	while( node )
	{
		next = node->next;

		if( node->child )
			pg_astnode_free( node->child );

		if( node->attributes )
			plist_for( node->attributes, e )
				pfree( plist_access( e ) );

		pfree( node );
		node = next;
	}

	return (pgastnode*)NULL;
}

/* Print */

void pg_astnode_print( pgastnode* node, FILE* stream )
{
	if( !node )
	{
		WRONGPARAM;
		return;
	}

	if( !stream )
		stream = stderr;

	fprintf( stream, "type:%s sym:%s tok:>%s<",
		node && pg_astnode_get_type( node ) ?
			pg_asttype_get_name( pg_astnode_get_type( node ) ) : "(X)",
		node && pg_astnode_get_symbol( node ) ?
			pg_symbol_get_name( pg_astnode_get_symbol( node ) ) : "(X)",
		node && pg_astnode_get_token( node ) ?
			pg_token_get_lexem( pg_astnode_get_token( node ) ) : "(X)" );
}

/* Attribute: type */

/* GET ONLY */
pgasttype* pg_astnode_get_type( pgastnode* node )
{
	if( !node )
	{
		WRONGPARAM;
		return (pgasttype*)NULL;
	}

	return node->type;
}

/* Attribute: symbol */

pgsymbol* pg_astnode_get_symbol( pgastnode* node )
{
	if( !node )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	return node->symbol;
}

pboolean pg_astnode_set_symbol( pgastnode* node, pgsymbol* symbol )
{
	if( !node )
	{
		WRONGPARAM;
		return FALSE;
	}

	node->symbol = symbol;
	return TRUE;
}

/* Attribute: token */

pgtoken* pg_astnode_get_token( pgastnode* node )
{
	if( !node )
	{
		WRONGPARAM;
		return (pgtoken*)NULL;
	}

	return node->token;
}

pboolean pg_astnode_set_token( pgastnode* node, pgtoken* token )
{
	if( !node )
	{
		WRONGPARAM;
		return FALSE;
	}

	node->token = token;
	return TRUE;
}

/* Attribute: Attributes */
#if 0
pboolean pg_astnode_set_attribute( pgastnode* node, char* key, char* value )
{
	plistel*	e;

	if( !( node ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !node->attributes )
		node->attributes = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_UNIQUE );

	if( key && *key && ( e = plist_get_by_key( node->attributes, key ) ) )
	{
		if( !value )
			plist_remove( node->attributes, e );
		else
		{
			pfree( plist_access( e ) );
			*( (char**)plist_access( e ) ) = pstrdup( value );
		}
	}
	else
		plist_insert( node->attributes, (plistel*)NULL,
						key, pstrdup( value ) );

	return TRUE;
}

char* pg_astnode_get_attribute( pgastnode* node, int off )
{
	plistel*	e;

	if( !( node && off >= 0 ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !node->attributes )
		return (char*)NULL;

	if( ( e = plist_get( node->attributes, off ) ) )
		return (char*)plist_access( e );

	return (char*)NULL;
}

char* pg_astnode_get_attribute_by_key( pgastnode* node, char* key )
{
	plistel*	e;

	if( !( node && key && *key ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !node->attributes )
		return (char*)NULL;

	if( ( e = plist_get_by_key( node->attributes, key ) ) )
		return *( (char**)plist_access( e ) );

	return (char*)NULL;
}
#endif
