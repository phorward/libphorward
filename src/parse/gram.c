/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	gram.c
Usage:	Grammar construction and preparation.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define NAMELEN				72

/** Creates a new ppgram-object. */
ppgram* pp_gram_create( void )
{
	ppgram*		g;

	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ), PLIST_MOD_RECYCLE
													| PLIST_MOD_EXTKEYS
														| PLIST_MOD_UNIQUE );
	g->prods = plist_create( sizeof( ppprod ), PLIST_MOD_RECYCLE );

	g->eof = pp_sym_create( g, PPSYM_T_EOF, PPFLAG_SPECIAL );

	return g;
}


/** Prepares the grammar //g// by computing all necessary stuff required for
runtime and parser generator.

The preparation process includes:
- Setting up final symbol and productions IDs
- Nonterminals FIRST-set computation
- Marking of left-recursions
- The 'lexem'-flag pull-through the grammar.
-

This function is only run internally.
Don't call it if you're unsure ;)... */
pboolean pp_gram_prepare( ppgram* g )
{
	plistel*		e;
	plistel*		f;
	ppprod*			prod;
	ppprod*			cprod;
	ppsym*			sym;
	pboolean		nullable;
	plist*			call;
	plist*			done;
	int				i;
	int				cnt;
	int				pcnt;
	unsigned int	idx;

	if( !g )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !g->goal )
	{
		/* No such goal! */
		fprintf( stderr, "Grammar has no goal!\n" );
		return FALSE;
	}

	/* Reset symbols */
	for( idx = 0, e = plist_first( g->symbols ); e; e = plist_next( e ), idx++ )
	{
		sym = (ppsym*)plist_access( e );
		sym->idx = idx;

		if( PPSYM_IS_TERMINAL( sym ) )
		{
			if( !plist_first( sym->first ) )
				plist_push( sym->first, sym );
		}
		else
			plist_erase( sym->first );
	}

	/* Reset productions */
	for( idx = 0, e = plist_first( g->prods ); e; e = plist_next( e ), idx++ )
	{
		prod = (ppprod*)plist_access( e );
		prod->idx = idx;
	}

	/* Compute FIRST sets and mark left-recursions */
	cnt = 0;
	call = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );
	done = plist_create( 0, PLIST_MOD_PTR | PLIST_MOD_RECYCLE );

	do
	{
		pcnt = cnt;
		cnt = 0;

		plist_for( g->prods, e )
		{
			cprod = (ppprod*)plist_access( e );
			plist_push( call, cprod );

			while( plist_pop( call, &prod ) )
			{
				plist_push( done, prod );

				plist_for( prod->rhs, f )
				{
					sym = (ppsym*)plist_access( f );

					nullable = FALSE;

					if( !PPSYM_IS_TERMINAL( sym ) )
					{
						/* Union first set */
						plist_union( cprod->lhs->first, sym->first );

						/* Put prods on stack */
						for( i = 0; ( prod = pp_sym_getprod( sym, i ) ); i++ )
						{
							if( plist_count( prod->rhs ) == 0 )
							{
								nullable = TRUE;
								continue;
							}

							if( prod == cprod )
							{
								prod->lhs->flags |= PPFLAG_LEFTREC;
								prod->flags |= PPFLAG_LEFTREC;
							}
							else if( !plist_get_by_ptr( done, prod ) )
								plist_push( call, prod );
						}
					}
					/* Extend first set if required */
					else if( !plist_get_by_ptr( cprod->lhs->first, sym ) )
						plist_push( cprod->lhs->first, sym );

					if( !nullable )
						break;
				}

				/* Flag nullable */
				if( !f )
				{
					cprod->flags |= PPFLAG_NULLABLE;
					cprod->lhs->flags |= PPFLAG_NULLABLE;
				}

				cnt += plist_count( cprod->lhs->first );
			}

			plist_erase( done );
		}
	}
	while( pcnt < cnt );

	plist_clear( call );
	plist_clear( done );

	/* Pull-through all lexem symbols */
	plist_for( g->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		if( PPSYM_IS_TERMINAL( sym ) || !( sym->flags & PPFLAG_LEXEM ) )
			continue;

		plist_push( call, sym );
	}

	while( plist_pop( call, &sym ) )
	{
		plist_push( done, sym );

		for( i = 0; ( prod = pp_sym_getprod( sym, i ) ); i++ )
		{
			plist_for( prod->rhs, f )
			{
				sym = (ppsym*)plist_access( f );
				sym->flags |= PPFLAG_LEXEM;

				if( !PPSYM_IS_TERMINAL( sym ) )
				{
					if( !plist_get_by_ptr( done, sym )
						&& !plist_get_by_ptr( call, sym ) )
						plist_push( call, sym );
				}
			}
		}
	}

	/* Clear all lists */
	plist_free( call );
	plist_free( done );

	/* Inherit precedences */
	plist_for( g->prods, e )
	{
		prod = (ppprod*)plist_access( e );

		if( prod->prec < prod->lhs->prec )
			prod->prec = prod->lhs->prec;

		for( f = plist_last( prod->rhs ); f; f = plist_prev( f ) )
		{
			sym = (ppsym*)plist_access( f );
			if( sym->prec > prod->prec )
			{
				prod->prec = sym->prec;
				break;
			}
		}
	}

	/* Set finalized */
	g->flags |= PPFLAG_FINALIZED;
	g->strval = pfree( g->strval );

	return TRUE;
}

/** Dump grammar to trace.

The PP_GRAM_DUMP-macro is used to dump all relevant contents of a ppgram object
into the program trace, for debugging purposes.

PP_GRAM_DUMP() can only be used when the function was trace-enabled by PROC()
before.
*/
/*MACRO:PP_GRAM_DUMP( ppgram* g )*/
void _dbg_gram_dump( char* file, int line, char* function,
						char* name, ppgram* g )
{
	plistel*	e;
	plistel*	f;
	ppprod*		p;
	ppsym*		s;
	int			maxlhslen	= 0;
	int			maxemitlen	= 0;
	int			maxsymlen	= 0;

	if( !_dbg_trace_enabled( file, function ) )
		return;

	_dbg_trace( file, line, "GRAMMAR", function, "%s = {", name );

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		if( pstrlen( s->emit ) > maxemitlen )
			maxemitlen = pstrlen( s->emit );

		if( !PPSYM_IS_TERMINAL( s ) && pstrlen( s->name ) > maxlhslen )
			maxlhslen = pstrlen( s->name );

		if( pstrlen( s->name ) > maxsymlen )
			maxsymlen = pstrlen( s->name );
	}

	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		fprintf( stderr,
			"%s%s%s%s%s%s%-*s %-*s : ",
			g->goal == p->lhs ? "$" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			p->lhs->flags & PPFLAG_LEXEM ? "X" : " ",
			p->lhs->flags & PPFLAG_WHITESPACE ? "W" : " ",
			p->emit ? "@" : " ",
			maxemitlen, p->emit ? p->emit : "",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (ppsym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				fprintf( stderr, " " );

			fprintf( stderr, "%s", pp_sym_to_str( s ) );
		}

		fprintf( stderr, "\n" );
	}

	fprintf( stderr, "\n" );

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		fprintf( stderr, "%03d  %-*s  %-*s",
			s->idx, maxemitlen, s->emit ? s->emit : "",
				maxsymlen, s->name );

		if( !PPSYM_IS_TERMINAL( s ) && plist_count( s->first ) )
		{
			fprintf( stderr, " {" );

			plist_for( s->first, f )
			{
				s = (ppsym*)plist_access( f );
				fprintf( stderr, " " );
				fprintf( stderr, "%s", pp_sym_to_str( s ) );
			}

			fprintf( stderr, " }" );
		}
		else if( PPSYM_IS_TERMINAL( s ) && s->ptn )
			fprintf( stderr, " /%s/", pregex_ptn_to_regex( s->ptn ) );

		fprintf( stderr, "\n" );
	}

	_dbg_trace( file, line, "GRAMMAR", function, "}" );
}

/** Get grammar string representation */
char* pp_gram_to_str( ppgram* grm )
{
	ppsym*		sym;
	ppsym*		psym;
	ppprod*		prod;
	int			maxlhslen	= 0;
	int			maxsymlen	= 0;
	plistel*	e;
	plistel*	f;
	plistel*	g;
	char		name		[ NAMELEN + 3 + 1 ];
	pboolean	first;

	if( !grm )
	{
		WRONGPARAM;
		return "";
	}

	if( !( grm->flags & PPFLAG_FINALIZED ) )
		pp_gram_prepare( grm );

	if( grm->strval )
		return grm->strval;

	/* Find longest lhs */
	plist_for( grm->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		if( !PPSYM_IS_TERMINAL( sym ) )
		{
			if( pstrlen( sym->name ) > maxlhslen )
				maxlhslen = pstrlen( sym->name );
			else if( !sym->name && maxlhslen < 4 )
				maxlhslen = 4;
		}

		if( pstrlen( sym->name ) > maxsymlen )
			maxsymlen = pstrlen( sym->name );
		else if( !sym->name && maxsymlen < 4 )
			maxsymlen = 4;
	}

	if( maxlhslen > NAMELEN )
		maxlhslen = NAMELEN;

	if( maxsymlen > NAMELEN )
		maxsymlen = NAMELEN;

	/* Generate! */
	plist_for( grm->symbols, e )
	{
		if( PPSYM_IS_TERMINAL( ( sym = (ppsym*)plist_access( e ) ) ) )
			continue;

		first = TRUE;
		sprintf( name, "%-*s : ", maxlhslen, pp_sym_to_str( sym ) );

		grm->strval = pstrcatstr( grm->strval, name, FALSE );

		plist_for( grm->prods, f )
		{
			prod = (ppprod*)plist_access( f );

			if( prod->lhs != sym )
				continue;

			if( !first )
			{
				sprintf( name, "%-*s | ", maxlhslen, "" );
				grm->strval = pstrcatstr( grm->strval, name, FALSE );
			}
			else
				first = FALSE;

			if( !plist_first( prod->rhs ) )
				grm->strval = pstrcatchar( grm->strval, '\n' );

			plist_for( prod->rhs, g )
			{
				psym = (ppsym*)plist_access( g );

				sprintf( name, "%.*s%s", maxsymlen, pp_sym_to_str( psym ),
					g == plist_last( prod->rhs ) ? "\n" : " ");
				grm->strval = pstrcatstr( grm->strval, name, FALSE );
			}
		}

		sprintf( name, "%-*s ;\n", maxlhslen, "" );
		grm->strval = pstrcatstr( grm->strval, name, FALSE );
	}

	return grm->strval;
}

/** Frees grammar //g// and all its related memory. */
ppgram* pp_gram_free( ppgram* g )
{
	if( !g )
		return (ppgram*)NULL;

	g->flags &= ~PPFLAG_FROZEN;

	/* Erase symbols */
	while( plist_count( g->symbols ) )
		pp_sym_free( (ppsym*)plist_access( plist_first( g->symbols ) ) );

	/* Erase productions */
	while( plist_count( g->prods ) )
		pp_prod_free( (ppprod*)plist_access( plist_first( g->prods ) ) );

	plist_free( g->symbols );
	plist_free( g->prods );

	pfree( g->strval );

	pfree( g );

	return (ppgram*)NULL;
}
