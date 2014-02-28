/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	stack.c
Author:	Jan Max Meyer
Usage:	Universal, dynamic stack management functions (on the heap ;))
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Performs a stack initialization.

//stack// is the pointer to stack to be initialized.

//size// defines the size of one stack element, in bytes.
This should be evaluated using the sizeof()-macro.

//step// defines the step size, where a stack-(re)allocation will be performed.
If, e.g. this is set to 128, then, if the 128th item is pushed onto the stack,
a realloction is done. Once allocated memory remains until the stack is freed
again.
*/
pboolean pstack_init( pstack* stack, size_t size, size_t step )
{
	PROC( "pstack_init" );
	PARMS( "stack", "%p", stack );
	PARMS( "size", "%ld", size );
	PARMS( "step", "%ld", step );

	if( !( stack && size > 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( step <= 0 )
		step = 1;

	memset( stack, 0, sizeof( pstack ) );
	stack->size = size;
	stack->step = step;

	RETURN( TRUE );
}

/** Create a new pstack as an object with an element allocation size //size//
and a reallocation-step-size of //step//.

The returned memory must be released with pstack_free().  */
pstack* pstack_create( size_t size, size_t step )
{
	pstack*	stack;

	if( size <= 0 )
	{
		WRONGPARAM;
		return (pstack*)NULL;
	}

	stack = (pstack*)pmalloc( sizeof( pstack ) );
	pstack_init( stack, size, step );

	return stack;
}

/** Erase a dynamic stack.
The stack must not be reinitalized after destruction.

//stack// is the stack to be erased. */
pboolean pstack_erase( pstack* stack )
{
	PROC( "pstack_free" );
	PARMS( "stack", "%p", stack );

	if( !stack )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	stack->stack = pfree( stack->stack );
	stack->top = 0;
	stack->count = 0;

	RETURN( TRUE );
}

/** Releases all the memory //stack// uses and destroys the stack object.

The function always returns (plist*)NULL. */
pstack* pstack_free( pstack* stack )
{
	if( !stack )
		return (pstack*)NULL;

	pstack_erase( stack );
	pfree( stack );

	return (pstack*)NULL;
}

/** Pushes an element onto the stack.

The element's memory is copied during the push. The item must be of the same
memory size as used at stack initialization.

//stack// is the pointer to stack where to push an item on.

//item// is the pointer to the memory of the item that should be pushed onto the
stack. The caller should cast his type into void, or wrap the push-operation
with a macro.

The function returns the address of the newly pushed item, and (void*)NULL if
the item could not be pushed.
*/
void* pstack_push( pstack* stack, void* item )
{
	PROC( "pstack_push" );
	PARMS( "stack", "%p", stack );
	PARMS( "item", "%p", item );

	if( !( stack ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	/* Is memory (re-)allocation required? */
	VARS( "stack->count", "%d", stack->count );
	if( stack->count == 0 )
	{
		MSG( "Performing first allocation" );
		VARS( "Allocating bytes", "%d", stack->step * stack->size );
		if( !( stack->stack = (void*)pmalloc( stack->step * stack->size ) ) )
		{
			MSG( "Memory failure - Can't allocate..." );
			RETURN( (void*)NULL );
		}

		stack->count = stack->step;
		stack->top = 0;
	}
	else if( stack->top + 1 == stack->count )
	{
		MSG( "A stack expansion is required" );

		stack->count += stack->step;

		VARS( "Re-allocating bytes", "%d", stack->step * stack->size );
		if( !( stack->stack = (void*)prealloc( (void*)stack->stack,
					stack->count * stack->size ) ) )
		{
			MSG( "Memory failure - Can't reallocate..." );
			RETURN( (void*)NULL );
		}
	}

	/* Copy item into top of stack */
	if( item )
	{
		memcpy( (char*)stack->stack + stack->top * stack->size,
					item, stack->size );
	}

	RETURN( (char*)stack->stack + stack->top++ * stack->size );
}

/** Pops an element off the stack.

The function returns a pointer to the popped item. Because dynamic stacks only
grows and no memory is freed, the returned data pointer is still valid, and will
only be overwritten with the next push operation.

//stack// is the pointer to stack where to pop an item off.

The function returns the address of the popped item, and (void*)NULL if the
item could not be popped (e.g. stack is empty).
*/
void* pstack_pop( pstack* stack )
{
	PROC( "pstack_pop" );
	PARMS( "stack", "%p", stack );

	if( !stack )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( stack->top == 0 )
	{
		MSG( "top is zero, no items on the stack." );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)stack->stack + ( --stack->top * stack->size ) );
}

/** Access an element from the stack via its offset position.

//stack// is the pointer to stack where to access the element from.
//offset// is the offset of the element to be accessed from the stack's
base address.

Returns the address of the accessed item, and (void*)NULL if the item could not
be accessed (e.g. if the stack is empty or offset is beyond the top of stack).
*/
void* pstack_access( pstack* stack, size_t offset )
{
	PROC( "pstack_access" );
	PARMS( "stack", "%p", stack );
	PARMS( "offset", "%d", offset );

	if( !stack )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( stack->top == 0 || offset >= stack->top )
	{
		MSG( "offset defines an item that is out of bounds within stack" );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)stack->stack + offset * stack->size );
}

/** Access top element of the stack.

Returns the address of the accessed item, and (void*)NULL if nothing is on
the stack.
*/
void* pstack_top( pstack* stack )
{
	PROC( "pstack_top" );
	PARMS( "stack", "%p", stack );

	if( !stack )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( stack->count == 0 )
		RETURN( (void*)NULL );

	RETURN( pstack_access( stack, stack->top - 1 ) );
}

/** Access bottom element of the stack.

Returns the address of the accessed item, and (void*)NULL if nothing is on
the stack.
*/
void* pstack_bottom( pstack* stack )
{
	PROC( "pstack_bottom" );
	PARMS( "stack", "%p", stack );

	if( !stack )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( stack->count == 0 )
		RETURN( (void*)NULL );

	RETURN( (char*)stack->stack );
}


/** Returns the number of elements in a stack. */
int pstack_count( pstack* stack )
{
	if( !stack )
		return 0;

	return stack->top;
}

