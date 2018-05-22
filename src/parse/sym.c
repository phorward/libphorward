/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	sym.c
Usage:	Symbol management function.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Check for a symbol type, whether it is configured to be a terminal or a
nonterminal symbol.

Returns TRUE in case //sym// is a terminal symbol, and FALSE otherwise.
*/
/*MACRO:PPSYM_IS_TERMINAL( ppsym* sym )*/

/** Creates a new symbol in the grammar //g//.

	//name// is the unique name for the symbol. It can be left empty,
	configuring the symbol as an unnamed symbol.

	//flags// can be a combination of PPFLAG_-flags related to the symbol's
	configuration.
*/
ppsym* pp_sym_create( ppgram* g, char* name, unsigned int flags )
{
	ppsym*	sym;

	if( !( g ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	if( g->flags & PPFLAG_FROZEN )
	{
		fprintf( stderr, "Grammar is frozen, can't create symbols\n" );
		return (ppsym*)NULL;
	}

	if( name && pp_sym_get_by_name( g, name ) )
	{
		fprintf( stderr, "Symbol '%s' already exists in this grammar\n", name );
		return (ppsym*)NULL;
	}

	if( flags & PPFLAG_FREENAME )
		name = pstrdup( name );

	/* Insert into symbol table */
	if( !( sym = (ppsym*)plist_access(
							plist_insert(
								g->symbols, (plistel*)NULL,
									name, (void*)NULL ) ) ) )
	{
		if( flags & PPFLAG_FREENAME )
			pfree( name );

		return (ppsym*)NULL;
	}

	sym->grm = g;

	if( !( sym->name = name ) )
		flags |= PPFLAG_NAMELESS;

	sym->flags = flags;

	sym->first = plist_create( 0, PLIST_MOD_PTR );

	g->flags &= ~PPFLAG_FINALIZED;

	return sym;
}

/** Frees a symbol. */
ppsym* pp_sym_free( ppsym* sym )
{
	if( !sym )
		return (ppsym*)NULL;

	sym->grm->flags &= ~PPFLAG_FINALIZED;

	if( sym->flags & PPFLAG_FREEEMIT )
		pfree( sym->emit );

	/* Remove symbol from pool */
	plist_remove( sym->grm->symbols,
		plist_get_by_ptr( sym->grm->symbols, sym ) );

	if( sym->flags & PPFLAG_FREENAME )
		pfree( sym->name );

	if( sym->ptn )
		pregex_ptn_free( sym->ptn );

	plist_free( sym->first );

	return (ppsym*)NULL;
}

/** Removes a symbol from its grammar. */
ppsym* pp_sym_drop( ppsym* sym )
{
	plistel* 	e;
	ppprod*		p;

	if( !sym )
		return (ppsym*)NULL;

	if( sym->grm->flags & PPFLAG_FROZEN )
	{
		fprintf( stderr, "Grammar is frozen, can't delete symbol\n" );
		return sym;
	}

	/* Remove all references from other productions */
	for( e = plist_first( sym->grm->prods ); e; )
	{
		p = (ppprod*)plist_access( e );
		e = plist_next( e );

		if( p->lhs == sym )
		{
			pp_prod_free( p );
			continue;
		}

		pp_prod_remove( p, sym );
	}

	/* Clear yourself */
	return pp_sym_free( sym );
}

/** Get the //n//th symbol from grammar //g//.
Returns (ppsym*)NULL if no symbol was found. */
ppsym* pp_sym_get( ppgram* g, unsigned int n )
{
	if( !( g ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get( g->symbols, n ) );
}

/** Get a symbol from grammar //g// by its //name//. */
ppsym* pp_sym_get_by_name( ppgram* g, char* name )
{
	if( !( g && name && *name ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	return (ppsym*)plist_access( plist_get_by_key( g->symbols, name ) );
}

/** Find a nameless terminal symbol by its pattern. */
ppsym* pp_sym_get_nameless_term_by_def( ppgram* g, char* name )
{
	int		i;
	ppsym*	sym;

	if( !( g && name && *name ) )
	{
		WRONGPARAM;
		return (ppsym*)NULL;
	}

	for( i = 0; ( sym = pp_sym_get( g, i ) ); i++ )
	{
		if( !PPSYM_IS_TERMINAL( sym ) || !( sym->flags & PPFLAG_NAMELESS ) )
			continue;

		if( sym->name && strcmp( sym->name, name ) == 0 )
			return sym;
	}

	return (ppsym*)NULL;
}

/** Get the //n//th production from symbol //sym//.
//sym// must be a nonterminal.

Returns (ppprod*)NULL if the production is not found or the symbol is
configured differently. */
ppprod* pp_sym_getprod( ppsym* sym, unsigned int n )
{
	plistel*	e;
	ppprod*		p;

	if( !( sym ) )
	{
		WRONGPARAM;
		return (ppprod*)NULL;
	}

	if( PPSYM_IS_TERMINAL( sym ) )
	{
		fprintf( stderr, "%s, %d: Symbol '%s' is not a non-terminal symbol\n",
			__FILE__, __LINE__, pp_sym_to_str( sym ) );
		return (ppprod*)NULL;
	}

	plist_for( sym->grm->prods, e )
	{
		p = (ppprod*)plist_access( e );
		if( p->lhs == sym && !n-- )
			return p;
	}

	return (ppprod*)NULL;
}

/** Returns the string representation of symbol //sym//.

	Nonterminals are not expanded, they are just returned as their name.
	The returned pointer is part of //sym// and can be referenced multiple
	times. It may not be freed by the caller. */
char* pp_sym_to_str( ppsym* sym )
{
	if( !sym )
	{
		WRONGPARAM;
		return "";
	}

	if( !sym->strval )
	{
		if( sym->name )
			sym->strval = pstrdup( sym->name );
		else
		{
			sym->strval = (char*)malloc( 4 + 1 * sizeof( char ) );
			sprintf( sym->strval, "%c%03d",
				PPSYM_IS_TERMINAL( sym ) ? 'T' : 'n', sym->idx );
		}
	}

	return sym->strval;
}

/** Constructs a positive closure from //sym//.

This helper function constructs the productions and symbol
```
pos_sym: sym pos_sym | sym ;
```
from //sym//, and returns the symbol //pos_sym//. If //pos_sym// already exists,
the symbol will be returned without any creation.
*/
ppsym* pp_sym_mod_positive( ppsym* sym )
{
	ppsym*		ret;
	char		buf		[ BUFSIZ + 1 ];
	char*		name	= buf;
	size_t		len;

	PROC( "pp_sym_mod_positive" );
	PARMS( "sym", "%p", sym );

	if( !sym )
	{
		WRONGPARAM;
		RETURN( (ppsym*)NULL );
	}

	len = pstrlen( sym->name ) + 4;
	if( len > BUFSIZ )
		name = (char*)pmalloc( ( len + 1 ) * sizeof( char ) );

	sprintf( name, "pos_%s", pstrget( sym->name ) );

	VARS( "name", "%s", name );

	if( !( ret = pp_sym_get_by_name( sym->grm, name ) ) )
	{
		MSG( "Symbol does not exist yet, creating" );

		ret = pp_sym_create( sym->grm, name,
				PPFLAG_DEFINED | PPFLAG_CALLED | PPFLAG_GENERATED
					| ( name == buf ? PPFLAG_FREENAME : PPFLAG_NONE ) );

		/* Flag config must be done here do avoid pstrdup() previously */
		if( name != buf )
			ret->flags |= PPFLAG_FREENAME;

		if( sym->grm->flags & PPFLAG_PREVENTLREC )
			pp_prod_create( sym->grm, ret, sym, ret, (ppsym*)NULL );
		else
			pp_prod_create( sym->grm, ret, ret, sym, (ppsym*)NULL );

		pp_prod_create( sym->grm, ret, sym, (ppsym*)NULL );
	}
	else if( name != buf )
		pfree( name );

	RETURN( ret );
}

/** Constructs an optional closure from //sym//.

This helper function constructs the productions and symbol
```
opt_sym: sym | ;
```
from //sym//, and returns the symbol //pos_sym//. If //opt_sym// already exists,
the symbol will be returned without any creation.
*/
ppsym* pp_sym_mod_optional( ppsym* sym )
{
	ppsym*		ret;
	char		buf		[ BUFSIZ + 1 ];
	char*		name	= buf;
	size_t		len;

	PROC( "pp_sym_mod_optional" );
	PARMS( "sym", "%p", sym );

	if( !sym )
	{
		WRONGPARAM;
		RETURN( (ppsym*)NULL );
	}

	len = pstrlen( sym->name ) + 4;
	if( len > BUFSIZ )
		name = (char*)pmalloc( ( len + 1 ) * sizeof( char ) );

	sprintf( name, "opt_%s", pstrget( sym->name ) );

	VARS( "name", "%s", name );

	if( !( ret = pp_sym_get_by_name( sym->grm, name ) ) )
	{
		MSG( "Symbol does not exist yet, creating" );
		ret = pp_sym_create( sym->grm, name,
				PPFLAG_DEFINED | PPFLAG_CALLED | PPFLAG_GENERATED
					| ( name == buf ? PPFLAG_FREENAME : PPFLAG_NONE ) );

		/* Flag config must be done here do avoid pstrdup() previously */
		if( name != buf )
			ret->flags |= PPFLAG_FREENAME;

		pp_prod_create( sym->grm, ret, sym, (ppsym*)NULL );
		pp_prod_create( sym->grm, ret, (ppsym*)NULL );
	}
	else if( name != buf )
		pfree( name );

	RETURN( ret );
}

/** Constructs an optional positive ("kleene") closure from //sym//.

This helper function constructs the productions and symbol
```
pos_sym: sym pos_sym | sym ;
opt_sym: pos_sym | ;
```
from //sym//, and returns the symbol //opt_sym//. If any of the given symbols
already exists, they are directly used. The function is a shortcut for a call
```
pp_sym_mod_optional( pp_sym_mod_positive( sym ) )
```
*/
ppsym* pp_sym_mod_kleene( ppsym* sym )
{
	PROC( "pp_sym_mod_kleene" );
	PARMS( "sym", "%p", sym );

	if( !sym )
	{
		WRONGPARAM;
		RETURN( (ppsym*)NULL );
	}

	sym = pp_sym_mod_positive( sym );
	sym = pp_sym_mod_optional( sym );

	RETURN( sym );
}
