/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	stack.c
Author:	Jan Max Meyer
Usage:	Universal, dynamic stack management functions
----------------------------------------------------------------------------- */

#include <phorward.h>

/** Performs a stack initialization.

//stack// is the pointer to stack to be initialized.

//size// defines the size of one stack element, in bytes.
This should be evaluated using the sizeof()-macro.

//step// defines the step size, where a stack-(re)allocation will be performed.
If, e.g. this is set to 128, then, if the 128th item is pushed onto the stack,
a realloction is done. Once allocated memory remains until the stack is freed
again.
*/
void stack_init( STACK* stack, psize size, psize step )
{
	PROC( "stack_init" );
	PARMS( "stack", "%p", stack );
	PARMS( "size", "%d", size );
	PARMS( "step", "%d", step );

	memset( stack, 0, sizeof( STACK ) );
	stack->size = size;
	stack->step = step;

	VOIDRET;
}

/** Frees the entire stack.
The stack must not be reinitalized after destruction.

//stack// is the stack to be freed.
The function //void (*ff)( pbyte*)// is a pointer to callback-function that is
used to free one element. The function retrieves the pointer of the element.
*/
void stack_free( STACK* stack, void (*ff)( pbyte* ) )
{
	psize	i;

	PROC( "stack_free" );
	PARMS( "stack", "%p", stack );
	PARMS( "ff", "%p", ff );

	if( ff )
	{
		MSG( "Freeing each element" );
		for( i = 0; i < stack->top; i++ )
		{
			VARS( "i", "%d", i );
			(*ff)( stack_access( stack, i ) );
		}
	}

	pfree( stack->stack );

	stack->stack = (pbyte*)NULL;
	stack->top = 0;
	stack->count = 0;

	VOIDRET;
}

/** Pushes an element onto the stack.

The element's memory is copied during the push. The item must be of the same
memory size as used at stack initialization.

//stack// is the pointer to stack where to push an item on.

//item// is the pointer to the memory of the item that should be pushed onto the
stack. The caller should cast his type into pbyte, or wrap the push-operation
with a macro.

The function returns the address of the newly pushed item, and (pbyte*)NULL if
the item could not be pushed.
*/
pbyte* stack_push( STACK* stack, pbyte* item )
{
	PROC( "stack_push" );
	PARMS( "stack", "%p", stack );
	PARMS( "item", "%p", item );

	/* Is memory (re-)allocation required? */
	VARS( "stack->count", "%d", stack->count );
	if( stack->count == 0 )
	{
		MSG( "Performing first allocation" );
		VARS( "Allocating bytes", "%d", stack->step * stack->size );
		if( !( stack->stack = (pbyte*)pmalloc( stack->step * stack->size ) ) )
		{
			MSG( "Memory failure - Can't allocate..." );
			return (pbyte*)NULL;
		}

		stack->count = stack->step;
		stack->top = 0;
	}
	else if( stack->top + 1 == stack->count )
	{
		MSG( "A stack expansion is required" );

		stack->count += stack->step;

		VARS( "Re-allocating bytes", "%d", stack->step * stack->size );
		if( !( stack->stack = (pbyte*)prealloc( (void*)stack->stack,
					stack->count * stack->size ) ) )
		{
			MSG( "Memory failure - Can't reallocate..." );
			return (pbyte*)NULL;
		}
	}

	/* Copy item into top of stack */
	memcpy( stack->stack + stack->top * stack->size, item, stack->size );
	return stack->stack + stack->top++ * stack->size;
}

/** Pops an element off the stack.

The function returns a pointer to the popped item. Because dynamic stacks only
grows and no memory is freed, the returned data pointer is still valid, and will
only be overwritten with the next push operation.

//stack// is the pointer to stack where to pop an item off.

The function returns the address of the popped item, and (pbyte*)NULL if the
item could not be popped (e.g. stack is empty).
*/
pbyte* stack_pop( STACK* stack )
{
	PROC( "stack_pop" );
	PARMS( "stack", "%p", stack );

	if( stack->top == 0 )
	{
		MSG( "top is zero, no items on the stack." );
		RETURN( (pbyte*)NULL );
	}

	RETURN( stack->stack + --stack->top * stack->size );
}

/** Accesses an element from the stack via its offset position.

//stack// is the pointer to stack where to access the element from.
//offset// is the offset of the element to be accessed from the stack's
base address.

Returns the address of the accessed item, and (pbyte*)NULL if the item could not
be accessed (e.g. if the stack is empty or offset is beyond the top of stack).
*/
pbyte* stack_access( STACK* stack, psize offset )
{
	PROC( "stack_pop" );
	PARMS( "stack", "%p", stack );
	PARMS( "offset", "%d", offset );

	if( stack->top == 0 || offset >= stack->top )
	{
		MSG( "offset defines an item that is out of bounds within stack" );
		RETURN( (pbyte*)NULL );
	}

	RETURN( stack->stack + offset * stack->size );
}

/** Dumps all stack-items to stderr.

//file// is the name of the source file of stack dump occurence.
Use the ``__FILE__``-macro to obtain source file name.

//line// is the line within the source file of stack dump occurence.
Use ``__LINE__``-macro to obtain source file name.

//name// defines the stack's name, in string letters.

//stack// is the pointer to stack to be dumped.

//void (*pf)( pbyte*)// is a pointer to callback-function that is used to print
one element. The function retrieves the pointer of the element.
*/
void stack_dump( char* file, int line, char* name,
		STACK* stack, void (*pf)( pbyte* ) )
{
	psize	i;

 	fprintf( stderr, "%s [%d]: %s %p\n", file, line, name, stack );

	for( i = 0; i < stack->top; i++ )
	{
		if( pf )
			(*pf)( stack_access( stack, i ) );
		else
			fprintf( stderr, "\t%p\n", stack_access( stack, i ) );
	}

	fprintf( stderr, "%s [%d]: %s %ld active, %ld left empty\n",
		file, line, name, stack->top, stack->count - stack->top );
}

/** Returns the number of elements in a stack. */
int stack_count( STACK* stack )
{
	if( !stack )
		return 0;

	return stack->top;
}
