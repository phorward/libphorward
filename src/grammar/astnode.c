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

		if( node->atts )
			plist_for( node->atts, e )
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
			pg_value_to_string( pg_token_get_lexem(
				pg_astnode_get_token( node ) ) ) : "(X)" );
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

pboolean pg_astnode_set_att( pgastnode* node, char* key, pgvalue* value )
{
	plistel*	e;

	if( !( node ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !node->atts )
		node->atts = plist_create( 0, PLIST_MOD_PTR
										| PLIST_MOD_UNIQUE
											| PLIST_MOD_RECYCLE );

	if( key && *key && ( e = plist_get_by_key( node->atts, key ) ) )
	{
		pg_value_free( (pgvalue*)plist_access( e ) );
		plist_remove( node->atts, e );
	}

	if( !plist_insert( node->atts, (plistel*)NULL, key, value ) )
		return FALSE;

	return TRUE;
}

pgvalue* pg_astnode_get_att( pgastnode* node, int off )
{
	plistel*	e;

	if( !( node && off >= 0 ) )
	{
		WRONGPARAM;
		return (pgvalue*)NULL;
	}

	if( !node->atts )
		return (pgvalue*)NULL;

	if( ( e = plist_get( node->atts, off ) ) )
		return (pgvalue*)plist_access( e );

	return (pgvalue*)NULL;
}

pgvalue* pg_astnode_get_att_by_key( pgastnode* node, char* key )
{
	plistel*	e;

	if( !( node && key && *key ) )
	{
		WRONGPARAM;
		return (pgvalue*)NULL;
	}

	if( !node->atts )
		return (pgvalue*)NULL;

	if( ( e = plist_get_by_key( node->atts, key ) ) )
		return (pgvalue*)plist_access( e );

	return (pgvalue*)NULL;
}
