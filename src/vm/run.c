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
	for( any = (pany*)parray_first( stack );
			any <= (pany*)parray_last( stack );
				any++ )
	{
		fprintf( stderr, "%03d => ", any - (pany*)parray_first( stack ) );
		pany_fprint( stderr, any );
	}

}

/** Run vm */
void pvm_prog_run( pvmprog* prog )
{
	pvmexec		rt;
	pvmbyte		instr;
	pvmaddr		addr;
	pany*		any;

	if( !( prog ) )
	{
		WRONGPARAM;
		return;
	}

	memset( &rt, 0, sizeof( pvmexec ) );
	rt.stack = parray_create( sizeof( pany ), 0 );

	rt.cs = rt.ip = (pvmbyte*)parray_first( &prog->prog );
	rt.ecs = rt.cs + parray_count( &prog->prog );

	while( rt.ip < rt.ecs )
	{
		instr = *(rt.ip++);
		fprintf( stderr, "! %d (%s)\n", instr,
							instr == 0 ? "nop" :
								instr == 1 ? "push" :
									prog->vm->mn[ instr ] );

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

	/*
	while( ( any = parray_shift( rt.stack ) ) )
	{
		pany_fprint( stderr, any );
		pany_reset( any );
	}
	*/
}

