/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.parse.c
Usage:	Parse LR/LALR parser
----------------------------------------------------------------------------- */

#include "local.h"

typedef struct
{
	int			state;	/* Current state */
	pgsymbol*	sym;	/* Symbol */
} pglrse;


typedef struct
{
	STACK		st;		/* Stack */

	pgtoken		la;		/* Current look ahead */
}
pglrpcb;

pboolean pg_parser_eval( pgparser* parser, char* input )
{
	pglrpcb		pcb;

	PROC( "pg_parser_eval" );

	if( !pg_parser_is_lr( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	memset( &pcb, 0, sizeof( pglrpcb ) );
	stack_init( &st, sizeof( pglrse ) );

}
