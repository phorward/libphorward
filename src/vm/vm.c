/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	vm.c
Usage:	Dynamic virtual machine definition.
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Basic VM instructions */
static void pvm_DIM( pvmexec* rt )
{
	pany*		amt;
	int			i;

	amt = (pany*)parray_pop( rt->stack );

	for( i = pany_get_int( amt ); i > 0; i-- )
		parray_malloc( rt->stack );
}

static void pvm_DRP( pvmexec* rt )
{
	pany*		amt;
	int			i;

	amt = (pany*)parray_pop( rt->stack );

	for( i = pany_get_int( amt ); i > 0; i-- )
		parray_pop( rt->stack );
}

static void pvm_JMP( pvmexec* rt )
{
	rt->ip = rt->cs + *((pvmaddr*)pany_get_ptr(
							(pany*)parray_pop( rt->stack ) ) );
}

static void pvm_JPC( pvmexec* rt )
{
	pany*		addr;

	addr = (pany*)parray_pop( rt->stack );

	if( pany_get_bool( (pany*)parray_pop( rt->stack ) ) )
		rt->ip = (pvmbyte*)pany_get_ptr( addr );
}

static void pvm_BOOL( pvmexec* rt )
{
	pany_convert( (pany*)parray_last( rt->stack ), PANYTYPE_BOOL );
}

static void pvm_INT( pvmexec* rt )
{
	pany_convert( (pany*)parray_last( rt->stack ), PANYTYPE_LONG );
}

static void pvm_ADDR( pvmexec* rt )
{
	pany_to_ptr( (pany*)parray_last( rt->stack ) );
}

static void pvm_FLOAT( pvmexec* rt )
{
	pany_to_double( (pany*)parray_last( rt->stack ) );
}

static void pvm_STR( pvmexec* rt )
{
	pany_to_str( (pany*)parray_last( rt->stack ) );
}

static void pvm_WSTR( pvmexec* rt )
{
	pany_to_wcs( (pany*)parray_last( rt->stack ) );
}

static pany* pvm_compute( pany* ret, char op,
							pany* op1, pany* op2,
								panytype enforce )
{
	int				c;
	char*			s;
	wchar_t*		ws;

	if( !( strchr( "+-*/", op ) && ret && op1 && op2 ) )
	{
		/* Invalid operands */
		if( ret )
			pany_reset( ret );

		return ret;
	}

	if( ( enforce == PANYTYPE_STR
			&& strchr( "+*", op ) )
		|| ( op == '+'
				&& ( op1->type == PANYTYPE_STR
					|| op2->type == PANYTYPE_STR ) )
		|| ( op == '*'
				&& op1->type == PANYTYPE_STR ) )
	{
		if( op == '+' )
		{
			s = pstrcatstr( pany_get_str( op1 ), pany_get_str( op2 ), FALSE );

			if( ret == op1 )
				ret->val.s = s;
			else
				pany_set_str( ret, s );
		}
		else
		{
			s = (char*)pmalloc( ( pstrlen( pany_get_str( op1 ) )
									* pany_to_int( op2 ) + 1 )
										* sizeof( char ) );

			for( c = pany_to_int( op2 ); c >= 0; c-- )
				strcat( s, pany_get_str( op1 ) );

			pany_reset( ret );
			pany_set_str( ret, s );
		}
	}
	else if( ( enforce == PANYTYPE_WCS
			&& strchr( "+*", op ) )
		|| ( op == '+'
				&& ( op1->type == PANYTYPE_WCS
					|| op2->type == PANYTYPE_WCS ) )
		|| ( op == '*'
				&& op1->type == PANYTYPE_WCS ) )
	{
		if( op == '+' )
		{
			ws = pwcscatstr( pany_get_wcs( op1 ), pany_get_wcs( op2 ), FALSE );

			if( ret == op1 )
				ret->val.ws = ws;
			else
				pany_set_wcs( ret, ws );
		}
		else
		{
			ws = (wchar_t*)pmalloc( ( pwcslen( pany_get_wcs( op1 ) )
										* pany_to_int( op2 ) + 1 )
											* sizeof( char ) );

			for( c = pany_to_int( op2 ); c >= 0; c-- )
				wcscat( ws, pany_get_wcs( op1 ) );

			pany_reset( ret );
			pany_set_wcs( ret, ws );
		}
	}
	/* Prefer for double type */
	else if( enforce == PANYTYPE_FLOAT
				|| enforce == PANYTYPE_DOUBLE
					|| op1->type == PANYTYPE_DOUBLE
						|| op2->type == PANYTYPE_DOUBLE
							|| op1->type == PANYTYPE_FLOAT
								|| op2->type == PANYTYPE_FLOAT )
	{
		switch( op )
		{
			case '+':
				pany_set_double( ret,
					pany_get_double( op1 ) + pany_get_double( op2 ) );
				break;

			case '-':
				pany_set_double( ret,
					pany_get_double( op1 ) - pany_get_double( op2 ) );
				break;

			case '*':
				pany_set_double( ret,
					pany_get_double( op1 ) * pany_get_double( op2 ) );
				break;

			case '/':
				pany_set_double( ret,
					pany_get_double( op1 ) / pany_get_double( op2 ) );
				break;

			default:
				MISSINGCASE;
		}
	}
	/* Prefer for pointer type */
	/*
	else if( ( enforce == PANYTYPE_PTR
				|| op1->type == PANYTYPE_PTR
					|| op2->type == PANYTYPE_PTR )
						&& strchr( "+-", ch ) )
	{
		switch( op )
		{
			case '+':
				pany_set_ptr( ret, pany_get_ptr( op1 ) + pany_get_ptr( op2 ) );
				break;

			case '-':
				pany_set_ptr( ret, pany_get_ptr( op1 ) - pany_get_ptr( op2 ) );
				break;

			default:
				MISSINGCASE;
		}
	}
	*/
	/* Enforce long integer type */
	else
	{
		switch( op )
		{
			case '+':
				pany_set_long( ret,
					pany_get_long( op1 ) + pany_get_long( op2 ) );
				break;

			case '-':
				pany_set_long( ret,
					pany_get_long( op1 ) - pany_get_long( op2 ) );
				break;

			case '*':
				pany_set_long( ret,
					pany_get_long( op1 ) * pany_get_long( op2 ) );
				break;

			case '/':
				pany_set_long( ret,
					pany_get_long( op1 ) / pany_get_long( op2 ) );
				break;

			default:
				MISSINGCASE;
		}
	}

	return ret;
}

static void pvm_ADD( pvmexec* rt )
{
	pany*		ops	[2];

	ops[1] = (pany*)parray_pop( rt->stack );
	ops[0] = (pany*)parray_pop( rt->stack );

	pvm_compute( ops[0], '+', ops[0], ops[1], PANYTYPE_NULL );
	parray_push( rt->stack, (void*)NULL );
}

static void pvm_SUB( pvmexec* rt )
{
	pany*		ops	[2];

	ops[1] = (pany*)parray_pop( rt->stack );
	ops[0] = (pany*)parray_pop( rt->stack );

	pvm_compute( ops[0], '-', ops[0], ops[1], PANYTYPE_NULL );
	parray_push( rt->stack, (void*)NULL );
}

static void pvm_MUL( pvmexec* rt )
{
	pany*		ops	[2];

	ops[1] = (pany*)parray_pop( rt->stack );
	ops[0] = (pany*)parray_pop( rt->stack );

	pvm_compute( ops[0], '*', ops[0], ops[1], PANYTYPE_NULL );
	parray_push( rt->stack, (void*)NULL );
}

static void pvm_DIV( pvmexec* rt )
{
	pany*		ops	[2];

	ops[1] = (pany*)parray_pop( rt->stack );
	ops[0] = (pany*)parray_pop( rt->stack );

	pvm_compute( ops[0], '/', ops[0], ops[1], PANYTYPE_NULL );
	parray_push( rt->stack, (void*)NULL );
}

static void pvm_JOIN( pvmexec* rt )
{
	pany*		ops		[2];
	panytype	prefer	= PANYTYPE_STR;

	ops[1] = (pany*)parray_pop( rt->stack );
	ops[0] = (pany*)parray_pop( rt->stack );

	if( ops[0]->type == PANYTYPE_WCS
		|| ops[1]->type == PANYTYPE_WCS )
		prefer = PANYTYPE_WCS;

	pany_convert( ops[0], prefer );
	pany_convert( ops[1], prefer );

	pvm_compute( ops[0], '+', ops[0], ops[1], prefer );
	parray_push( rt->stack, (void*)NULL );
}

/** Initializes the virtual machine //vm//. */
pvm* pvm_init( pvm* vm )
{
	if( !vm )
	{
		WRONGPARAM;
		return (pvm*)NULL;
	}

	memset( vm, 0, sizeof( pvm ) );

	/* Implement default instruction set */
	pvm_define( vm, "dim", pvm_DIM );
	pvm_define( vm, "drp", pvm_DRP );
	pvm_define( vm, "jmp", pvm_JMP );
	pvm_define( vm, "jpc", pvm_JPC );
	pvm_define( vm, "bool", pvm_BOOL );
	pvm_define( vm, "int", pvm_INT );
	pvm_define( vm, "float", pvm_FLOAT );
	pvm_define( vm, "addr", pvm_ADDR );
	pvm_define( vm, "str", pvm_STR );
	pvm_define( vm, "wstr", pvm_WSTR );

	pvm_define( vm, "add", pvm_ADD );
	pvm_define( vm, "sub", pvm_SUB );
	pvm_define( vm, "mul", pvm_MUL );
	pvm_define( vm, "div", pvm_DIV );

	pvm_define( vm, "join", pvm_JOIN );

	return vm;
}

/** Creates a new virtual machine. */
pvm* pvm_create( void )
{
	pvm*	vm;

	vm = (pvm*)pmalloc( sizeof( pvm ) );
	pvm_init( vm );

	return pvm_init( vm );
}

/** Resets a virtual machine //vm//. */
pvm* pvm_reset( pvm* vm )
{
	int		i;

	if( !vm )
	{
		WRONGPARAM;
		return (pvm*)NULL;
	}

	for( i = 0; i < PVM_MAXOPS; i++ )
	{
		vm->op[i] = (pvmop)NULL;
		vm->mn[i] = pfree( vm->mn[i] );
	}

	vm->lex = plex_free( vm->lex );

	return vm;
}

/** Frees a virtual machine. */
pvm* pvm_free( pvm* vm )
{
	if( !vm )
		return (pvm*)NULL;

	return (pvm*)pfree( pvm_reset( vm ) );
}

/** Implements mnemonic //mn// with operational function //op// into //vm//.

Returns the operation's opcode, or a value < 0 on error.
*/
int pvm_define( pvm* vm, char* mn, pvmop op )
{
	int		i;

	if( !( mn && *mn ) )
	{
		WRONGPARAM;
		return -1;
	}

	for( i = 2; i < PVM_MAXOPS; i++ )
	{
		if( !vm->mn[i] )
		{
			vm->op[i] = op;
			pstrput( &(vm->mn[i]), mn );
			return i;
		}
	}

	return -1;
}

