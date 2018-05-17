/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	run.c
Usage:	Dynamic virtual machine execution.
----------------------------------------------------------------------------- */

#include "phorward.h"

static void dump_stack( parray* stack )
{
	pany*	any;

	PROC( "dump_stack" );

	for( any = (pany*)parray_first( stack );
			any <= (pany*)parray_last( stack );
				any++ )
		PANY_DUMP( any );

	VOIDRET;
}

/** Run vm */
void pvm_prog_run( pany** ret, pvmprog* prog )
{
	parray		stack;
	pvmexec		rt;
	pvmbyte		instr;
	pvmaddr		addr;
	pany*		any;

	PROC( "pvm_prog_run" );

	if( !( prog ) )
	{
		WRONGPARAM;
		VOIDRET;
	}

	parray_init( &stack, sizeof( pany ), 0 );

	memset( &rt, 0, sizeof( pvmexec ) );
	rt.stack = &stack;
	rt.cs = rt.ip = (pvmbyte*)parray_first( &prog->prog );
	rt.ecs = rt.cs + parray_count( &prog->prog );

	while( rt.ip < rt.ecs )
	{
		instr = *(rt.ip++);

		/*
		fprintf( stderr, "! %d (%s)\n", instr,
							instr == 0 ? "nop" :
								instr == 1 ? "push" :
									prog->vm->mn[ instr ] );
		*/

		switch( instr )
		{
			case 0:
				break;

			case 1: /* PUSH */
				addr = *( (pvmaddr*)( rt.ip ) );
				rt.ip += sizeof( pvmaddr );

				any = (pany*)parray_push( rt.stack, (void*)NULL );
				pany_copy( any, (pany*)parray_get( &prog->lit, addr ) );
				break;

			default: /* anything else */
				if( prog->vm->op[ instr ] )
					(*(prog->vm->op[ instr ]))( &rt );

				break;
		}

		dump_stack( rt.stack );
		/* getchar(); */
	}


	while( parray_count( rt.stack ) > 1
			&& ( any = parray_shift( rt.stack ) ) )
		pany_reset( any );

	if( ret )
	{
		memcpy( stack.array, parray_first( rt.stack ), sizeof( pany ) );
		*ret = stack.array;
	}
	else
		parray_erase( &stack );

	VOIDRET;
}

