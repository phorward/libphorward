/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	production.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

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

	p = (pgproduction*)plist_malloc( lhs->grammar->productions );

	p->rhs = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	p->select = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	/* Connect with grammar */
	p->grammar = pg_symbol_get_grammar( lhs );

	/* Connect with left-hand side */
	p->lhs = lhs;
	plist_push( lhs->productions, p );

	/* Fill in right-hand side symbols */
	va_start( args, lhs );

	while( ( sym = va_arg( args, pgsymbol* ) ) )
		pg_production_append( p, sym );

	va_end( args );

	return p;
}

pgproduction* pg_production_create_as_node(
	pgnonterminal* lhs, char* name, pgastfn func, ... )
{
	pgproduction*	p;
	pgsymbol*		sym;
	va_list			args;

	p = pg_production_create( lhs, (pgsymbol*)NULL );

	pg_production_set_astname( p, name );
	pg_production_set_astfunc( p, func );

	/* Fill in right-hand side symbols */
	va_start( args, func );

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

	p->select = plist_free( p->select );

	return (pgproduction*)NULL;
}

/* Debug */

char* pg_production_to_string( pgproduction* p )
{
	plistel*	e;
	pgsymbol*	sym;

	if( ( !p ) )
	{
		WRONGPARAM;
		return "";
	}

	p->strval = (char*)pfree( p->strval );

	p->strval = pstrcatstr( p->strval, p->lhs->name, FALSE );
	p->strval = pstrcatstr( p->strval, " : ", FALSE );

	plist_for( p->rhs, e )
	{
		sym = (pgsymbol*)plist_access( e );

		if( e != plist_first( p->rhs ) )
			p->strval = pstrcatstr( p->strval, " ", FALSE );

		if( pg_symbol_is_terminal( sym ) )
			p->strval = pstrcatstr( p->strval, "@", FALSE );

		p->strval = pstrcatstr( p->strval, sym->name, FALSE );
	}

	return p->strval;
}

void pg_production_print( pgproduction* p, FILE* f )
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

	pg_symbol_print( p->lhs, f );
	fprintf( f, " : " );

	for( i = 0; ( sym = pg_production_get_rhs( p, i ) ); i++ )
	{
		if( i > 0 )
			fprintf( f, " " );

		pg_symbol_print( sym, f );
	}

	fprintf( f, "\n" );
}

/* Append right-hand side */

pboolean pg_production_append( pgproduction* p, pgsymbol* sym )
{
	if( !( p && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	plist_push( p->rhs, sym );
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

	return (pgproduction*)plist_access( plist_get( grammar->productions, i ) );
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

	return (pgproduction*)plist_access( plist_get( lhs->productions, i ) );
}

/* Retrieve: Right-hand side */

pgsymbol* pg_production_get_rhs( pgproduction* p, int i )
{
	if( !( p && i >= 0 ) )
	{
		WRONGPARAM;
		return (pgsymbol*)NULL;
	}

	return (pgsymbol*)plist_access( plist_get( p->rhs, i ) );
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

	return plist_offset( plist_get_by_ptr( p->grammar->productions, p ) );
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

/* Attribute: rhs_length */

/* GET ONLY! */
int pg_production_get_rhs_length( pgproduction* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return 0;
	}

	return plist_count( p->rhs );
}

/* Attribute: astname */

char* pg_production_get_astname( pgproduction* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (char*)NULL;
	}

	return p->astname;
}

pboolean pg_production_set_astname( pgproduction* p, char* name )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	psetstr( &p->astname, name );
	return TRUE;
}

/* Attribute: astfunc */

pgastfn pg_production_get_astfunc( pgproduction* p )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return (pgastfn)NULL;
	}

	return p->astfunc;
}

pboolean pg_production_set_astfunc( pgproduction* p, pgastfn func )
{
	if( !( p ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	p->astfunc = func;
	return TRUE;
}
