/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	prog.c
Usage:	Dynamic virtual machine programs.
----------------------------------------------------------------------------- */

#include "phorward.h"

typedef struct
{
	pboolean	defined;
	pvmaddr		addr;
	pany*		ref;
} pvmlabel;

pvmprog* pvm_prog_create( pvm* vm, char* src )
{
	pvmprog*	prog;

	prog = (pvmprog*)pmalloc( sizeof( pvmprog ) );

	prog->vm = vm;
	parray_init( &prog->prog, sizeof( pvmbyte ), 0 );
	parray_init( &prog->lit, sizeof( pany ), 0 );
	plist_init( &prog->label, sizeof( pvmlabel ), PLIST_DFT_HASHSIZE,
					PLIST_MOD_RECYCLE | PLIST_MOD_UNIQUE );

	if( src && *src )
		pvm_prog_compile( prog, src );

	return prog;
}

pvmprog* pvm_prog_free( pvmprog* prog )
{
	pany*		val;

	if( !prog )
		return (pvmprog*)NULL;

	parray_erase( &prog->prog );

	while( ( val = parray_shift( &prog->lit ) ) )
		pany_reset( val );

	parray_erase( &prog->lit );
	return (pvmprog*)NULL;
}

/* Assemble a prog */
pboolean pvm_prog_compile( pvmprog* prog, char* src )
{
	char*			start		= src;
	char*			end;
	pvmbyte			instr;
	pvmbyte*		ptr;
	pvmaddr			addr;
	pany*			lit;
	unsigned int	tok;
	pvmlabel*		lbl;

	char			buf			[ BUFSIZ + 1 ];
	char*			sbuf;

	enum
	{
		NEXT		= 1,
		LABEL,
		ADDR,
		IDENT,
		INT,
		DBL,
		STR,
		WSTR,
		BOOL
	};

	if( !( prog ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( !prog->vm->lex )
	{
		/* Creating a lexer */
		prog->vm->lex = plex_create( PREGEX_COMP_INSENSITIVE );

		plex_define( prog->vm->lex, "\\n", 						NEXT,	0 );
		plex_define( prog->vm->lex, "[^\"\'\\d\\s]+",			IDENT,	0 );
		plex_define( prog->vm->lex, "%[^'\"\\d\\s]+",			LABEL,	0 );
		plex_define( prog->vm->lex, "#[^'\"\\d\\s]+",			ADDR,	0 );
		plex_define( prog->vm->lex, "\\d+", 					INT,	0 );
		plex_define( prog->vm->lex, "\\d+\\.\\d*|\\d*\\.\\d+",	DBL,	0 );
		plex_define( prog->vm->lex, "'(\\\\'|[^'])*'",			STR,	0 );
		plex_define( prog->vm->lex, "\"(\\\\\"|[^\"])*\"",		WSTR,	0 );
		plex_define( prog->vm->lex, "true|false",				BOOL,	0 );
	}

	while( start && *start )
	{
		lit = (pany*)NULL;
		start = plex_next( prog->vm->lex, start, &tok, &end );

		/* fprintf( stderr, "%d >%.*s<\n", tok, end - start, start ); */
		if( end - start < BUFSIZ )
		{
			sprintf( buf, "%.*s", (int)( end - start ), start );
			sbuf = buf;
		}
		else
			sbuf = pstrndup( start, end - start );

		switch( tok )
		{
			case NEXT:
				break;

			case LABEL:
			case ADDR:
				if( !( lbl = plist_access( plist_get_by_key(
								&prog->label, sbuf + 1 ) ) ) )
				{
					lbl = plist_access(
							plist_insert( &prog->label,
								(plistel*)NULL, sbuf + 1, (void*)NULL ) );
				}

				if( tok == LABEL )
				{
					lbl->defined = TRUE;
					lbl->addr = parray_count( &prog->prog );

					if( lbl->ref )
						pany_set_ulong( lbl->ref, (unsigned long)lbl->addr );
				}
				else
				{
					if( !lbl->ref )
					{
						lbl->ref = (pany*)parray_malloc( &prog->lit );
						pany_set_ptr( lbl->ref, (void*)lbl->addr );
					}

					addr = parray_offset( &prog->lit, lbl->ref );

					/* Reserve one addr + one instruction */
					parray_reserve( &prog->prog, 1 + sizeof( pvmaddr ) );

					/* Write instruction */
					instr = 1;
					ptr = parray_push( &prog->prog, &instr );

					/* Write literal index */
					memcpy( ptr + 1, &addr, sizeof( pvmaddr ) );
					prog->prog.last += sizeof( pvmaddr );
				}
				break;

			case IDENT:
				for( instr = 2; instr < PVM_MAXOPS; instr++ )
				{
					if( !prog->vm->mn[instr] )
						continue;

					/*
					fprintf( stderr, "%s >%.*s< %d\n",
								vm->mn[instr], end - start, start, end-start );
					*/

					if( !pstrncasecmp( prog->vm->mn[instr],
							start, pstrlen( prog->vm->mn[instr] ) ) )
					{
						parray_push( &prog->prog, &instr );
						break;
					}
				}

				/* Have a match?
					Then emit previous command first and remember current. */
				if( instr < PVM_MAXOPS )
					break;
				/* NO break here */

			default:
				lit = pany_create( sbuf );

				addr = parray_offset( &prog->lit,
						parray_push( &prog->lit, lit ) );

				/* Reserve one addr + one instruction */
				parray_reserve( &prog->prog, 1 + sizeof( pvmaddr ) );

				/* Write instruction */
				instr = 1;
				ptr = parray_push( &prog->prog, &instr );

				/* Write literal index */
				memcpy( ptr + 1, &addr, sizeof( pvmaddr ) );
				prog->prog.last += sizeof( pvmaddr );
				break;
		}

		if( sbuf != buf )
			pfree( sbuf );

		start = end;
	}

	return TRUE;
}

/* Dump a prog */
void pvm_prog_dump( pvmprog* prog )
{
	pvmbyte*	ip;
	pvmbyte*	cs;
	pvmbyte*	ecs;
	pvmbyte		instr;

	pvmaddr		addr;
	pany*		any;
	char*		s;

	ip = cs = (pvmbyte*)parray_first( &prog->prog );
	ecs =  cs + parray_count( &prog->prog );

	while( ip < ecs )
	{
		fprintf( stderr, "%08d ", ip - cs );
		instr = *(ip++);

		switch( instr )
		{
			case 0:
				fprintf( stderr, "nop\n" );
				break;

			case 1: /* PUSH */
				addr = *( (pvmaddr*)( ip ) );
				ip += sizeof( pvmaddr );

				any = pany_dup( (pany*)parray_get( &prog->lit, addr ) );

				switch( any->type )
				{
					case PANYTYPE_STR:
					case PANYTYPE_WCS:
						s = pregex_qreplace( "([\\\\\"])",
								pany_get_str( any ), "\\$1", 0 );

						fprintf( stderr, "\"%s\"\n", s );
						break;

					default:
						fprintf( stderr, "%s\n", pany_get_str( any ) );
						break;
				}

				pany_free( any );

				break;

			default: /* anything else */
				if( prog->vm->op[ instr ] )
					fprintf( stderr, "%s\n", prog->vm->mn[ instr ] );

				break;
		}
	}
}

