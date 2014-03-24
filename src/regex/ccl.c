/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	ccl.c
Author:	Jan Max Meyer
Usage:	Charclass-Handling
----------------------------------------------------------------------------- */

#define PREGEX_LOCAL
#include "phorward.h"

/* Sort-function required for quick sort */
static int ccl_SORTFUNC( plist* list, plistel* el, plistel* er )
{
	pregex_cr*	l	= (pregex_cr*)plist_access( el );
	pregex_cr*	r	= (pregex_cr*)plist_access( er );

	return r->begin - l->begin;
}

/** Constructor function to create a new character-class.


Returns a pointer to the newly created character-class. This pointer should be
released with pregex_ccl_free() when its existence is no longer required.
*/
pregex_ccl* pregex_ccl_create( int min, int max, char* ccldef )
{
	pregex_ccl*	ccl;

	PROC( "pregex_ccl_create" );
	PARMS( "min", "%d", min );
	PARMS( "max", "%d", max );
	PARMS( "ccldef", "%s", ccldef );

	if( min < 0 )
		min = PREGEX_CCL_MIN;
	if( max < 0 )
		max = PREGEX_CCL_MAX;

	ccl = (pregex_ccl*)pmalloc( sizeof( pregex_ccl ) );

	ccl->ranges = plist_create( sizeof( pregex_cr ), PLIST_MOD_RECYCLE );
	plist_set_sortfn( ccl->ranges, ccl_SORTFUNC );

	if( min > max )
	{
		ccl->min = max;
		ccl->max = min;
	}
	else
	{
		ccl->min = min;
		ccl->max = max;
	}

	if( ccldef )
		pregex_ccl_parse( ccl, ccldef, FALSE );


	RETURN( ccl );
}

/** Checks if the character-classes //l// and //r// are in the same
character universe and compatible for operations. */
pboolean pregex_ccl_compat( pregex_ccl* l, pregex_ccl* r )
{
	if( !( l && r ) )
		return FALSE;

	if( l->min != r->min && l->max != r->max )
		return FALSE;

	return TRUE;
}

/** Returns the number of range pairs within a character-class.

//ccl// is a pointer to the character-class to be processed.

To retrieve the number of characters in a character-class, use
pregex_ccl_count() instead.

Returns the number of pairs the charclass holds.
*/
int pregex_ccl_size( pregex_ccl* ccl )
{
	if( !ccl )
	{
		WRONGPARAM;
		return 0;
	}

	return plist_count( ccl->ranges );
}

/** Returns the number of characters within a character-class.

//ccl// is a pointer to the character-class to be processed.

Returns the total number of characters the class is holding.
*/
int pregex_ccl_count( pregex_ccl* ccl )
{
	plistel*	e;
	pregex_cr*	cr;
	int			cnt	= 0;

	if( !ccl )
	{
		WRONGPARAM;
		return 0;
	}

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		cr = (pregex_cr*)plist_access( e );
		cnt += ( cr->end - cr->begin ) + 1;
	}

	return cnt;
}

/** Duplicates a character-class into a new one.

//ccl// is the pointer to the character-class to be duplicated.

Returns a pointer to the duplicate of //ccl//, or (pregex_cr)NULL
in error case.
*/
pregex_ccl* pregex_ccl_dup( pregex_ccl* ccl )
{
	pregex_ccl* dup;
	plistel*	e;

	if( !ccl )
	{
		WRONGPARAM;
		return (pregex_ccl*)NULL;
	}

	/* Create new, empty ccl */
	dup = pregex_ccl_create( ccl->min, ccl->max, (char*)NULL );

	/* Copy elements */
	plist_for( ccl->ranges, e )
		plist_push( dup->ranges, plist_access( e ) );

	return dup;
}

/* Normalizes a pre-parsed or modified character-class.

Normalization means, that duplicate elements will be removed, the range pairs
are sorted and intersections are resolved. The result is a unique, normalized
character-class to be used for further operations.

//ccl// is the character-class to be normalized.

Returns the number of cycles used for normalization.
*/
static int pregex_ccl_normalize( pregex_ccl* ccl )
{
	pregex_ccl*	nccl;
	plistel*	e;
	plistel*	f;
	pregex_cr*	l;
	pregex_cr*	r;
	int			count		= 0;
	int			oldcount	= 0;
	int			cycles		= 0;

	PROC( "pregex_ccl_normalize" );
	PARMS( "ccl", "%p", ccl );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	do
	{
		oldcount = count;

		/* First sort the character ranges */
		plist_sort( ccl->ranges );

		/* Then, find intersections and... */
		for( e = plist_first( ccl->ranges ); e; )
		{
			l = (pregex_cr*)plist_access( e );

			if( ( r = (pregex_cr*)plist_access( plist_next( e ) ) ) )
			{
				if( r->begin <= l->end && r->end >= l->begin )
				{
					if( r->end > l->end )
						l->end = r->end;

					plist_remove( ccl->ranges, plist_next( e ) );
					continue;
				}
				else if( l->end + 1 == r->begin )
				{
					l->end = r->end;
					plist_remove( ccl->ranges, plist_next( e ) );
					continue;
				}
			}

			e = plist_next( e );
		}

		cycles++;
	}
	while( ( count = pregex_ccl_size( ccl ) ) != oldcount );

	RETURN( cycles );
}

/** Tests a character-class to match a character range.

//ccl// is a pointer to the character-class to be tested.
//begin// is the begin of character-range to be tested.
//end// is the end of character-range to be tested.

Returns TRUE if the entire character range matches the class, and FALSE if not.
*/
pboolean pregex_ccl_testrange( pregex_ccl* ccl, wchar_t begin, wchar_t end )
{
	pregex_cr*	cr;
	plistel*	e;

	if( !( ccl ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		cr = (pregex_cr*)plist_access( e );

		if( begin >= cr->begin && end <= cr->end )
			return TRUE;
	}

	return FALSE;
}

/** Tests a character-class if it cointains a character.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

The function is a shortcut for pregex_ccl_testrange().

It returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean pregex_ccl_test( pregex_ccl* ccl, wchar_t ch )
{
	return pregex_ccl_testrange( ccl, ch, ch );
}

/** Tests for a character in case-insensitive-mode if it matches
a character-class.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

The function is a shortcut for pregex_ccl_testrange().

It returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean pregex_ccl_instest( pregex_ccl* ccl, wchar_t ch )
{
	if( !ccl )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( pregex_ccl_test( ccl, ch ) )
		return TRUE;

#if UNICODE
	if( iswupper( ch ) )
		ch = towlower( ch );
	else
		ch = towupper( ch );
#else
	if( isupper( ch ) )
		ch = tolower( ch );
	else
		ch = toupper( ch );
#endif

	return pregex_ccl_test( ccl, ch );
}

/* Internal function without normalization */
static pboolean pregex_ccl_ADDRANGE( pregex_ccl* ccl, wchar_t begin, wchar_t end )
{
	pregex_cr	cr;

	PROC( "pregex_ccl_ADDRANGE" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( begin > end )
	{
		cr.begin = end;
		cr.end = begin;
	}
	else
	{
		cr.begin = begin;
		cr.end = end;
	}

	if( cr.begin < ccl->min )
		cr.begin = ccl->min;

	if( cr.end > ccl->max )
		cr.end = ccl->max;

	if( pregex_ccl_testrange( ccl, cr.begin, cr.end ) )
	{
		MSG( "Range already in character-class" );
		RETURN( TRUE );
	}

	if( cr.begin > ccl->max || cr.end < ccl->min )
	{
		MSG( "Character-range not in the universe of character-class" );
		RETURN( FALSE );
	}

	plist_push( ccl->ranges, (void*)&cr );

	RETURN( TRUE );
}

/** Integrates a character range into a character-class.

//ccl// is the pointer to the character-class to be affected. If //ccl// is
provided as (pregex_ccl*)NULL, it will be created by the function.

//begin// is the begin of character range to be integrated.
//end// is the end of character range to be integrated.

If //begin// is greater than //end//, the values will be swapped.
*/
pboolean pregex_ccl_addrange( pregex_ccl* ccl, wchar_t begin, wchar_t end )
{
	PROC( "pregex_ccl_addrange" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	if( !pregex_ccl_ADDRANGE( ccl, begin, end ) )
		RETURN( FALSE );

	pregex_ccl_normalize( ccl );
	RETURN( TRUE );
}


/** Integrates a single character into a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be integrated.

The function is a shortcut for pregex_ccl_addrange().
*/
pboolean pregex_ccl_add( pregex_ccl* ccl, wchar_t ch )
{
	return pregex_ccl_addrange( ccl, ch, ch );
}

/** Removes a character range from a character-class.

//ccl// is the pointer to the character-class to be affected.
//begin// is the begin of character range to be removed.
//end// is the end of character range to be removed.
*/
pboolean pregex_ccl_delrange( pregex_ccl* ccl, wchar_t begin, wchar_t end )
{
	plistel*	e;
	pregex_cr	d;
	pregex_cr*	r;

	PROC( "pregex_ccl_delrange" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( begin > end )
	{
		d.begin = end;
		d.end = begin;
	}
	else
	{
		d.begin = begin;
		d.end = end;
	}

	/* Which elements do match? */
	do
	{
		for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
		{
			r = (pregex_cr*)plist_access( e );

			if( d.begin <= r->end && end >= r->begin )
			{
				/* Slitting required? */
				if( d.begin > r->begin && d.end < r->end )
				{
					MSG( "Split current range" );
					end = r->end;
					r->end = d.begin - 1;

					if( !pregex_ccl_addrange( ccl, d.end + 1, end ) )
						RETURN( FALSE );

					break;
				}
				/* Move the end */
				else if( d.begin > r->begin )
				{
					MSG( "Move end of current range" );
					r->end = d.begin - 1;
				}
				/* Move the begin */
				else if( d.end < r->end )
				{
					MSG( "Change begin of current range" );
					r->begin = d.end + 1;
				}
				/* Remove entire range */
				else
				{
					MSG( "Remove entire range" );
					plist_remove( ccl->ranges, e );

					e = plist_first( ccl->ranges );
					break;
				}
			}
		}
	}
	while( e );

	pregex_ccl_normalize( ccl );
	RETURN( TRUE );
}

/** Removes a character from a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be removed from //ccl//.

The function is a shortcut for pregex_ccl_delrange().
*/
pboolean pregex_ccl_del( pregex_ccl* ccl, wchar_t ch )
{
	return pregex_ccl_delrange( ccl, ch, ch );
}

/** Negates all ranges in a character-class.

//ccl// is the pointer to the character-class to be negated.

Returns a pointer to //ccl//.
*/
pboolean pregex_ccl_negate( pregex_ccl* ccl )
{
	wchar_t		start;
	wchar_t		end;
	plistel*	e;
	plistel*	ne;
	pregex_cr*	r;

	PROC( "pregex_ccl_negate" );
	PARMS( "ccl", "%p", ccl );

	if( !ccl )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	start = end = ccl->min;

	for( e = plist_first( ccl->ranges ); e; e = ne )
	{
		r = (pregex_cr*)plist_access( e );
		ne = plist_next( e );

		if( end < r->begin )
		{
			start = r->begin;
			r->begin = end;

			end = r->end + 1;
			r->end = start - 1;
		}
		else
		{
			end = r->end + 1;
			plist_remove( ccl->ranges, e );
		}
	}

	if( end < ccl->max )
		pregex_ccl_addrange( ccl, end, ccl->max );

	pregex_ccl_normalize( ccl );

	RETURN( TRUE );
}

/** Unions two character-classes into a new, normalized one.

//ccl// is the pointer to the character-class that will be extended to all
ranges contained in //add//.
//add// is character-class that will be unioned with //ccl//.

The function creates and returns a new character-class that is the union
if //ccl// and //add//.
*/
pregex_ccl* pregex_ccl_union( pregex_ccl* ccl, pregex_ccl* add )
{
	pregex_ccl*	un;
	plistel*	e;
	pregex_cr*	r;

	PROC( "pregex_ccl_union" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "add", "%p", add );

	if( !( ccl && add ) )
	{
		WRONGPARAM;
		RETURN( (pregex_ccl*)NULL );
	}

	if( !pregex_ccl_compat( ccl, add ) )
	{
		MSG( "Incompatible character-classes" );
		RETURN( (pregex_ccl*)NULL );
	}

	un = pregex_ccl_dup( ccl );

	for( e = plist_first( add->ranges ); e; e = plist_next( e ) )
	{
		r = (pregex_cr*)plist_access( e );

		if( !pregex_ccl_ADDRANGE( un, r->begin, r->end ) )
			RETURN( FALSE );
	}

	pregex_ccl_normalize( un );

	RETURN( un );
}


/** Returns the difference quantity of two character-classes.
All elements from //rem// will be removed from //ccl//, and put into a
new character-class.

//ccl// is the pointer to the first character-class.
//rem// is the pointer to the second character-class.

Returns a new pointer to a copy of //ccl//, without the ranges contained in
//rem//. Returns (pregex_ccl*)NULL in case of memory allocation or parameter
error.
*/
pregex_ccl* pregex_ccl_diff( pregex_ccl* ccl, pregex_ccl* rem )
{
	plistel*	e;
	pregex_cr*	r;
	pregex_ccl*	diff;

	PROC( "pregex_ccl_diff" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "rem", "%p", rem );

	if( !( ccl && rem ) )
	{
		WRONGPARAM;
		RETURN( (pregex_ccl*)NULL );
	}

	if( !pregex_ccl_compat( ccl, rem ) )
	{
		MSG( "Incompatible character-classes" );
		RETURN( (pregex_ccl*)NULL );
	}

	if( !( diff = pregex_ccl_dup( ccl ) ) )
		return (pregex_ccl*)NULL;

	for( e = plist_first( rem->ranges ); e; e = plist_next( e ) )
	{
		r = (pregex_cr*)plist_access( e );
		pregex_ccl_delrange( diff, r->begin, r->end );
	}

	return diff;
}

/** Checks for differences in two character-classes.

//left// is the pointer to the first character-class.
//right// is the pointer to the second character-class.

Returns a value < 0 if //left// is lower than //right//, 0 if //left// is
equal to //right// or a value > 0 if //left// is greater than //right//.
*/
int pregex_ccl_compare( pregex_ccl* left, pregex_ccl* right )
{
	plistel*	e;
	plistel*	f;
	pregex_cr*	l;
	pregex_cr*	r;
	int			ret		= 0;

	PROC( "pregex_ccl_compare" );
	PARMS( "left", "%p", left );
	PARMS( "right", "%p", right );

	if( !( left && right ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	if( !pregex_ccl_compat( left, right ) )
	{
		MSG( "Incompatible character-classes" );
		RETURN( left->max - right->max );
	}

	if( ( ret = pregex_ccl_size( left ) - pregex_ccl_size( right ) ) != 0 )
	{
		MSG( "Unequal number of range pairs" );
		RETURN( ret );
	}

	MSG( "Deep check all ranges" );
	for( e = plist_first( left->ranges ), f = plist_first( right->ranges );
			e && f; e = plist_next( e ), f = plist_next( f ) )
	{
		l = (pregex_cr*)plist_access( e );
		r = (pregex_cr*)plist_access( f );

		if( !( l->begin == r->begin && l->end == r->end ) )
		{
			MSG( "Ranges not equal" );
			RETURN( ( ( l->begin > r->begin ) ? 1 : -1 ) );
		}
	}

	MSG( "Character-classes are equal" );
	RETURN( ret );
}

/** Returns a new character-class with all characters that exist in both
provided character-classes.

//ccl// is the pointer to the first character-class.
//within// is the pointer to the second character-class.

Returns a new character-class containing the insersections from //ccl//
and //within//. If there is no intersection between both character-classes,
the function returns (pregex_ccl*)NULL.
*/
pregex_ccl* pregex_ccl_intersect( pregex_ccl* ccl, pregex_ccl* within )
{
	plistel*	e;
	plistel*	f;
	pregex_cr*	r;
	pregex_cr*	s;
	pregex_ccl*	in	= (pregex_ccl*)NULL;

	PROC( "pregex_ccl_intersect" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "within", "%p", within );

	if( !( ccl && within ) )
	{
		WRONGPARAM;
		RETURN( (pregex_ccl*)NULL );
	}

	if( !pregex_ccl_compat( ccl, within ) )
	{
		MSG( "Character-classes are not compatible" );
		RETURN( (pregex_ccl*)NULL );
	}

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		r = (pregex_cr*)plist_access( e );

		for( f = plist_first( within->ranges ); f; f = plist_next( f ) )
		{
			s = (pregex_cr*)plist_access( f );

			if( s->begin <= r->end && s->end >= r->begin )
			{
				if( !in )
					in = pregex_ccl_create( ccl->min, ccl->max, (char*)NULL );

				pregex_ccl_addrange( in,
					( r->begin > s->begin ) ? r->begin : s->begin,
					( r->end > s->end ) ? s->end : r->end );
			}
		}
	}

	if( in )
	{
		MSG( "Normalizing" );
		pregex_ccl_normalize( in );
	}

	RETURN( in );
}

/** Return a character or a character-range by its offset.

If the function is called only with pointer //from// provided, and //to// as
(char*)NULL, it writes the character in //offset//th position of the
character-class into from.

If the function is called both with pointer //from// and //to// provided,
it writes the //begin// and //end// character of the character-range in the
//offset//th position of the character-class into //from// and //to//.

If no character or range with the given offset was found, the function
returns FALSE, meaning that the end of the characters is reached.
On success, the function will always return TRUE. */
pboolean pregex_ccl_get( wchar_t* from, wchar_t* to, pregex_ccl* ccl, int offset )
{
	plistel*	e;
	pregex_cr*	cr;

	PROC( "pregex_ccl_get" );
	PARMS( "from", "%p", from );
	PARMS( "to", "%p", to );
	PARMS( "ccl", "%p", ccl );
	PARMS( "offset", "%d", offset );

	if( !( ccl && from && offset >= 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !to )
	{
		MSG( "Single-character retrival" );

		for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
		{
			cr = (pregex_cr*)plist_access( e );

			VARS( "offset", "%d", offset );
			VARS( "cr->begin", "%d", cr->begin );
			VARS( "cr->end", "%d", cr->end );
			VARS( "LEN cr->end - cr->begin + 1", "%d",
					cr->end - cr->begin + 1 );

			if( offset >= ( cr->end - cr->begin ) + 1 )
			{
				MSG( "Offset not in this range" );
				offset -= ( cr->end - cr->begin ) + 1;
			}
			else if( offset < ( cr->end - cr->begin ) + 1 )
			{
				MSG( "Offset is within this class" );

				*from = cr->begin + offset;

				VARS( "*from", "%d", *from );
				RETURN( TRUE );
			}
		}
	}
	else
	{
		MSG( "Range retrival" );

		if( ( cr = (pregex_cr*)plist_access(
						plist_get( ccl->ranges, offset ) ) ) )
		{
			*from = cr->begin;
			*to = cr->end;

			VARS( "*from", "%d", *from );
			VARS( "*to", "%d", *to );

			RETURN( TRUE );
		}
	}

	MSG( "Offset not available" );
	RETURN( FALSE );
}


/** Parses the character-class definition provided in //ccldef// and assigns
this definition to the character-class //ccl//. //ccldef// may contain
UTF-8 formatted input. Escape-sequences will be interpretered to their correct
character representations.

A typical character-class definition simply exists of single characters and
range definitions. For example, "$A-Z#0-9" defines a character-class that
consists of the characters "$#0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ".

The parameter //extend// specifies, if the provided character-class overwrites
(//extend// = FALSE) or extends (//extend// = TRUE) the provided
character-class. This means that definitions that already exist in the
character-class, should be erased first or not.

The function returns TRUE on success, and FALSE on an error.
*/
pboolean pregex_ccl_parse( pregex_ccl* ccl, char* ccldef, pboolean extend )
{
	char*		cclptr;
	pregex_cr	cr;
	wchar_t		begin;
	wchar_t		end;

	PROC( "pregex_ccl_parse" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "ccldef", "%s", ccldef );
	PARMS( "extend", "%s", BOOLEAN_STR( extend ) );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !extend )
		pregex_ccl_erase( ccl );

	for( cclptr = pgetstr( ccldef ); *cclptr; )
	{
		VARS( "cclptr", "%s", cclptr );

		cclptr += pstrparsechar( &begin, cclptr, TRUE );
		end = begin;

		VARS( "begin", "%d", begin );
		VARS( "end", "%d", end );

		/* Is this a range def? */
		if( *cclptr == '-' )
		{
			MSG( "This is a range!" );
			cclptr++;
			cclptr += pstrparsechar( &end, cclptr, TRUE );
			VARS( "end", "%d", end );
		}

		VARS( "cclptr", "%s", cclptr );

		pregex_ccl_ADDRANGE( ccl, begin, end );
	}

	MSG( "Finally normalize the character-class" );
	pregex_ccl_normalize( ccl );

	RETURN( TRUE );
}

/** Erases a character-class //ccl//.

The function sets a character-class to zero, as it continas no character range
definitions. The object //ccl// will be still alive. To delete the entire
object, use pregex_ccl_free().
*/
pboolean pregex_ccl_erase( pregex_ccl* ccl )
{
	if( !ccl )
	{
		WRONGPARAM;
		return FALSE;
	}

	return plist_erase( ccl->ranges );
}

/** Frees a character-class //ccl// and all its used memory.

The function always returns (pregex_ccl*)NULL.
*/
pregex_ccl* pregex_ccl_free( pregex_ccl* ccl )
{
	if( !ccl )
		return (pregex_ccl*)NULL;

	plist_free( ccl->ranges );
	pfree( ccl->str );

	pfree( ccl );

	return (pregex_ccl*)NULL;
}

/** Converts a character-class back to a string representation of the
character-class definition, which in turn can be converted back into a
character-class using pregex_ccl_create().

//ccl// is the pointer to character-class to be converted.
//escape//, if TRUE, escapes "unprintable" characters in their hexadecimal
representation. If FALSE, it prints all characters, except the zero, which will
be returned as "\0"

Returns a pointer to the generated string that represents the charclass.
The returned pointer belongs to the //ccl// and is managed by the
character-class handling functions, so it should not be freed manually.
*/
char* pregex_ccl_to_str( pregex_ccl* ccl, pboolean escape )
{
	plistel*	e;
	pregex_cr*	r;
	char		from	[ 40 + 1 ];
	char		to		[ 20 + 1 ];

	PROC( "pregex_ccl_to_str" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "escape", "%s", BOOLEAN_STR( escape ) );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	ccl->str = pfree( ccl->str );

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		r = (pregex_cr*)plist_access( e );

		if( escape )
			u8_escape_wchar( from, sizeof( from ), r->begin );
		else
			u8_toutf8( from, sizeof( from ), &( r->begin ), 1 );

		if( r->begin != r->end )
		{
			if( escape )
				u8_escape_wchar( to, sizeof( to ), r->end );
			else
				u8_toutf8( to, sizeof( to ), &( r->end ), 1 );

			sprintf( from + strlen( from ), "-%s", to );
		}

		ccl->str = pstrcatstr( ccl->str, from, FALSE );
	}

	VARS( "ret", "%s", ccl->str );
	RETURN( ccl->str ? ccl->str : "" );
}

/** Print character-class to output stream.
This function is provided for debug-purposes only.

//stream// is the output stream to dump the character-class to; This can be
left (FILE*)NULL, so //stderr// will be used.
//ccl// is the pointer to character-class

//break_after// defines:
- if < 0 print with pointer info
- if 0 print all into one line
- if > 0 print linewise
-
*/
void pregex_ccl_print( FILE* stream, pregex_ccl* ccl, int break_after )
{
	plistel*	e;
	pregex_cr*	r;
	int			cnt;
	char		outstr[ 2 ] [ 10 + 1 ];

	if( !( ccl ) )
		return;

	if( !stream )
		stream = stderr;

	if( break_after < 0 )
		fprintf( stream, "*** begin of ccl %p ***\n", ccl );

	for( e = plist_first( ccl->ranges ), cnt = 0;
			e; e = plist_next( e ), cnt++ )
	{
		r = (pregex_cr*)plist_access( e );

		u8_toutf8( outstr[0], sizeof( outstr[0] ), &( r->begin ), 1 );

		if( r->begin != r->end )
		{
			u8_toutf8( outstr[1], sizeof( outstr[1] ), &( r->end ), 1 );
			fprintf( stream, "'%s' [%d] to '%s' [%d] ",
				outstr[0], (int)r->begin, outstr[1], (int)r->end );
		}
		else
			fprintf( stream, "'%s' [%d] ", outstr[0], (int)r->begin );

		if( break_after > 0 && cnt % break_after == 0 )
			fprintf( stream, "\n" );
	}

	if( break_after < 0 )
		fprintf( stream, "*** end of ccl %p ***\n", ccl );
}

#if 0
int main( int argc, char** argv )
{
	pregex_ccl*	c;
	pregex_ccl*	d;
	pregex_ccl* e;

	c = pregex_ccl_create( 0, 255, "A-Za-z0-9" );
	d = pregex_ccl_create( 0, 255, "A-@." );

	/*pregex_ccl_addrange( c, 0, 10 );*/
	pregex_ccl_print( NULL, c, 1 );
	pregex_ccl_print( NULL, d, 1 );

	e = pregex_ccl_union( c, d );
	pregex_ccl_print( NULL, e, 1 );

	printf( "%s\n", pregex_ccl_to_str( e, TRUE ) );

	/*
	pregex_ccl_negate( c );
	pregex_ccl_delrange( c, 0, 255 );
	pregex_ccl_print( c );
	*/

	return 0;
}
#endif

