/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See \LICENSE for more information.

File:	production.c
Usage:
----------------------------------------------------------------------------- */

#include "local.h"

/* Constructor */

pgproduction* pg_production_create( pgnonterminal* lhs, ... )
{
	pgproduction*	p;
	pgsymbol*		sym;
	va_list			args;

	if( !( lhs && pg_symbol_get_type( lhs ) == PGSYMTYPE_NONTERMINAL ) )
	{
		WRONGPARAM;
		return (pgproduction*)NULL;
	}

	p = (pgproduction*)pmalloc( sizeof( pgproduction ) );

	/* Connect with grammar */
	p->grammar = pg_symbol_get_grammar( lhs );
	p->grammar->productions = list_push( p->grammar->productions, p );

	/* Connect with left-hand side */
	p->lhs = lhs;
	lhs->productions = list_push( lhs->productions, p );

	/* Fill in right-hand side symbols */
	va_start( args, lhs );

	while( ( sym = va_arg( args, pgsymbol* ) ) )
		pg_production_append( p, sym );

	va_end( args );

	return p;
}

/* Destructor */

pgproduction* pg_production_drop( pgproduction* p )
{
	if( !p )
		return (pgproduction*)NULL;

	pfree( p->strval );


	return (pgproduction*)NULL;
}

/* Debug */

uchar* pg_production_to_string( pgproduction* p )
{
	LIST*		l;
	pgsymbol*	sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	p->strval = (uchar*)pfree( p->strval );

	p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );
	p->strval = pstrcatstr( p->strval, " : ", FALSE );

	LISTFOR( p->rhs, l )
	{
		sym = (pgsymbol*)list_access( l );

		if( l != p->rhs )
			p->strval = pstrcatstr( p->strval, " ", FALSE );

		if( pg_symbol_is_terminal( sym ) )
			p->strval = pstrcatstr( p->strval, "@", FALSE );

		p->strval = pstrcatstr( p->strval, sym->name, FALSE );
	}

	return p->strval;
}

/* Append right-hand side */

pboolean pg_production_append( pgproduction* p, pgsymbol* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	p->rhs = list_push( p->rhs, sym );

	return TRUE;
}

/* Retrieve: By offset from grammar */

pgproduction* pg_production_get( pggrammar* grammar, int i )
{
	if( !( grammar && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgproduction*)NULL;
	}

	return (pgproduction*)list_getptr( grammar->productions, i );
}

/* Retrieve: By offset from keft-hand side */

pgproduction* pg_production_get_by_lhs( pgnonterminal* lhs, int i )
{
	LIST*	l;

	if( !( pg_symbol_is_nonterminal( lhs ) && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgproduction*)NULL;
	}

	return (pgproduction*)list_getptr( lhs->productions, i );
}

/* Attribute: id */

/* GET ONLY! */
int pg_production_get_id( pgproduction* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return -1;
	}

	return list_find( p->grammar->productions, p );
}

/* Attribute: grammar */

/* GET ONLY! */
pggrammar* pg_production_get_grammar( pgproduction* p )
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
pgnonterminal* pg_production_get_lhs( pgproduction* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (pgnonterminal*)NULL;
	}

	return p->lhs;
}
