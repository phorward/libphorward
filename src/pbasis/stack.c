/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Basis Library
Copyright (C) 2006-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	stack.c
Author:	Jan Max Meyer
Usage:	Universal, dynamic stack management functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include "pbasis.h"

/*
 * Global variables
 */

/*
 * Defines
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		stack_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs a stack initialization.
					
	Parameters:		STACK*	stack			Pointer to stack to be initialized.
					psize	size			Size of one stack element, in bytes.
											This should be evaluated with the
											sizeof()-macro.
					psize	step			Step-wide, where a stack-(re)allo-
											cation will be performed. If, e.g.
											this is set to 128, then, if the 
											128th item is pushed onto the
											stack, a realloction is done.
											Once allocated memory remains until
											the stack is free'd again.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		stack_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees an entire stack. The stack must not be re-initalized
					after destruction.
					
	Parameters:		STACK*	stack			Stack to be freed.
					void	(*ff)( pbyte*)	Pointer to callback-function that
											is used to free one element. The
											function retrieves the pointer of
											the element.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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


/* -FUNCTION--------------------------------------------------------------------
	Function:		stack_push()
	
	Author:			Jan Max Meyer
	
	Usage:			Pushes an element onto the stack. The element's memory
					is copied during the push. The item must be of the same
					memory size as used at stack initialization.
					
	Parameters:		STACK*	stack			Pointer to stack where to push an
											item on.
					pbyte*	item			Pointer to the memory of the item,
											to be pushed onto the stack. Cast
											your type into pbyte, or wrap
											the push-operation with a macro.
	
	Returns:		pbyte*					Returns the address of the newly
											pushed item, and (pbyte*)NULL if
											the item could not be pushed.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		stack_pop()
	
	Author:			Jan Max Meyer
	
	Usage:			Pops an element off the stack.
					The function returns a pointer to the popped item. Because
					dynamic stacks only grow, and no memory is freed, the re-
					turned data pointer is still valid, and will only be over-
					written with the next push operation.
					
	Parameters:		STACK*	stack			Pointer to stack where to pop an
											item off.
	
	Returns:		pbyte*					Returns the address of the popped
											item, and (pbyte*)NULL if the item
											could not be popped (e.g. stack
											is empty).
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		stack_access()
	
	Author:			Jan Max Meyer
	
	Usage:			Accesses any element of the stack.
					
	Parameters:		STACK*	stack			Pointer to stack where to access
											the element from.
					psize	offset			The offset of the element.
	
	Returns:		pbyte*					Returns the address of the accessed
											item, and (pbyte*)NULL if the item
											could not be accessed (e.g. stack
											is empty or offset is beyond the
											top of stack).
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
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

/* -FUNCTION--------------------------------------------------------------------
	Function:		stack_dump()
	
	Author:			Jan Max Meyer
	
	Usage:			Dumps all stack-items to stderr.
					
	Parameters:		uchar*	file			Name of the source file of stack
											dump occurence. Use __FILE__-macro
											to obtain source file name.
					int		line			Line within the source file of stack
											dump occurence. Use __LINE__-macro
											to obtain source file name.
					uchar*	name			Stack's name, in string letters.
					STACK*	stack			Pointer to stack to be dumped.
					void	(*pf)( pbyte*)	Pointer to callback-function that
											is used to print one element. The
											function retrieves the pointer of
											the element.
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void stack_dump( uchar* file, int line, uchar* name,
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

