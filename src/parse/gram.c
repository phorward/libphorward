/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	gram.c
Usage:	Grammar-specific stuff.
		THIS SOURCE IS UNDER DEVELOPMENT AND EXPERIMENTAL.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define NAMELEN			80
#define DERIVCHAR		'\''

#define T_CCL			PPSYMTYPE_CCL
#define T_STRING		PPSYMTYPE_STRING
#define T_REGEX			PPSYMTYPE_REGEX
#define T_IDENT			10
#define T_INT			11
#define T_FLOAT			12

#define T_RECOGNIZER	15

#define T_SYMBOL		20

#define T_KLEENE		25
#define T_POSITIVE		26
#define T_OPTIONAL		27

#define T_PRODUCTION	30
#define T_NONTERMDEF	31
#define T_INLINE		32

#define T_TERMDEF		35

#define T_FLAG			40
#define T_GFLAG			41
#define T_EMIT			42

#define T_DOCMD			50
#define T_DOPARAM		51

#define T_DOBEFORE		60
#define T_DOWITHIN		61
#define T_DOBEHIND		62

/* Derive name from basename */
static char* derive_name( ppgram* g, char* base )
{
	int             i;
	static
	char    deriv   [ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !pp_sym_get_by_name( g, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

/* Stack machine compiling AST to ppgram. */
static pboolean ast_to_gram( ppgram* g, parray* ast )
{
	typedef struct
	{
		int			emit;
		char*		buf;
		ppsym*		sym;
		int			i;
		double		d;
		ppdofunc	func;
		pany*		any;
	} ATT;

	ATT			att;
	ATT*		attp;
	parray*		st;
	parray*		dec;
	ppmatch*	e;
	ppsym*		sym;
	ppsym*		nonterm;
	ppprod*		p;
	int			i;
	pboolean	ignore;
	pboolean	doemit;
	pboolean	emitall		= FALSE;
	int			emit;
	int			emit_max	= 0;
	char		name		[ NAMELEN * 2 + 1 ];

	st = parray_create( sizeof( ATT ), 0 );
	dec = parray_create( sizeof( ppsym* ), 0 );

	for( e = (ppmatch*)parray_first( ast );
			e <= (ppmatch*)parray_last( ast ); e++ )
	{
		doemit = emitall;
		emit = 0;
		ignore = FALSE;

		memset( &att, 0, sizeof( ATT ) );

		if( e->type & PPMATCH_BEGIN )
		{
			if( e->emit == T_NONTERMDEF )
			{
				e++;

				att.buf = pstrndup( e->start, e->end - e->start );

				if( !( nonterm = pp_sym_get_by_name( g, att.buf ) ) )
					nonterm = pp_sym_create( g, PPSYMTYPE_NONTERM,
												att.buf, (char*)NULL );

				parray_push( dec, &nonterm );
				pfree( att.buf );
			}
			else if( e->emit == T_INLINE )
			{
				sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
							derive_name( g, nonterm->name ), (char*)NULL );
				sym->flags |= PPFLAG_DEFINED;

				parray_push( dec, &sym );
			}
			else if( e->emit == T_PRODUCTION )
				/* Push an empty sequence delimiter */
				parray_push( st, &att );
		}

		if( !( e->type & PPMATCH_END ) )
			continue;

		/*
		fprintf( stderr, "emit %d >%.*s<\n",
			e->emit, e->end - e->start, e->start );
		*/

		switch( ( att.emit = e->emit ) )
		{
			case T_IDENT:
				att.buf = pstrndup( e->start, e->end - e->start );
				break;

			case T_INT:
				att.i = atoi( e->start );
				break;

			case T_FLOAT:
				att.d = atof( e->start );
				break;

			case T_CCL:
			case T_STRING:
			case T_REGEX:
				att.buf = pstrndup( e->start + 1, e->end - e->start - 2 );
				break;

			case T_INLINE:
				/* Inline nonterminal */
				att.sym = *( (ppsym**)parray_pop( dec ) );
				att.emit = T_SYMBOL;
				break;

			case T_SYMBOL:
				/* Symbol */
				attp = parray_pop( st );

				if( attp->sym )
					att.sym = attp->sym;
				else if( attp->buf )
				{
					if( attp->emit == T_IDENT )
					{
						if( !( att.sym = pp_sym_get_by_name( g, attp->buf ) ) )
							att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
										attp->buf, (char*)NULL );
					}
					else if( !( att.sym = pp_sym_get_nameless_term_by_def(
												g, attp->buf ) ) )
					{
						att.sym = pp_sym_create( g, attp->emit,
													(char*)NULL, attp->buf );
						att.sym->flags |= PPFLAG_DEFINED;

						if( emitall )
							att.sym->emit = ++emit_max;
					}

					pfree( attp->buf );
				}
				else
				{
					fprintf( stderr, "CHECK YOUR CODE:\n" );
					fprintf( stderr, "%s, %d: Can't find symbol.\n",
										__FILE__, __LINE__ );
				}

				att.sym->flags |= PPFLAG_CALLED;
				break;

			case T_KLEENE:
			case T_POSITIVE:
			case T_OPTIONAL:
				attp = parray_pop( st );

				if( e->emit == T_KLEENE || e->emit == T_POSITIVE )
				{
					sprintf( name, "%s%c", attp->sym->name, PPMOD_POSITIVE );

					if( !( att.sym = pp_sym_get_by_name( g, name ) ) )
					{
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
														name, (char*)NULL );

						if( g->flags & PPFLAG_PREVENTLREC )
							pp_prod_create( g, att.sym,
								attp->sym, att.sym, (ppsym*)NULL );
						else
							pp_prod_create( g, att.sym,
								att.sym, attp->sym, (ppsym*)NULL );

						pp_prod_create( g, att.sym, attp->sym, (ppsym*)NULL );
					}

					attp->sym = att.sym;
				}

				if( e->emit == T_OPTIONAL || e->emit == T_KLEENE )
				{
					sprintf( name, "%s%c", attp->sym->name, PPMOD_OPTIONAL );

					if( !( att.sym = pp_sym_get_by_name( g, name ) ) )
					{
						att.sym = pp_sym_create( g, PPSYMTYPE_NONTERM,
														name, (char*)NULL );

						pp_prod_create( g, att.sym, attp->sym, (ppsym*)NULL );
						pp_prod_create( g, att.sym, (ppsym*)NULL );
					}
				}

				att.emit = T_SYMBOL;
				break;

			case T_PRODUCTION:
				p = pp_prod_create( g, *( (ppsym**)parray_last( dec ) ),
											(ppsym*)NULL );

				while( ( attp = (ATT*)parray_last( st ) )
							&& ( attp->emit == T_FLAG
									|| attp->emit == T_EMIT
										|| attp->emit == T_DOBEFORE
											|| attp->emit == T_DOWITHIN
												|| attp->emit == T_DOBEHIND ) )
				{
					parray_pop( st );

					switch( attp->emit )
					{
						case T_EMIT:
							doemit = TRUE;
							emit = attp->i;
							break;

						case T_FLAG:
							if( strcmp( attp->buf, "noemit" ) == 0 )
								doemit = FALSE;

							pfree( attp->buf );
							break;

						default:
							break;
					}
				}

				for( i = 0; ( attp = (ATT*)parray_pop( st ) )
								&& attp->emit == T_SYMBOL; i++ )
					;

				while( i-- )
					pp_prod_append( p, ( ++attp )->sym );

				/* Set emit ID if configured */
				if( doemit )
				{
					if( emit )
					{
						p->emit = emit;

						if( emit > emit_max )
							emit_max = emit;
					}
					else
						p->emit = ++emit_max;
				}

				att.emit = 0;
				break;

			case T_FLAG:
				att.buf = pstrndup( e->start, e->end - e->start );
				break;

			case T_GFLAG:
				if( !strncmp( e->start, "emitall", e->end - e->start ) )
					emitall = TRUE;
				else if( !strncmp( e->start, "emitnone", e->end - e->start ) )
					emitall = FALSE;
				else if( !strncmp( e->start, "lrec", e->end - e->start ) )
					g->flags &= ~PPFLAG_PREVENTLREC;
				else if( !strncmp( e->start, "rrec", e->end - e->start ) )
					g->flags |= PPFLAG_PREVENTLREC;

				att.emit = 0;
				break;

			case T_EMIT:
				if( ( attp = (ATT*)parray_last( st ) ) && attp->emit == T_INT )
				{
					parray_pop( st );
					att.i = attp->i;
				}
				break;

			case T_TERMDEF:
				while( ( attp = (ATT*)parray_last( st ) )
							&& ( attp->emit == T_FLAG
									|| attp->emit == T_EMIT
										|| attp->emit == T_DOBEFORE
											|| attp->emit == T_DOWITHIN
												|| attp->emit == T_DOBEHIND ) )
				{
					parray_pop( st );

					switch( attp->emit )
					{
						case T_EMIT:
							doemit = TRUE;
							emit = attp->i;
							break;

						case T_FLAG:
							if( strcmp( attp->buf, "noemit" ) == 0 )
								doemit = FALSE;
							else if( strcmp( attp->buf, "ignore" ) == 0
										|| strcmp( attp->buf, "skip" ) == 0 )
								ignore = TRUE;

							pfree( attp->buf );
							break;

						default:
							break;
					}
				}

				attp = parray_pop( st ); 	/* Definition */

				if( parray_last( st )
						&& ( (ATT*)parray_last( st ) )->emit == T_IDENT )
				{
					attp = parray_pop( st );	/* Identifier */

					sym = pp_sym_create( g, ( attp + 1 )->emit,
										attp->buf, ( attp + 1 )->buf );

					pfree( ( attp + 1 )->buf );
				}
				else
					sym = pp_sym_create( g, attp->emit,
											(char*)NULL, attp->buf );

				pfree( attp->buf );

				sym->flags |= PPFLAG_DEFINED;

				/* Whitespace now only for terminals */
				if( ignore )
				{
					sym->flags |= PPFLAG_WHITESPACE;
					plist_push( g->ws, sym );
				}

				sym->flags |= PPFLAG_DEFINED;

				/* Set emit ID if configured */
				if( doemit )
				{
					if( emit )
					{
						sym->emit = emit;

						if( emit > emit_max )
							emit_max = emit;
					}
					else
						sym->emit = ++emit_max;
				}

				att.emit = 0;
				break;

			case T_NONTERMDEF:
				while( ( attp = (ATT*)parray_last( st ) )
							&& ( attp->emit == T_FLAG
									|| attp->emit == T_EMIT
										|| attp->emit == T_DOBEFORE
											|| attp->emit == T_DOWITHIN
												|| attp->emit == T_DOBEHIND ) )
				{
					parray_pop( st );

					switch( attp->emit )
					{
						case T_EMIT:
							doemit = TRUE;
							emit = attp->i;
							break;

						case T_FLAG:
							if( strcmp( attp->buf, "goal" ) == 0 )
							{
								if( nonterm && !g->goal ) /* fixme */
								{
									g->goal = nonterm;
									nonterm->flags |= PPFLAG_CALLED;
								}
							}
							else if( strcmp( attp->buf, "noemit" ) == 0 )
								doemit = FALSE;
							else if( strcmp( attp->buf, "ignore" ) == 0 )
								ignore = TRUE;
							else if( strcmp( attp->buf, "lexem" ) == 0 )
								nonterm->flags |= PPFLAG_LEXEM;

							pfree( attp->buf );
							break;

						default:
							break;
					}
				}

				nonterm->flags |= PPFLAG_DEFINED;
				parray_pop( st ); /* Identifier */
				parray_pop( dec );

				/* Set emit ID if configured */
				if( doemit )
				{
					if( emit )
					{
						nonterm->emit = emit;

						if( emit > emit_max )
							emit_max = emit;
					}
					else
						nonterm->emit = ++emit_max;
				}

				att.emit = 0;
				break;

			case T_DOCMD:
				attp = parray_pop( st );
				/* TODO: Find do-command and push pointer */
				break;

			case T_DOPARAM:
				attp = parray_pop( st );
				/* TODO: Generate pany */
				break;

			case T_DOBEFORE:
			case T_DOWITHIN:
			case T_DOBEHIND:
				/* TODO: Initialize action */

				while( ( attp = parray_pop( st ) ) && attp->emit == T_DOPARAM )
					/* TODO: Take parameter */
					;

				/* TODO: Take command. */
				break;
		}

		if( att.emit > 0 )
			parray_push( st, &att );
	}

	if( parray_count( st ) )
	{
		fprintf( stderr, "CHECK YOUR CODE:\n" );
		fprintf( stderr, "%s, %d: Still %d elements on stack\n",
			__FILE__, __LINE__, parray_count( st ) );

		while( attp = (ATT*)parray_pop( st ) )
			fprintf( stderr, "%d\n", attp->emit );

		parray_free( st );
		return FALSE;
	}

	parray_free( st );

	/* If there is no goal, then the last defined nonterm becomes goal symbol */
	if( !g->goal )
		g->goal = nonterm;

	/* Look for unique goal sequence */
	if( plist_count( g->goal->prods ) > 1 )
	{
		nonterm = pp_sym_create( g, PPSYMTYPE_NONTERM,
						derive_name( g, g->goal->name ), (char*)NULL );

		pp_prod_create( g, nonterm, g->goal, (ppsym*)NULL );
		g->goal = nonterm;
	}

	return TRUE;
}

/** Prepares the grammar //g// by computing all necessary stuff required for
runtime and parser generator.

The preparation process includes:
- Symbol and productions IDs
- FIRST-set computation
- Mark left-recursions
- Lexem flag pull-trough
-

*/
pboolean pp_gram_prepare( ppgram* g )
{
	plistel*	e;
	plistel*	er;
	plistel*	ep;
	ppprod*		p;
	ppprod*		cp;
	ppsym*		s;
	pboolean	nullable;
	plist*		call;
	plist*		done;
	int			id;
	int			cnt;
	int			pcnt;

	if( !g )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !g->goal )
	{
		/* No such goal! */
		return FALSE;
	}

	/* Redefine list of whitespace symbols */
	plist_erase( g->ws );

	/* Set ID values for symbols and productions */
	for( id = 0, e = plist_first( g->symbols ); e; e = plist_next( e ), id++ )
	{
		s = (ppsym*)plist_access( e );
		s->id = id;

		if( s->flags & PPFLAG_WHITESPACE )
			plist_push( g->ws, s );
	}

	for( id = 0, e = plist_first( g->prods ); e; e = plist_next( e ), id++ )
	{
		p = (ppprod*)plist_access( e );
		p->id = id;

		if( !p->emit )
			p->emit = p->lhs->emit;
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
			cp = (ppprod*)plist_access( e );
			plist_push( call, cp );

			while( plist_pop( call, &p ) )
			{
				plist_push( done, p );

				plist_for( p->rhs, er )
				{
					s = (ppsym*)plist_access( er );

					nullable = FALSE;

					if( s->type == PPSYMTYPE_NONTERM )
					{
						/* Union first set */
						plist_union( cp->lhs->first, s->first );

						/* Put prods on stack */
						plist_for( s->prods, ep )
						{
							p = (ppprod*)plist_access( ep );

							if( plist_count( p->rhs ) == 0 )
							{
								nullable = TRUE;
								continue;
							}

							if( p == cp )
							{
								p->lhs->flags |= PPFLAG_LEFTREC;
								p->flags |= PPFLAG_LEFTREC;
							}
							else if( !plist_get_by_ptr( done, p ) )
								plist_push( call, p );
						}
					}
					/* Extend first set if required */
					else if( !plist_get_by_ptr( cp->lhs->first, s ) )
						plist_push( cp->lhs->first, s );

					if( !nullable )
						break;
				}

				/* Flag nullable */
				if( !er )
				{
					cp->flags |= PPFLAG_NULLABLE;
					cp->lhs->flags |= PPFLAG_NULLABLE;
				}

				cnt += plist_count( cp->lhs->first );
			}

			plist_erase( done );
		}
	}
	while( pcnt < cnt );

	plist_clear( call );
	plist_clear( done );

	/* Pull-trough all lexem symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		if( s->type != PPSYMTYPE_NONTERM
				|| !( s->flags & PPFLAG_LEXEM ) )
			continue;

		plist_push( call, s );
	}

	while( plist_pop( call, &s ) )
	{
		plist_push( done, s );

		plist_for( s->prods, e )
		{
			p = (ppprod*)plist_access( e );
			plist_for( p->rhs, er )
			{
				s = (ppsym*)plist_access( er );
				s->flags |= PPFLAG_LEXEM;

				if( s->type == PPSYMTYPE_NONTERM )
				{
					if( !plist_get_by_ptr( done, s )
						&& !plist_get_by_ptr( call, s ) )
						plist_push( call, s );
				}
			}
		}
	}

	/* Clear all lists */
	plist_free( call );
	plist_free( done );

	return TRUE;
}

/** Creates a new ppgram-object. */
ppgram* pp_gram_create( void )
{
	ppsym*		s;
	ppgram*		g;

	/* Setup grammar description */
	g = (ppgram*)pmalloc( sizeof( ppgram ) );

	g->symbols = plist_create( sizeof( ppsym ), PLIST_MOD_RECYCLE
													| PLIST_MOD_EXTKEYS
														| PLIST_MOD_UNIQUE );
	g->prods = plist_create( sizeof( ppprod ), PLIST_MOD_RECYCLE );

	g->ws = plist_create( sizeof( ppsym* ), PLIST_MOD_PTR );
	g->eof = pp_sym_create( g, PPSYMTYPE_SPECIAL, "eof", (char*)NULL );

	return g;
}

/** Compiles a grammar definition into a grammar.

//g// is the grammar that receives the result of the parse.
//bnf// is the BNF definition string that defines the grammar.
*/
pboolean pp_gram_from_bnf( ppgram* g, char* bnf )
{
	ppgram*		bnfgram;
	char*		s = bnf;
	char*		e;
	parray*		a;

	if( !( g && bnf ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Define grammar for BNF */
	bnfgram = pp_gram_create();
	pp_bnf_define( bnfgram );
	pp_gram_prepare( bnfgram );

	if( !pp_lr_parse( &a, bnfgram, s, &e ) )
	{
		pp_gram_free( bnfgram );
		return FALSE;
	}

	/* pp_ast_simplify( a ); */

	if( !ast_to_gram( g, a ) )
		return FALSE;

	pp_gram_free( bnfgram );
	parray_free( a );

	return TRUE;
}

/** Dumps the grammar //g// to stdout. */
void pp_gram_print( ppgram* g )
{
	plistel*	e;
	plistel*	f;

	ppprod*		p;
	ppsym*		s;
	int			maxlhslen	= 0;

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		if( s->type == PPSYMTYPE_NONTERM && pstrlen( s->name ) > maxlhslen )
			maxlhslen = pstrlen( s->name );
	}

	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		printf( "%2d %s%s%s%s%s %-*s : ",
			p->lhs->emit,
			g->goal == p->lhs ? "G" : " ",
			p->flags & PPFLAG_LEFTREC ? "L" : " ",
			p->flags & PPFLAG_NULLABLE ? "N" : " ",
			p->lhs->flags & PPFLAG_LEXEM ? "X" : " ",
			p->lhs->flags & PPFLAG_WHITESPACE ? "W" : " ",
			maxlhslen, p->lhs->name );

		plist_for( p->rhs, f )
		{
			s = (ppsym*)plist_access( f );

			if( f != plist_first( p->rhs ) )
				printf( " " );

			printf( "%s", pp_sym_to_str( s ) );
		}

		printf( "\n" );
	}

	printf( "\n" );

	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );
		if( s->type != PPSYMTYPE_NONTERM )
			continue;

		printf( "FIRST %-*s {", maxlhslen, s->name );
		plist_for( s->first, f )
		{
			s = (ppsym*)plist_access( f );
			printf( " " );
			printf( "%s", pp_sym_to_str( s ) );
		}

		printf( " }\n" );
	}
}

/** Frees grammar //g// and all its related memory. */
ppgram* pp_gram_free( ppgram* g )
{
	plistel*	e;
	ppsym*		s;
	ppprod*		p;

	if( !g )
		return (ppgram*)NULL;

	/* Erase symbols */
	plist_for( g->symbols, e )
	{
		s = (ppsym*)plist_access( e );

		pfree( s->name );

		pfree( s->str );
		p_ccl_free( s->ccl );
		pregex_free( s->re );

		plist_free( s->prods );
		plist_free( s->first );
	}

	/* Erase productions */
	plist_for( g->prods, e )
	{
		p = (ppprod*)plist_access( e );
		plist_free( p->rhs );
	}

	plist_free( g->symbols );
	plist_free( g->prods );
	plist_free( g->ws );
	pfree( g );

	return (ppgram*)NULL;
}
