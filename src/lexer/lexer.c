/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lexer.c
Usage:
----------------------------------------------------------------------------- */

#include "phorward.h"

pglexer* pg_lexer_create( pgparser* parser )
{
	pregex_nfa*		nfa;
	pregex_dfa*		dfa;
	pregex_ptn*		p;
	pglexer*		l;
	pgterminal*		t;
	int				i;

	PROC( "pg_lexer_create" );
	PARMS( "parser", "%p", parser );

	if( !( parser ) )
	{
		WRONGPARAM;
		RETURN( (pglexer*)NULL );
	}

	l = (pglexer*)pmalloc( sizeof( pglexer ) );
	l->grammar = pg_parser_get_grammar( parser );
	l->flags = PG_LEXMOD_UTF8;

	pg_lexer_set_source( l, PG_LEX_SRCTYPE_FUNC, getchar );

	MSG( "Turning terminal symbols into lexer symbols" );
	nfa = pregex_nfa_create();

	for( i = 0; ( t = pg_terminal_get( l->grammar, i ) ); i++ )
	{
		VARS( "t", "%s", pg_symbol_get_name( t ) );

		if( ( p = pg_terminal_get_pattern( t ) ) )
		{
			p->accept->accept = pg_symbol_get_id( t ) + 1;

			if( pregex_ptn_to_nfa( nfa, p ) != ERR_OK )
			{
				MSG( "Cannot integrate symbol" );

				pg_lexer_free( l );
				RETURN( (pglexer*)NULL );
			}
		}
	}

	MSG( "Setting whitespace" );
	if( ( p = pg_grammar_get_whitespace( l->grammar ) ) )
	{
		MSG( "Whitespace defined by grammar" );

		p->accept->accept = 0;

		if( pregex_ptn_to_nfa( nfa, p ) != ERR_OK )
		{
			MSG( "Cannot integrate whitespace" );

			pg_lexer_free( l );
			RETURN( (pglexer*)NULL );
		}
	}
	else
	{
		MSG( "Any other input is whitespace" );
		l->flags |= PG_LEXMOD_SKIP_UNKNOWN;
	}

	MSG( "Construct a DFA from NFA" );
	dfa = pregex_dfa_create();

	if( pregex_dfa_from_nfa( dfa, nfa ) <= 0
			|| pregex_dfa_minimize( dfa ) <= 0 )
	{
		MSG( "Subset construction error" );

		pregex_nfa_free( nfa );
		pg_lexer_free( l );
		RETURN( (pglexer*)NULL );
	}

	pregex_nfa_free( nfa );

	MSG( "Convert DFA tables into lexer" );
	pregex_dfa_to_matrix( (pchar***)NULL, dfa );
	l->states_cnt = pregex_dfa_to_matrix( &l->states, dfa );
	pregex_dfa_free( dfa );

	MSG( "Reset lexer" );
	pg_lexer_reset( l );

	MSG( "Lexer created" );
	RETURN( l );
}

pboolean pg_lexer_reset( pglexer* lex )
{
	PROC( "pg_lexer_reset" );
	PARMS( "lex", "%p", lex );

	if( !( lex ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( lex->source != PG_LEX_SRCTYPE_STRING )
		lex->bufbeg = lex->bufend;

	if( lex->flags & PG_LEXMOD_UTF8 || lex->flags & PG_LEXMOD_WCHAR )
		lex->chsize = sizeof( wchar );
	else
		lex->chsize = sizeof( char );

	lex->line = 1;
	lex->column = 1;

	RETURN( TRUE );
}

pglexer* pg_lexer_free( pglexer* lex )
{
	if( !lex )
		return (pglexer*)NULL;

	pg_lexer_reset( lex );

	if( lex->bufsiz > 0 )
		lex->bufbeg = lex->bufend = pfree( lex->bufbeg );

	pfree( lex );

	return (pglexer*)NULL;
}

pboolean pg_lexer_set_source( pglexer* lex, int type, void* ptr )
{
	if( !( lex && ptr ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	switch( type )
	{
		case PG_LEX_SRCTYPE_FUNC:
			lex->src.func = (unsigned int (*)(void))ptr;
			break;

		case PG_LEX_SRCTYPE_STRING:
			lex->src.str = (char*)ptr;
			break;

		case PG_LEX_SRCTYPE_STREAM:
			lex->src.stream = (FILE*)ptr;
			break;


		default:
			WRONGPARAM;
			return FALSE;
	}

	pg_lexer_reset( lex );

	/* In String mode, the buffer is the string */
	if( ( lex->source = type ) == PG_LEX_SRCTYPE_STRING )
	{
		if( lex->flags & PG_LEXMOD_WCHAR )
		{
			lex->bufbeg = (pchar*)lex->src.str;
			lex->bufend = (pchar*)( (pchar*)lex->src.str
									+ wcslen( (pchar*)lex->src.str ) );
		}
		else
		{
			lex->bufbeg = (pchar*)lex->src.str;
			lex->bufend = (pchar*)( lex->src.str + strlen( lex->src.str ) );
		}

		lex->eof = 0;
	}
	else
		lex->eof = EOF;

	return TRUE;
}

/* Read next character from input. */
static pchar pg_lexer_getchar( pglexer* lex )
{
	pchar	ch		= 0;
	pchar	cch;
	int		ubytes	= 0;
	int		ucount	= 0;

	/* UTF-8 skirmish... */
	static int offsets_utf8[ 6 ] =
	{
		0x00000000UL, 0x00003080UL, 0x000E2080UL,
		0x03C82080UL, 0xFA082080UL, 0x82082080UL
	};

	static int trailbyte_utf8[ 256 ] =
	{
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
	};

	if( !lex )
	{
		WRONGPARAM;
		return 0;
	}

	do
	{
		switch( lex->source )
		{
			case PG_LEX_SRCTYPE_FUNC:
				cch = (pchar)(*(lex->src.func))();
				break;

			case PG_LEX_SRCTYPE_STRING:
				if( lex->flags & PG_LEXMOD_WCHAR )
				{
					cch = (pchar) *( (wchar*)( lex->src.str ) );
					lex->src.str += sizeof( wchar );
				}
				else
					cch = (pchar)( *( lex->src.str++ ) );

				break;

			case PG_LEX_SRCTYPE_STREAM:
				if( lex->flags & PG_LEXMOD_WCHAR )
				{
					if( fread( &ch, sizeof( wchar ), 1, lex->src.stream )
							!= sizeof( wchar ) )
						cch = lex->eof;
				}
				else
					cch = (pchar)fgetc( lex->src.stream );

				break;

			default:
				MISSINGCASE;
				break;
		}


		if( cch == lex->eof )
		{
			lex->is_eof = TRUE;
			ch = cch;

			return cch;
		}

		if( !( lex->flags & PG_LEXMOD_WCHAR ) && lex->flags & PG_LEXMOD_UTF8 )
		{
			if( ucount == 0 )
				ubytes = ucount = trailbyte_utf8[ cch ];
			else
				ucount--;

			ch += cch;

			if( ucount )
				ch <<= 6;
		}
		else
			ch = cch;
	}
	while( ucount );

	return ch - offsets_utf8[ ubytes ];
}

/* Get character from buffer */
static pchar pg_lexer_getinput( pglexer* lex, size_t offset )
{
	char*		ptr;
	size_t		count;

	if( !lex )
	{
		WRONGPARAM;
		return 0;
	}

	if( lex->source == PG_LEX_SRCTYPE_STRING )
	{
		if( lex->flags & PG_LEXMOD_WCHAR )
		{
			if( lex->bufbeg + offset >= lex->bufend )
			{
				lex->is_eof = TRUE;
				return *( lex->bufend );
			}

			return lex->bufbeg[ offset ];
		}
		else if( !( lex->flags & PG_LEXMOD_UTF8 ) )
		{
			if( (char*)lex->bufbeg + offset >= (char*)lex->bufend )
			{
				lex->is_eof = TRUE;
				return *( (char*)lex->bufend );
			}

			return ( (char*)lex->bufbeg )[ offset ];
		}

		for( ptr = (char*)lex->bufbeg; offset && *ptr;
				ptr += u8_seqlen( ptr ), offset-- )
			;

		/*
			NOTE: What if eof is a unicode-char? ...
			Then for-loop and if below should be something like this:

				for( ...; u8_char( ptr ) == lex->eof; ... )
		*/

		if( offset || !*ptr )
		{
			lex->is_eof = TRUE;
			return lex->eof;
		}

		return u8_char( ptr );
	}

	while( lex->bufbeg + offset >= lex->bufend )
	{
		/* Is buffer reallocation required? */
		if( !lex->bufbeg )
		{
			lex->bufbeg = lex->bufend = (pchar*)pmalloc(
											PLEX_BUFSTEP * sizeof( pchar ) );
			lex->bufsiz = PLEX_BUFSTEP;
		}
		else if( ( count = ( lex->bufend - lex->bufbeg ) ) > lex->bufsiz )
		{
			lex->bufbeg = (pchar*)prealloc( lex->bufbeg,
									( lex->bufsiz + PLEX_BUFSTEP )
										* sizeof( pchar ) );

			lex->bufend = lex->bufbeg + count;
			lex->bufsiz += PLEX_BUFSTEP;
		}


		if( lex->is_eof || ( *( lex->bufend ) = pg_lexer_getchar( lex ) )
								== lex->eof )
		{
			lex->is_eof = TRUE;
			return lex->eof;
		}

		*( ++lex->bufend ) = 0;
	}

	return lex->bufbeg[ offset ];
}

static void pg_lexer_clearinput( pglexer* lex, size_t len )
{
	char*	ptr;

	if( !lex )
	{
		WRONGPARAM;
		return;
	}

	if( len == 0 )
		return;

	if( lex->source == PG_LEX_SRCTYPE_STRING )
	{
		if( lex->flags & PG_LEXMOD_WCHAR )
		{
			for( ; *lex->bufbeg && len; lex->bufbeg++, len-- )
			{
				if( *lex->bufbeg == '\n' )
				{
					lex->line++;
					lex->column = 1;
				}
				else
					lex->column++;
			}
		}
		else
		{
			for( ptr = (char*)lex->bufbeg; len && *ptr; len-- )
			{
				if( *ptr == '\n' )
				{
					lex->line++;
					lex->column = 1;
				}
				else
					lex->column++;

				if( lex->flags & PG_LEXMOD_UTF8 )
					ptr += u8_seqlen( ptr );
				else
					ptr++;
			}

			lex->bufbeg = (pchar*)ptr;
		}
	}
	else if( len > 0 )
	{
		memmove( lex->bufbeg, lex->bufbeg + len,
					( ( lex->bufend - ( lex->bufbeg + len ) ) + 1 + 1 )
						* sizeof( pchar ) );

		lex->bufend = lex->bufbeg + ( lex->bufend - ( lex->bufbeg + len ) );
	}
	else
	{
		lex->bufend = lex->bufbeg;
		*( lex->bufbeg ) = 0;
	}
}

char* pg_lexer_isolate( pglexer* lex )
{
	char*	ret;

	if( !( lex ) )
	{
		WRONGPARAM;
		return (char*)NULL;
	}

	if( lex->source == PG_LEX_SRCTYPE_STRING
	 		&& !( lex->flags & PG_LEXMOD_WCHAR ) )
		ret = pstrndup( (char*)lex->bufbeg, lex->len );
	else
	{
		ret = (char*)pmalloc( ( lex->len + 1 ) * sizeof( pchar ) );
		memcpy( ret, lex->bufbeg, lex->len * sizeof( pchar ) );
	}

	return ret;
}

pgtoken* pg_lexer_fetch( pglexer* lex )
{
	int			i;
	int			state;
	int			trans;
	int			accept;
	pchar		ch;
	size_t		len;
	pgsymbol*	sym;

	PROC( "pg_lexer_fetch" );
	PARMS( "lex", "%p", lex );

	if( !( lex ) )
	{
		WRONGPARAM;
		RETURN( (pgtoken*)NULL );
	}

	pg_lexer_clearinput( lex, lex->len );

	do
	{
		lex->len = 0;
		len = 0;

		state = 0;
		accept = -1;

		do
		{
			trans = -1;
			ch = pg_lexer_getinput( lex, len++ );

			VARS( "ch", "%d", ch );
			VARS( "state", "%d", state );

			for( i = 3; i < lex->states[ state ][0]; i += 3 )
			{
				if( ch >= lex->states[ state ][i]
						&& ch <= lex->states[ state ][i+1] )
				{
					 trans = lex->states[ state ][i+2];
					 break;
				}
			}

			if( lex->states[ state ][1] >= 0 )
			{
				accept = lex->states[ state ][1];
				lex->len = len - 1;
			}

			if( trans < 0 )
				state = lex->states[ state ][2];
			else
				state = trans;
		}
		while( state >= 0 );

		/* TODO: Is this allowed?  PG_LEXMOD_SKIP_UNKNOWN */
		if( accept < 0 )
		{
			if( lex->is_eof )
			{
				MSG( "EOF reached" );
				fprintf( stderr, "EOF read\n" );
				RETURN( (pgtoken*)NULL );
			}

			pg_lexer_clearinput( lex, 1 );
		}
	}
	while( accept < 0 );

	MSG( "Token fetched" );
	VARS( "token", "%d", accept );
	VARS( "len", "%d", lex->len );

	if( lex->source == PG_LEX_SRCTYPE_STRING
	 		&& !( lex->flags & PG_LEXMOD_WCHAR ) )
		fprintf( stderr, "Token %d len %d lexem >%.*s<\n",
			accept, lex->len, lex->len, (char*)lex->bufbeg );
	else
		fprintf( stderr, "Token %d len %d lexem >%.*ls<\n",
			accept, lex->len, lex->len, lex->bufbeg );

	sym = pg_symbol_get_by_id( lex->grammar, accept - 1 );

	lex->token = pg_token_create( sym, (char*)NULL );

	if( lex->source == PG_LEX_SRCTYPE_STRING
	 		&& !( lex->flags & PG_LEXMOD_WCHAR ) )
		pg_token_set_lexem( lex->token, pg_lexer_isolate( lex ) );
	else
		pg_token_set_wlexem( lex->token, (pchar*)pg_lexer_isolate( lex ) );

	RETURN( lex->token );
}
