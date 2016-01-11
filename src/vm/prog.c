/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	prog.c
Usage:	Dynamic virtual machine programs.
----------------------------------------------------------------------------- */

#include "phorward.h"

pvmprog* pvm_prog_create( pvm* vm, char* src )
{
	pvmprog*	prog;

	prog = (pvmprog*)pmalloc( sizeof( pvmprog ) );

	prog->vm = vm;
	parray_init( &prog->prog, sizeof( pvmbyte ), 0 );
	parray_init( &prog->lit, sizeof( pany ), 0 );

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
	char*		start		= src;
	char*		end;
	pvmbyte		instr		= PVM_MAXOPS;
	pvmbyte		n_instr;
	pvmbyte*	ptr;
	pvmaddr		addr;
	pany*		lit;
	pvmbyte		i;
	int			tok;

	char		buf			[ BUFSIZ + 1 ];
	char*		sbuf;

	#define 	NEXT		1
	#define 	LABEL		2
	#define 	IDENT		3
	#define 	INT			4
	#define 	DBL			5
	#define 	STR			6
	#define 	WSTR		7
	#define 	BOOL		8

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
		plex_define( prog->vm->lex, "%[A-Za-z_][A-Za-z_0-9]+",	LABEL,	0 );
		plex_define( prog->vm->lex, "[A-Za-z_][A-Za-z_0-9]+",	IDENT,	0 );
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

		switch( tok )
		{
			case NEXT:
				if( instr < PVM_MAXOPS )
				{
					parray_push( &prog->prog, &instr );
					instr = PVM_MAXOPS;
				}

				break;

			case IDENT:
				for( i = 2, n_instr = PVM_MAXOPS; i < PVM_MAXOPS; i++ )
				{
					if( !prog->vm->mn[i] )
						continue;

					/*
					fprintf( stderr, "%s >%.*s< %d\n",
								vm->mn[i], end - start, start, end-start );
					*/

					if( !pstrncasecmp( prog->vm->mn[i],
							start, pstrlen( prog->vm->mn[i] ) ) )
					{
						n_instr = i;
						break;
					}
				}

				/* Have a match?
					Then emit previous command first and remember current. */
				if( i < PVM_MAXOPS )
				{
					if( instr < PVM_MAXOPS )
						parray_push( &prog->prog, &instr );

					instr = n_instr;
					break;
				}
				/* NO break here */

			default:
				if( end - start < BUFSIZ )
				{
					sprintf( buf, "%.*s", end - start, start );
					sbuf = buf;
				}
				else
					sbuf = pstrndup( start, end - start );

				lit = pany_create( sbuf );

				if( sbuf != buf )
					pfree( sbuf );

				addr = parray_offset( &prog->lit,
						parray_push( &prog->lit, lit ) );

				/* Reserve one addr + one instruction */
				parray_reserve( &prog->prog, 1 + sizeof( pvmaddr ) );

				/* Write instruction */
				n_instr = 1;
				ptr = parray_push( &prog->prog, &n_instr );

				/* Write literal index */
				memcpy( ptr + 1, &addr, sizeof( pvmaddr ) );
				prog->prog.last += sizeof( pvmaddr );
				break;
		}

		start = end;
	}

	if( instr < PVM_MAXOPS )
		parray_push( &prog->prog, &instr );

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
		instr = *(ip++);

		fprintf( stderr, "%08ld ", ip - cs );

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
