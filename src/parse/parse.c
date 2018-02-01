/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	parse.c
Usage:	Parser maintainance object.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define DEBUGLEVEL		0

/** Creates a new parser object with flags //flags// and the grammar //bnf//. */
pparser* pp_parser_create( ppgram* g )
{
	pparser*	p;

	if( !g )
	{
		WRONGPARAM;
		return (pparser*)NULL;
	}

	p = (pparser*)pmalloc( sizeof( pparser ) );

	/* Init */
	parray_init( &p->tokens, sizeof( ppsym* ), 0 );

	/* Grammar */
	p->gram = g;
	pp_gram_prepare( p->gram );

	if( !pp_lr_build( &p->states, &p->dfa, p->gram ) )
		return pp_parser_free( p );

	return p;
}

/** Free parser //par//. */
pparser* pp_parser_free( pparser* p )
{
	if( !p )
		return (pparser*)NULL;

	plex_free( p->lex );

	parray_erase( &p->tokens );
	pfree( p );

	return (pparser*)NULL;
}

/** Automatically generate tokens */
int pp_parser_auto_token( pparser* p )
{
	ppsym*		sym;
	plistel*	e;
	int			gen = 0;
	char*		emit;
	char*		ptr;

	if( !( p ) )
	{
		WRONGPARAM;
		return 0;
	}

	plist_for( p->gram->symbols, e )
	{
		sym = (ppsym*)plist_access( e );

		if( PPSYM_IS_TERMINAL( sym )
				&& !parray_partof( &p->tokens, sym )
					&& ( ptr = sym->name ) )
		{
			/* emit = strlen( ptr ) > 1 && *ptr == '@' ? ++ptr : (char*)NULL; */

			pp_parser_define_token( p, sym, sym->name, PREGEX_COMP_STATIC );

			/*
			if( !sym->emit )
				sym->emit = emit;
			*/

			gen++;
		}
	}

	return gen;
}

/** Define token */
pboolean pp_parser_define_token( pparser* p, ppsym* sym, char* pat, int flags )
{
	plistel*		e;

	if( !( p && sym && pat ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !p->lex )
		p->lex = plex_create( flags );

	parray_push( &p->tokens, &sym );
	plex_define( p->lex, pat, (int)parray_count( &p->tokens ), flags );

	return TRUE;
}

#if DEBUGLEVEL > 2
/* Function to dump the parse stack content */
static void print_stack( char* title, plist* states, parray* stack )
{
	pplrse*	e;
	int		i;

	fprintf( stderr, "STACK DUMP %s\n", title );

	for( i = 0; i < parray_count( stack ); i++ )
	{
		e = (pplrse*)parray_get( stack, i );
		fprintf( stderr, "%02d: %s %d >%.*s<\n",
			i, e->symbol ? e->symbol->name : "(null)",
				e->state ?
					plist_offset( plist_get_by_ptr( states, e->state ) ) : -1,
			e->end - e->start, e->start );
	}
}
#endif


/* LR-Stackitem */
typedef struct
{
	int				state;			/* State */

	char*			start;			/* Begin of match */
	char*			end;			/* End of match */

	ppast*			node;			/* AST construction */

	int				row;			/* Positioning in source */
	int				col;			/* Positioning in source */
} pplrse;


static ppsym* pp_parser_scan( pparser* p, char** start, char** end )
{
	ppsym*	sym;
	int		id;

	if( ( *start = plex_next( p->lex, *start, &id, end ) ) )
		sym = *((ppsym**)parray_get( &p->tokens, id - 1 ));
	else
		sym = p->gram->eof;

	#if DEBUGLEVEL > 1
	fprintf( stderr, "Next token '%s' @ >%.*s<\n",
				sym->name, *end - *start, *start );
	#endif

	return sym;
}

/** Run parser //p// with input //start//.

The function uses the parsing method defined when then parser was created.
//end// receives a pointer to the position where the parser stopped.

It returns an abstract syntax tree to //root// on success. */
pboolean pp_parser_parse( ppast** root, pparser* p, char* start, char** end )
{
	int			i;
	int			id;
	int			row		= 1;
	int			col		= 1;
	int			lrow;
	int			lcol;
	char*		lstart;
	ppsym*		lhs;
	ppsym*		sym;
	plistel*	e;
	parray*		stack;
	pplrse		item;
	pplrse*		tos;
	int			shift;
	int			reduce;
	ppprod*		prod;
	pboolean	token;
	ppast*		node;

	if( !( p && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Init */
	if( p->lex )
		plex_prepare( p->lex );

	stack = parray_create( sizeof( pplrse ), 0 );

	tos = (pplrse*)parray_malloc( stack );
	tos->start = start;
	tos->end = *end;

	/* Read token */
	*end = start;
	sym = pp_parser_scan( p, &start, end );

	do
	{
		/* Parse */
		#if DEBUGLEVEL > 1
		fprintf( stderr, "State %d\n", tos->state );

		/* AST construction debug */
		/*
		{
			int			i;
			pplrse*		x;


			for( i = 0; i < parray_count( stack ); i++ )
			{
				x = parray_get( stack, i );
				fprintf( stderr, "%d: %p\n", i, x->node );
				if( x->node )
					pp_ast_printnew( x->node );
			}

			fprintf( stderr, "---\n" );
			getchar();
		}
		*/
		#endif
		#if DEBUGLEVEL > 2
		fprintf( stderr, "BEFORE >%s<\n", *end );
		#endif

		/* Check for entries in the parse table */
		if( tos->state > -1 )
		{
			for( i = 2, shift = 0, reduce = 0;
					i < p->dfa[tos->state][0]; i += 3 )
			{
				if( p->dfa[tos->state][i] == sym->idx + 1 )
				{
					if( p->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = p->dfa[tos->state][ i + 2 ];

					if( p->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						reduce = p->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			if( !shift && !reduce )
				reduce = p->dfa[ tos->state ][ 1 ];
		}

		if( !shift && !reduce )
		{
			/* Parse Error */
			/* TODO: Error Recovery */
			fprintf( stderr, "Parse Error [line:%d col:%d] @ >%s<\n",
				row, col, *end );
			return FALSE;
		}

		#if DEBUGLEVEL > 2
		fprintf( stderr, "AFTER  >%s<\n", *end );
		#endif

		/* Shift */
		if( shift )
		{
			#if DEBUGLEVEL > 1
			if( reduce )
				fprintf( stderr,
					"shift on %s and reduce by production %d\n",
						sym->name,
						reduce - 1 );
			else
				fprintf( stderr,
					"shift on %s to state %d\n",
						sym->name,
						shift - 1 );
			#endif

			tos = (pplrse*)parray_malloc( stack );

			tos->state = reduce ? 0 : shift - 1;
			tos->start = start;
			tos->end = *end;
			tos->row = row;
			tos->col = col;

			/* Shifted symbol becomes AST node? */
			if( root && sym->emit )
				tos->node = pp_ast_create( sym->emit, sym,
											(ppprod*)NULL, start, *end,
												row, col, (ppast*)NULL );

			/* Read next token */
			start = *end;
			sym = pp_parser_scan( p, &start, end );
		}

		/* Reduce */
		while( reduce )
		{
			prod = pp_prod_get( p->gram, reduce - 1 );

			#if DEBUGLEVEL > 1
			fprintf( stderr,
				"reduce by production '%s'\n"
				"popping %d items off the stack, replacing by %s\n",
				pp_prod_to_str( prod ),
				plist_count( prod->rhs ),
				prod->lhs->name );
			#endif

			#if DEBUGLEVEL > 2
			print_stack( "Before Reduce", states, stack );
			#endif

			node = (ppast*)NULL;

			for( i = 0; i < plist_count( prod->rhs ); i++ )
			{
				tos = (pplrse*)parray_pop( stack );

				lstart = tos->start;

				/* Connecting nodes, remember last node. */
				if( tos->node )
				{
					if( node )
					{
						while( tos->node->next )
							tos->node = tos->node->next;

						tos->node->next = node;
						node->prev = tos->node;
					}

					node = tos->node;

					while( node->prev )
						node = node->prev;

					lrow = tos->row;
					lcol = tos->col;
				}
			}

			tos = (pplrse*)parray_last( stack );

			/* Construction of AST node */
			if( root && prod->emit )
				node = pp_ast_create( prod->emit,
										prod->lhs, prod, lstart, *end,
											lrow, lcol, node );
			else if( root && prod->lhs->emit )
				node = pp_ast_create( prod->lhs->emit,
										prod->lhs, prod, lstart, *end,
											lrow, lcol, node );

			/* Goal symbol reduced? */
 			if( prod->lhs == p->gram->goal && parray_count( stack ) == 1 )
			{
				if( root )
					*root = node;

				break;
			}

			/* Check for entries in the parse table */
			for( i = p->dfa[tos->state][0] - 3, shift = 0, reduce = 0;
					i >= 2; i -= 3 )
			{
				if( p->dfa[tos->state][i] == prod->lhs->idx + 1 )
				{
					if( p->dfa[ tos->state ][ i + 1 ] & PPLR_SHIFT )
						shift = p->dfa[tos->state][ i + 2 ];

					if( p->dfa[ tos->state ][ i + 1 ] & PPLR_REDUCE )
						reduce = p->dfa[tos->state][ i + 2 ];

					break;
				}
			}

			tos = (pplrse*)parray_malloc( stack );

			tos->state = shift - 1;
			tos->start = lstart;
			tos->end = *end;
			tos->node = node;
			tos->row = lrow;
			tos->col = lcol;

			#if DEBUGLEVEL > 2
			print_stack( "Behind Reduce", states, stack );
			#endif
		}

		#if 0
		/* Skip over whitespace */
		if( !( tos->symbol->flags & PPFLAG_LEXEM ) )
		{
			#if DEBUGLEVEL > 1
			fprintf( stderr, "tos->symbol >%s< is not LEXEM\n",
						tos->symbol->name );
			#endif

			/*
			if( pp_white_in_input( p->gram, ( lstart = *end ), end ) )
				pp_pos_in_input( &row, &col, lstart, *end );
			*/
		}
		#endif
	}
	while( !reduce );

	return TRUE;
}
