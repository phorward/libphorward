/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	ll.c
Usage:	Phorward Parsing Library
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* LL parser with left-recursion support */

static pboolean is_direct_lrec( ppprod* p )
{
	int		i;
	ppsym*	s;

	for( i = 0; ( s = pp_prod_getfromrhs( p, i ) ); i++ )
	{
		if( s == p->lhs )
			return TRUE;

		if( !( s->flags & PPFLAG_NULLABLE ) )
			break;
	}

	return FALSE;
}

static pboolean pp_ll_PARSE( plist* ast, ppgram* grm,
								char* start, char** end, ppsym* sym )
{
	pboolean	myast	= FALSE;
	ppsym*		rsym;
	ppprod*		p;
	char*		ptr;
	int			loop	= 0;
	int			i;
	int			j;
	pboolean	pnext;
	pboolean	rnext;
	ppmatch*	match;
	ppmatch*	smatch;
	ppmatch*	ematch;
	plistel*	sme;
	plistel*	e;

	if( !( grm && start && end && sym ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !ast )
	{
		ast = plist_create( sizeof( ppmatch ), PLIST_MOD_RECYCLE );
		myast = TRUE;
	}

	do
	{
		/*
		printf( "loop = %d\n", loop );
		*/

		smatch = (ppmatch*)plist_malloc( ast );
		smatch->type = PPMATCH_BEGIN;
		sme = plist_last( ast );

		for( i = 0, pnext = TRUE;
				pnext && ( p = (ppprod*)plist_access(
									plist_get( sym->productions, i ) ) ); i++ )
		{
			/*
			printf( "Trying %s\n", pp_prod_to_str( p ) );
			*/

			if( !( ( loop == 0 && !is_direct_lrec( p ) )
						|| ( loop > 0 && is_direct_lrec( p ) ) ) )
				continue;

			for( j = 0, ptr = start, rnext = TRUE;
					rnext && ( rsym = pp_prod_getfromrhs( p, j ) ); j++ )
			{
				/*
				printf( "Testing >%s<\n", ptr );
				*/
				switch( rsym->type )
				{
					case PPSYMTYPE_CCL:
						if( p_ccl_test( rsym->ccl, *ptr ) )
							ptr++;
						else
							rnext = FALSE;

						break;

					case PPSYMTYPE_STRING:
						if( !strncmp( ptr, rsym->name, strlen( rsym->name ) ) )
							ptr += strlen( rsym->name );
						else
							rnext = FALSE;

						break;

					case PPSYMTYPE_NONTERM:
						for( e = plist_prev( sme ); e; e = plist_prev( e ) )
						{
							match = (ppmatch*)plist_access( e );
							if( match->start < ptr )
							{
								e = (plistel*)NULL;
								break;
							}

							if( match->type == PPMATCH_END
									&& match->sym == rsym )
							{
								ptr = match->end;
								break;
							}
						}

						if( !e )
							rnext = pp_ll_PARSE( ast, grm, ptr, &ptr, rsym );

						break;

					default:
						MISSINGCASE;
						rnext = FALSE;
						break;
				}
			}

			if( rnext )
				break;

			while( plist_next( sme ) )
				plist_remove( ast, plist_next( sme ) );
		}

		if( !( pnext && p ) )
		{
			plist_remove( ast, sme );
			break;
		}

		ematch = (ppmatch*)plist_malloc( ast );
		ematch->type = PPMATCH_END;
		smatch->start = ematch->start = start;
		smatch->end = ematch->end = ptr;
		smatch->sym = ematch->sym = sym;
		smatch->prod = ematch->prod = p;

		*end = ptr;
		loop++;
	}
	while( sym->flags & PPFLAG_LEFTREC );

	if( myast )
		plist_free( ast );

	return loop ? TRUE : FALSE;
}

pboolean pp_ll_parse( plist* ast, ppgram* grm, char* start, char** end )
{
	plistel*	e;
	ppmatch*	match;
	pboolean	myast	= FALSE;
	pboolean	ret;

	if( !( grm && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !ast )
	{
		ast = plist_create( sizeof( ppmatch ), PLIST_MOD_RECYCLE );
		myast = TRUE;
	}

	ret = pp_ll_PARSE( ast, grm, start, end, grm->goal );

	if( myast )
	{
		char	gap[80+1];
		*gap = '\0';

		for( e = plist_first( ast ); e; e = plist_next( e ) )
		{
			match = (ppmatch*)plist_access( e );

			if( match->type == PPMATCH_END && *gap )
				gap[ strlen( gap ) - 1 ] = '\0';

			printf( "%s%s %s >%.*s<\n",
				gap,
				match->type == PPMATCH_BEGIN ? "{" : "}",
				match->sym->name,
				match->end - match->start,
				match->start );

			if( match->type == PPMATCH_BEGIN )
				strcat( gap, " " );
		}

		plist_free( ast );
	}

	return ret;
}
