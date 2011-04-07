/* -MODULE----------------------------------------------------------------------
Phorward Basis Library :: Linked Pointer Lists
Copyright (C) 2006-2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward-software<dot>com

File:	llist.c
Usage:	Universal management functions for dynamic linked pointer list
----------------------------------------------------------------------------- */

/* ---------------- */
/* --- Includes --- */
/* ---------------- */
#include "pbasis.h"

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

/* ---------------------- */
/* --- Implementation --- */
/* ---------------------- */

/* -FUNCTION--------------------------------------------------------------------
	Function:		list_push()

	Author:			Jan Max Meyer
						
	Usage:			Pushes a pointer of any type to a linked list of pointers.
					Therefore, the list can act as a stack when using the function
					list_pop() to pop items off this "stack".

					If not used as a stack, list_push() simply appends a node
					to a linked list of nodes.
					
	Parameters:		LIST*		list	Pointer to the element list where the
										element should be pushed on.
										If this is (LIST*)NULL, the item
										acts as the first element of the list.
					void*		ptr		Pointer to the element to be pushed
										on the list.
											
	Returns:		LIST*				Pointer to the first item of the
										linked list of elements.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
LIST* list_push( LIST* list, void* ptr )
{
	LIST*	elem	= (LIST*)NULL;
	LIST*	item	= (LIST*)NULL;

	elem = (LIST*)malloc( sizeof( LIST ) );
	if( elem )
	{
		memset( elem, 0, sizeof( LIST ) );
	
		elem->pptr = ptr;
		elem->next = (LIST*)NULL;

		if( list == (LIST*)NULL )
			list = elem;
		else
		{
			item = list;
			while( item->next != (LIST*)NULL )
			{
				item = item->next;
			}
	
			item->next = elem;
		}
	}

	return list;
} /* list_push() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_pop()

	Author:			Jan Max Meyer
						
	Usage:			Pops the last element off a linked-list of pointers.
					
	Parameters:		LIST*		list	Pointer to the element list where the
										element should be popped off.
										If this is (LIST*)NULL, nothing occurs
										(then, the stack is empty).
					void**		ptr		Pointer to be filled with the pointer
										that was stored on the popped element
										of the list.
											
	Returns:		LIST*				Pointer to the first item of the
										linked list of elements. If the last
										element is popped, (LIST*)NULL is
										returned.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
LIST* list_pop( LIST* list, void** ptr )
{
	LIST*	item	= (LIST*)NULL;
	LIST*	prev	= (LIST*)NULL;
	
	if( list == (LIST*)NULL )
	{
		*ptr = (void*)NULL;
		return (LIST*)NULL;
	}
	else
	{
		item = list;
		while( item->next != (LIST*)NULL )
		{
			prev = item;
			item = item->next;
		}
		
		if( prev != (LIST*)NULL )
		{
			prev->next = (LIST*)NULL;
		}
		
		*ptr = item->pptr;

		if( item == list )
			list = (LIST*)NULL;
			
		free( item );

		item = (LIST*)NULL;		
	}
	
	return list;
} /* list_pop() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_remove()

	Author:			Jan Max Meyer
						
	Usage:			Removes an item from a linked list. Instead as list_pop(),
					list_remove() can remove an item anywhere in the linked
					list.
					
	Parameters:		LIST*		list	Pointer to the element list where the
										element should be popped off.
										If this is (LIST*)NULL, nothing occurs
										(then, the stack is empty).
					void*		ptr		Pointer to be searched for. The element
										with this pointer will be removed from
										the list.
											
	Returns:		LIST*				Pointer to the updated list, or
										(LIST*)NULL if the last item was removed.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
LIST* list_remove( LIST* list, void* ptr )
{
	LIST*	item	= (LIST*)NULL;
	LIST*	prev	= (LIST*)NULL;
	
	if( ptr == (void*)NULL )
		return list;
	
	for( item = list; item != (LIST*)NULL; item = item->next )
	{
		if( item->pptr == ptr )
		{
			if( prev == (LIST*)NULL )
			{
				list = item->next;
			}
			else
			{
				/*printf("remove: prev->next->pptr %p (item %p) = item->next %p\n", ((LIST*)prev->next)->pptr, item->pptr, ((LIST*)item->next)->pptr );*/
				prev->next = item->next;
			}
			
			free( item );
			break;
		}
			
		prev = item;
	}
	
	return list;
} /* list_remove() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_free()

	Author:			Jan Max Meyer
						
	Usage:			Frees a linked list of pointers.
					
	Parameters:		LIST*	list			Linked list to be freed.
											
	Returns:		LIST*					Always pointer to (LIST*)NULL.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
LIST* list_free( LIST* list )
{
	LIST*	next	= (LIST*)NULL;
	LIST*	item	= (LIST*)NULL;

	item = list;
	while( item != (LIST*)NULL )
	{			
		next = item->next;
		free( item );

		item = next;
	}

	return (LIST*)NULL;
} /* list_free() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_print()

	Author:			Jan Max Meyer
						
	Usage:			Prints the items of a linked list of elements for debug
					purposes. This function expects a callback-function which
					then performs the output of the pointer that is associated
					with the list element.
					
	Parameters:		LIST*	list				Linked list to be printed.
					void 	(*callback)(void*)	Callback function that is called
												for each element.
											
	Returns:		void

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void list_print( LIST* list, void (*callback)( void* ) )
{
	LIST*	item	= (LIST*)NULL;

	item = list;
	while( item != (LIST*)NULL )
	{	
		callback( list->pptr );
		item = item->next;
	}
} /* list_print() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_dup()

	Author:			Jan Max Meyer
						
	Usage:			Duplicates a list in a 1:1 copy.
					
	Parameters:		LIST*	list				Linked list to be copied.
											
	Returns:		LIST*						Pointer to the copy if list.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
LIST* list_dup( LIST* src )
{
	LIST*	item	= (LIST*)NULL;
	LIST*	tar		= (LIST*)NULL;
	
	for( item = src; item != (LIST*)NULL; item = item->next )
	{
		tar = list_push( tar, item->pptr );
	}
	
	return tar;
} /* list_dup() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_find()

	Author:			Jan Max Meyer
						
	Usage:			Searches a pointer in a linked list of pointers.
					
	Parameters:		LIST*	list				Linked list where ptr should be
												searched in.
					void*	ptr					Pointer to be searched in the
												linked list.
											
	Returns:		int							-1 if the desired item was not
												found, else the number of the
												element from the list's begin.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int list_find( LIST* list, void* ptr )
{
	LIST*	item	= (LIST*)NULL;
	int		cnt		= 0;
	
	if( ptr == (LIST*)NULL )
		return -1;
	
	for( item = list; item != (LIST*)NULL; item = item->next )
	{
		if( item->pptr == ptr )
			return cnt;
			
		cnt++;
	}
	
	return -1;
} /* list_find() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_getptr()

	Author:			Jan Max Meyer
						
	Usage:			Returns the pointer of the desired position from the linked
					list.
					
	Parameters:		LIST*	list				Linked list where ptr should be
												searched in.
					int		cnt					Number of the item starting from
												the list's first element that
												should be returned.
											
	Returns:		void*						Pointer of the desired position,
												(void*)NULL if the position is
												not in the list.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void* list_getptr( LIST* list, int cnt )
{
	LIST*	item	= (LIST*)NULL;
	
	if( cnt < 0 )
		return (void*)NULL;
	
	for( item = list; item != (LIST*)NULL; item = item->next )
	{		
		if( cnt == 0 )
			return item->pptr;
		
		cnt--;
	}
	
	return (void*)NULL;
} /* list_getptr() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_diff()

	Author:			Jan Max Meyer
						
	Usage:			Verifies two lists if they contain the same pointers.
					
	Parameters:		LIST*	first				First linked list.
					LIST*	second				Second linked list.
											
	Returns:		int							1 if indifferent,
												0 if different

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int list_diff( LIST* first, LIST* second )
{
	int		first_cnt	= 0;
	int		second_cnt	= 0;
	LIST*	item		= (LIST*)NULL;
	
	for( item = first; item != (LIST*)NULL; item = item->next )
		first_cnt++;
	for( item = second; item != (LIST*)NULL; item = item->next )
		second_cnt++;
	
	if( first_cnt == second_cnt && first_cnt > 0 )
	{
		for( item = first; item != (LIST*)NULL; item = item->next )
		{
			first_cnt--;
			if( list_find( second, item->pptr ) >= 0 )
				second_cnt--;
		}
			
		if( first_cnt != second_cnt  )
			return 0;

		return 1;
	}
	
	return 0;
} /* list_diff() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_union()

	Author:			Jan Max Meyer
						
	Usage:			Unions two list to a huger new one.
					
	Parameters:		LIST*	first				First linked list.
					LIST*	second				Second linked list.
											
	Returns:		LIST*						The union of both.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
LIST* list_union( LIST* first, LIST* second )
{
	LIST*	ret		= (LIST*)NULL;
	LIST*	current	= (LIST*)NULL;

	if( first != (LIST*)NULL )
	{
		ret = first;
		for( current = second; current != (LIST*)NULL; current = current->next )
		{
			if( list_find( ret, current->pptr ) == -1 )
			{
				ret = list_push( ret, current->pptr );
			}
		}
	}
	else
	{
		ret = list_dup( second );
	}
	
	return ret;
} /* list_union() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_count()

	Author:			Jan Max Meyer
						
	Usage:			Counts the elements in a list.
					
	Parameters:		LIST*	list				List start point which items
												should be counted.
											
	Returns:		int							Number of items contained by
												the list!

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int list_count( LIST* list )
{
	int		count		= 0;

	for( ; list != (LIST*)NULL; list = list->next )
		count++;

	return count;
} /* list_count() */


/* -FUNCTION--------------------------------------------------------------------
	Function:		list_subset()

	Author:			Jan Max Meyer
						
	Usage:			Checks if a list contains a subset of another list.
					
	Parameters:		LIST*	list				First linked list.
					LIST*	subset				Possible subset list.
											
	Returns:		int							0 in case if it is not a subset,
												!0 if it is a subset.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	04.11.2009	Jan Max Meyer	Fixed this function, it never worked before ;)
----------------------------------------------------------------------------- */
int list_subset( LIST* list, LIST* subset )
{
	LIST*	current	= (LIST*)NULL;

	if( list != (LIST*)NULL )
	{
		for( current = subset; current != (LIST*)NULL; current = current->next )
		{
			if( list_find( list, current->pptr ) < 0 )
				break;
		}
		
		if( current == (LIST*)NULL )
			return 1;
	}

	return 0;
} /* list_subset() */

