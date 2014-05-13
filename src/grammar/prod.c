/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	prod.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Constructor */

pgprod* pg_prod_create( pggrammar* g, pgnonterm* lhs, ... )
{
	pgprod*	p;
	pgsymbol*		sym;
	va_list			args;

	if( !( g ) )
	{
		WRONGPARAM;
		return (pgprod*)NULL;
	}

	p = (pgprod*)plist_malloc( g->productions );

	p->rhs = plist_create( 0, PLIST_MOD_PTR |
								PLIST_MOD_UNIQUE |
									PLIST_MOD_RECYCLE );

	p->select = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	/* Connect with grammar */
	p->grammar = g;

	/* Connect with left-hand side */
	if( ( p->lhs = lhs ) )
		plist_push( lhs->productions, p );

	/* Fill in right-hand side symbols */
	va_start( args, lhs );

	while( ( sym = va_arg( args, pgsymbol* ) ) )
		pg_prod_append( p, sym );

	va_end( args );

	return p;
}

/* Destructor */

pgprod* pg_prod_drop( pgprod* p )
{
	if( !p )
		return (pgprod*)NULL;

	pfree( p->strval );
	p->select = plist_free( p->select );

	return (pgprod*)NULL;
}

/* Debug */

char* pg_prod_to_string( pgprod* p )
{
	plistel*	e;
	pgsymbol*	sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	p->strval = (char*)pfree( p->strval );

	if( p->lhs )
		p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );

	p->strval = pstrcatstr( p->strval, " : ", FALSE );

	plist_for( p->rhs, e )
	{
		sym = (pgsymbol*)plist_access( e );

		if( e != plist_first( p->rhs ) )
			p->strval = pstrcatstr( p->strval, " ", FALSE );

		if( pg_symbol_is_term( sym ) )
			p->strval = pstrcatstr( p->strval, "@", FALSE );

		p->strval = pstrcatstr( p->strval, sym->name, FALSE );
	}

	return p->strval;
}

void pg_prod_print( pgprod* p, FILE* f )
{
	int			i;
	pgsymbol*	sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return;
	}

	if( !f )
		f = stderr;

	if( p->lhs )
		pg_symbol_print( p->lhs, f );

	fprintf( f, " : " );

	for( i = 0; ( sym = pg_prod_get_rhs( p, i ) ); i++ )
	{
		if( i > 0 )
			fprintf( f, " " );

		pg_symbol_print( sym, f );
	}

	fprintf( f, "\n" );
}

/* Append right-hand side */

pboolean pg_prod_append( pgprod* p, pgsymbol* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !plist_push( p->rhs, sym ) )
		return FALSE;

	return TRUE;
}

/* Append right-hand side with key */

pboolean pg_prod_append_with_key(
	pgprod* p, char* key, pgsymbol* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !( key && *key ) )
		return pg_prod_append( p, sym );

	if( !plist_insert( p->rhs, (plistel*)NULL, key, sym ) )
		return FALSE;

	return TRUE;
}

/* Retrieve: By offset from grammar */

pgprod* pg_prod_get( pggrammar* grammar, int i )
{
	if( !( grammar && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgprod*)NULL;
	}

	return (pgprod*)plist_access( plist_get( grammar->productions, i ) );
}

/* Retrieve: By offset from keft-hand side */

pgprod* pg_prod_get_by_lhs( pgnonterm* lhs, int i )
{
	if( !( pg_symbol_is_nonterm( lhs ) && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgprod*)NULL;
	}

	return (pgprod*)plist_access( plist_get( lhs->productions, i ) );
}

/* Retrieve: Right-hand side item */

pgsymbol* pg_prod_get_rhs( pgprod* p, int i )
{
	if( !( p && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	return (pgsymbol*)plist_access( plist_get( p->rhs, i ) );
}

/* Retrieve: Right-hand side item by key */

pgsymbol* pg_prod_get_rhs_by_key( pgprod* p, char* key )
{
	if( !( p && key && *key ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	return (pgsymbol*)plist_get_by_key( p->rhs, key );
}

/* Attribute: id */

/* GET ONLY! */
int pg_prod_get_id( pgprod* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return -1;
	}

	return plist_offset( plist_get_by_ptr( p->grammar->productions, p ) );
}

/* Attribute: grammar */

/* GET ONLY! */
pggrammar* pg_prod_get_grammar( pgprod* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (pggrammar*)NULL;
	}

	return p->grammar;
}

/* Attribute: lhs */

/* GET ONLY! */
pgnonterm* pg_prod_get_lhs( pgprod* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (pgnonterm*)NULL;
	}

	return p->lhs;
}

/* Attribute: rhs_length */

/* GET ONLY! */
int pg_prod_get_rhs_length( pgprod* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return 0;
	}

	return plist_count( p->rhs );
}
