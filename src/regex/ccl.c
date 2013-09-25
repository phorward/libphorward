/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	ccl.c
Author:	Jan Max Meyer
Usage:	Charclass-Handling
----------------------------------------------------------------------------- */

#define PREGEX_LOCAL
#include "phorward.h"

/* Internal */
pchar pregex_ccl_max( void )
{
#ifdef UNICODE
	return (pchar)0xffff;
#else
	return (pchar)0xff;
#endif
}

pboolean pregex_ccl_end( pregex_ccl ccl )
{
	if( ccl && ( (ccl)->begin == PREGEX_CCL_MAX ) )
		return TRUE;

	return FALSE;
}

pregex_ccl pregex_ccl_free( pregex_ccl ccl )
{
	return (pregex_ccl)pfree( ccl );
}


/** Returns the number of range pairs within a character class (= its length).

//ccl// is a pointer to the character class to be processed.

To retrieve the number of characters in a character class, use
pregex_ccl_count() instead.

Returns the number of pairs the charclass holds.
*/
int pregex_ccl_size( pregex_ccl ccl )
{
	pregex_ccl	i;
	int			cnt	= 0;

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
		cnt++;

	return cnt;
}

/** Returns the number of characters within a character class.

//ccl// is a pointer to the character class to be processed.

Returns the total number of characters the class is holding.
*/
int pregex_ccl_count( pregex_ccl ccl )
{
	pregex_ccl	i;
	int			cnt	= 0;

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
		cnt += ( i->end - i->begin ) + 1;

	return cnt;
}

/** Duplicates a character class into a new one.

//ccl// is the pointer to the character class to be duplicated.

Returns a pointer to the duplicate of //ccl//, or (pregex_ccl)NULL
in error case.
*/
pregex_ccl pregex_ccl_dup( pregex_ccl ccl )
{
	pregex_ccl dup;

	if( !ccl )
	{
		WRONGPARAM;
		return NULL;
	}

	/*
		Don't use memdup() here... there is one pregex_cr of junk always behind
		the terminator, for negating character classes.

		This way it won't come up in valgrind.
	*/
	if( !( dup = (pregex_ccl)pmalloc( ( pregex_ccl_size( ccl ) + 1 + 1 )
						* sizeof( pregex_cr ) ) ) )
		return (pregex_ccl)NULL;

	memcpy( dup, ccl, ( pregex_ccl_size( ccl ) + 1 ) * sizeof( pregex_cr ) );

	return dup;
}

/* Sort-function required for quick sort */
static int pregex_ccl_sortfunc( const void* v_r1, const void* v_r2 )
{
	pregex_ccl	r1	= (pregex_ccl)v_r1;
	pregex_ccl	r2	= (pregex_ccl)v_r2;

	if( r1->begin == r2->begin )
		return 0;
	else if( r1->begin < r2->begin )
		return -1;

	return 1;
}

/* Normalizes a pre-parsed or modified character class. Normalization means,
that duplicate elements will be removed, the range pairs will be sorted and
intersections will be resolved. The result is a unique, normalized character
class to be used for further operations.

//ccl// is the character class to be normalized.
//mem_opt// specifies if memory optimization should be performed, so that
exactly the required size of memory is used that is exactly holds the character
class.

Returns a pointer to ccl or to optimized character class memory address.
*/
static pregex_ccl pregex_ccl_normalize( pregex_ccl ccl, BOOLEAN mem_opt )
{
	pregex_ccl	nccl;
	pregex_ccl	i;
	pregex_ccl	j;
	int			count		= 0;
	int			oldcount	= 0;

	PROC( "pregex_ccl_normalize" );
	PARMS( "ccl", "%p", ccl );

	do
	{
		oldcount = count;

		/* First sort the ccl! */
		qsort( ccl, pregex_ccl_size( ccl ),
				sizeof( pregex_cr ),
					pregex_ccl_sortfunc );

		/* Then, find intersections and... */
		for( i = ccl; i->begin != PREGEX_CCL_MAX; i++ )
		{
			j = i + 1;

			if( j->begin != PREGEX_CCL_MAX )
			{
				if( j->begin <= i->end && j->end >= i->begin )
				{
					/*
					printf( "overlap! %d %d - %d %d\n",
						*i, *(i+1), *j, *(j+1) );
					*/

					if( i->begin > j->begin )
						i->begin = j->begin;
					else
						j->begin = i->begin;

					if( i->end > j->end )
						j->end = i->end;
					else
						i->end = j->end;
				}
				else if( i->end + 1 == j->begin )
				{
					i->end = j->end;
					j->begin = i->begin;
				}
			}
		}

		/* ...remove them! */
		for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
		{
			for( j = i + 1; ; j++ )
			{
				if( j == i + 1 )
				{
					if( j->begin == PREGEX_CCL_MAX || i->begin != j->begin )
						break;
				}
				else
				{
					if( ( ( j - 1 )->begin = j->begin ) != PREGEX_CCL_MAX )
						( j - 1 )->end = j->end;
					else
						break;
				}
			}
		}
	}
	while( ( count = pregex_ccl_size( ccl ) ) != oldcount );

	/* Memory optimization */
	if( mem_opt )
	{
		nccl = pregex_ccl_dup( ccl );
		ccl = pregex_ccl_free( ccl );
		ccl = nccl;
	}

	RETURN( ccl );
}

/** Negates all ranges in a character class.

//ccl// is the pointer to the character class to be negated.

Returns a pointer to //ccl//.
*/
pregex_ccl pregex_ccl_negate( pregex_ccl ccl )
{
	pchar		size	= 0;
	pchar		start;
	pchar		end		= PREGEX_CCL_MIN;
	pregex_ccl	i;
	pregex_ccl	j;

	PROC( "pregex_ccl_negate" );
	PARMS( "ccl", "%p", ccl );

	if( !ccl )
	{
		WRONGPARAM;
		RETURN( (pregex_ccl)NULL );
	}

	for( i = j = ccl; i; i++, size++ )
	{
		if( i->begin > end )
		{
			start = i->begin;
			j->begin = end;
			end = i->end + 1;
			j->end = start - 1;

			j++;

			if( start == PREGEX_CCL_MAX
				|| end == PREGEX_CCL_MAX )
			{
				j->begin = PREGEX_CCL_MAX;
				break;
			}
		}
		else
			end = i->end + 1;
	}

	RETURN( ccl );
}

/** Unions two character classes into a normalized one.

//first// is the pointer to the first character class
//second// is the pointer to the second character class to be unioned with
//first//.

Returns a pointer to //first//, after //first// has been exteded to the
amount of characters from //second//. //second// remains untouched.
*/
pregex_ccl pregex_ccl_union( pregex_ccl first, pregex_ccl second )
{
	int			fsize;
	int			size;
	pregex_ccl	ret;

	PROC( "pregex_ccl_union" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( !( first && second ) )
	{
		WRONGPARAM;
		RETURN( (pregex_ccl)NULL );
	}

	ret = first;
	size = ( fsize = pregex_ccl_size( first ) ) +
				pregex_ccl_size( second ) + 1;

	if( !( ret = prealloc( (pregex_ccl)ret,
			( size  + 2 ) * sizeof( pregex_cr ) ) ) )
	{
		MSG( "Ran out of memory" );
		RETURN( (pregex_ccl)NULL );
	}

	memcpy( ret + fsize, second,
		( ( pregex_ccl_size( second ) ) + 1 ) * sizeof( pregex_cr ) );

	pregex_ccl_normalize( ret, FALSE );

	RETURN( ret );
}

/** Parses a character class definition and returns a normalized character class
to be used for further operations.

//ccldef// is the character class definition string, accepting UTF-8 formatted
input.

Returns a pointer to the newly created character class. This pointer should be
released with pregex_ccl_free() when its existence is no longer required.
*/
pregex_ccl pregex_ccl_create( char* ccldef )
{
	char*		cclptr;
	pregex_ccl	ccl		= (pregex_ccl)NULL;
	pchar		begin, end, swap;
	pchar		size	= 0;

	PROC( "pregex_ccl_create" );
	PARMS( "ccldef", "%s", ccldef );

	/*
		Run this twice ... first one to obtain the required size
			second one to parse the charclass
	*/
	do
	{
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

			/* Test if range is correct */
			if( begin > end )
			{
				swap = end;
				end = begin;
				begin = swap;
			}

			if( !ccl )
				size++;
			else
			{
				ccl[size].begin = begin;
				ccl[size].end = end;
				size++;
			}
		}

		if( !ccl )
		{
			/*
				We allocate one extra-pair to avoid reallocation
				at charclass negation
			*/
			if( !( ccl = (pregex_ccl)pmalloc( ( size + 2 )
											* sizeof( pregex_cr ) ) ) )
			{
				MSG( "I think we have a memory problem..." );
				RETURN( ccl );
			}

			size = 0;
		}
		else
			ccl[ size++ ].begin = PREGEX_CCL_MAX;
	}
	while( !size );

	ccl = pregex_ccl_normalize( ccl, TRUE );

	RETURN( ccl );
}

/** Print character class to output stream.
This function is for debug-purposes only.

//stream// is the output stream to dump the character class to; This can be
left (FILE*)NULL, so stderr will be used.
//ccl// is the pointer to character class

//break_after// defines:
- if < 0 print with pointer info
- if 0 print all into one line
- if > 0 print linewise
-
*/
void pregex_ccl_print( FILE* stream, pregex_ccl ccl, int break_after )
{
	pregex_ccl	i;
	int			cnt;
	char		outstr[ 2 ] [ 10 + 1 ];

	if( !stream )
		stream = stderr;

	if( break_after < 0 )
		fprintf( stream, "*** begin of ccl %p ***\n", ccl );

	for( i = ccl, cnt = 0; i && i->begin != PREGEX_CCL_MAX; i++, cnt++ )
	{
		u8_toutf8( outstr[0], sizeof( outstr[0] ), &( i->begin ), 1 );

		if( i->begin != i->end )
		{
			u8_toutf8( outstr[1], sizeof( outstr[1] ), &( i->end ), 1 );
			fprintf( stream, "'%s' [%d] to '%s' [%d] ",
				outstr[0], (int)i->begin, outstr[1], (int)i->end );
		}
		else
			fprintf( stream, "'%s' [%d] ", outstr[0], (int)i->begin );

		if( break_after > 0 && cnt % break_after == 0 )
			fprintf( stream, "\n" );
	}

	if( break_after < 0 )
		fprintf( stream, "*** end of ccl %p ***\n", ccl );
}

/** Converts a character class back to a string representation of the
character class definition, which in turn can be converted back into a
character class using pregex_ccl_create().

//ccl// is the pointer to character class to be converted.
//escape//, if TRUE, escapes "unprintable" characters in their hexadecimal
representation. If FALSE, it prints all characters, except the zero, which will
be returned as "\0"

Returns the generated string that represents the charclass. The returned pointer
must be released with pfree() after its existence is no longer required.
*/
char* pregex_ccl_to_str( pregex_ccl ccl, pboolean escape )
{
	pregex_ccl	i;
	char		from	[ 40 + 1 ];
	char		to		[ 20 + 1 ];
	char*		ret		= (char*)NULL;

	PROC( "pregex_ccl_to_str" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "escape", "%d", escape );

	if( !( ccl ) )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( escape )
			u8_escape_wchar( from, sizeof( from ), i->begin );
		else
			u8_toutf8( from, sizeof( from ), &( i->begin ), 1 );

		if( i->begin != i->end )
		{
			if( escape )
				u8_escape_wchar( to, sizeof( to ), i->end );
			else
				u8_toutf8( to, sizeof( to ), &( i->end ), 1 );

			sprintf( from + strlen( from ), "-%s", to );
		}

		if( !ret )
		{
			ret = (char*)pmalloc( ( strlen( from ) + 1 )
									* sizeof( char ) );
			*ret = '\0';
		}
		else
			ret = (char*)prealloc( (char*)ret,
					( strlen( ret ) + strlen( from ) + 1 )
									* sizeof( char ) );

		if( !ret )
		{
			MSG( "Out of memory?" );
			RETURN( (char*)NULL );
		}

		strcat( ret, from );
	}

	VARS( "ret", "%s", ret );
	RETURN( ret ? ret : (char*)strdup( "" ) );
}

/** Integrates a character range into a character class.

//ccl// is the pointer to the character class to be affected. If //ccl// is
provided as (pregex_ccl)NULL, it will be created by the function.
//begin// is the begin of character range to be integrated.
//end// is the end of character range to be integrated.

Returns a pointer to the extended version of //ccl// on success, or
(pregex_ccl)NULL on error.
*/
pregex_ccl pregex_ccl_addrange( pregex_ccl ccl, pchar begin, pchar end )
{
	int		size	= pregex_ccl_size( ccl );
	pchar	tmp;

	PROC( "pregex_ccl_addrange" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "begin", "%d", begin );
	PARMS( "end", "%d", end );

	/* Swap begin and end if required */
	if( end < begin )
	{
		MSG( "Swapping begin with end" );
		tmp = end;
		end = begin;
		begin = tmp;
	}

	/* Better check this... */
	if( end >= PREGEX_CCL_MAX )
		end = PREGEX_CCL_MAX - 1;

	/* Is something really to do? */
	for( tmp = begin; tmp <= end; tmp++ )
		if( !pregex_ccl_test( ccl, tmp ) )
			break;

	if( ccl && tmp > end )
	{
		MSG( "Nothing to do!" );
		RETURN( ccl );
	}

	/* Re-allocate */
	if( !( ccl = (pregex_ccl)prealloc( ccl,
			( pregex_ccl_size( ccl ) + 1 + 2 )
				* sizeof( pregex_cr ) ) ) )
	{
		MSG( "Can't (re)allocate memory" );
		RETURN( (pregex_ccl)NULL );
	}

	VARS( "size", "%d", size );
	ccl[ size ].begin = begin;
	ccl[ size ].end = end;

	ccl[ ++size ].begin = PREGEX_CCL_MAX;

	MSG( "Normalizing" );
	RETURN( pregex_ccl_normalize( ccl, FALSE ) );
}

/** Removes a character range from a character class.

//ccl// is the pointer to the character class to be affected.
//begin// is the begin of character range to be removed.
//end// is the end of character range to be removed.

Returns a pointer to the modified version of //ccl// on success, or
(pregex_ccl)NULL on error.
*/
pregex_ccl pregex_ccl_delrange( pregex_ccl ccl, pchar begin, pchar end )
{
	int			size;
	pchar		tmp;
	pregex_ccl	i;

	if( !( ccl ) )
	{
		WRONGPARAM;
		return (pregex_ccl)NULL;
	}

	/* Swap begin and end if required */
	if( end < begin )
	{
		tmp = end;
		end = begin;
		begin = tmp;
	}

	/* Which elements do match? */
	do
	{
		size = pregex_ccl_size( ccl );

		for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
		{
			if( begin <= i->end && end >= i->begin )
			{
				if( begin > i->begin && end < i->end )
				{
					/* Splitting required! */
					tmp = i->end;
					i->end = begin - 1;
					if( !( ccl = pregex_ccl_addrange( ccl, end + 1, tmp ) ) )
						return (pregex_ccl)NULL;

					break;
				}
				else if( begin > i->begin )
					/* Move the end */
					i->end = begin - 1;
				else if( end < i->end )
					/* Move the begin */
					i->begin = end + 1;
				else
				{
					/* Removing whole element required */
					if( i != &( ccl[ size - 1 ] ) )
						memcpy( i, &( ccl[ size - 1 ] ), sizeof( pregex_cr ) );

					ccl[ size - 1 ].begin = PREGEX_CCL_MAX;
					pregex_ccl_normalize( ccl, FALSE );
				}
			}
		}
	}
	while( size != pregex_ccl_size( ccl ) );

	return pregex_ccl_normalize( ccl, FALSE );
}

/** Integrates a character into a character class.

//ccl// is the pointer to the character class to be affected. If //ccl// is
provided as (pregex_ccl)NULL, it will be created by the function.
//ch// is the character to be integrated.

The function is a shortcut for pregex_ccl_addrange().

Returns a pointer to the extended version of //ccl// on success, or
(pregex_ccl)NULL on error.
*/
pregex_ccl pregex_ccl_add( pregex_ccl ccl, pchar ch )
{
	return pregex_ccl_addrange( ccl, ch, ch );
}

/** Removes a character from a character class.

//ccl// is the pointer to the character class to be affected.
//ch// is the character to be removed from //ccl//.

The function is a shortcut for pregex_ccl_delrange().

Returns a pointer to the modified version of //ccl// on success, or
(pregex_ccl)NULL on error.
*/
pregex_ccl pregex_ccl_del( pregex_ccl ccl, pchar ch )
{
	return pregex_ccl_delrange( ccl, ch, ch );
}

/** Tests a character class if it cointains a character.

//ccl// is the pointer to character class to be tested.
//ch// is the character to be tested.

Returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean pregex_ccl_test( pregex_ccl ccl, pchar ch )
{
	pregex_ccl	i;

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( i->begin <= ch && i->end >= ch )
			return TRUE;
	}

	return FALSE;
}

/** Tests a character class to match a character range.

//ccl// is a pointer to the character class to be tested.
//begin// is the begin of character-range to be tested.
//end// is the end of character-range to be tested.

Returns TRUE if the entire character range matches the class, and FALSE if not.
*/
pboolean pregex_ccl_testrange( pregex_ccl ccl, pchar begin, pchar end )
{
	pregex_ccl	i;

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( begin >= i->begin && end <= i->end )
			return TRUE;
	}

	return FALSE;
}

/** Tests for a case-insensitive character to match a character class.

//ccl// is the pointer to character class to be tested,
//ch// is the character to be tested,

Returns TRUE if the character matches the character class and FALSE if not.
*/
pboolean pregex_ccl_instest( pregex_ccl ccl, pchar ch )
{
	if( pregex_ccl_test( ccl, ch ) )
		return TRUE;

	if( iswupper( ch ) )
		ch = towlower( ch );
	else
		ch = towupper( ch );

	return pregex_ccl_test( ccl, ch );
}

/** Checks for differences in two character classes.

//first// is the pointer to the first character class.
//second// is the pointer to the second character class.

Returns a value < 0 if //first// is lower than //second//, 0 if //first// is
equal to //second// or a value > 0 if //first// is greater than //second//.
*/
int pregex_ccl_compare( pregex_ccl first, pregex_ccl second )
{
	pregex_ccl	i;
	pregex_ccl	j;
	int			ret		= 0;

	PROC( "pregex_ccl_compare" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( ( ret = (int)pregex_ccl_size( first ) -
					(int)pregex_ccl_size( second ) ) != 0 )
		RETURN( ret );

	for( i = first, j = second;
			i && i->begin != PREGEX_CCL_MAX && j && j->begin != PREGEX_CCL_MAX;
				i++, j++ )
		if( !( i->begin == j->begin && i->end == j->end ) )
			RETURN( ( ( i->begin > j->begin ) ? 1 : -1 ) );

	RETURN( ret );
}

/** Returns a new character class with all characters that exist in both
provided character classes.

//first// is the pointer to the first character class.
//second// is the pointer to the second character class.

Returns a new character class containing the insersections from //first//
and //second//.
*/
pregex_ccl pregex_ccl_intersect( pregex_ccl first, pregex_ccl second )
{
	psize		cnt				= 0;
	pregex_ccl	i;
	pregex_ccl	j;
	pregex_ccl	intersections	= (pregex_ccl)NULL;
	pregex_cr	inter;

	PROC( "pregex_ccl_intersect" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	for( i = first; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		for( j = second; j && j->begin != PREGEX_CCL_MAX; j++ )
		{
			if( j->begin <= i->end && j->end >= i->begin )
			{
				inter.begin = ( i->begin > j->begin ) ? i->begin : j->begin;
				inter.end = ( i->end > j->end ) ? j->end : i->end;

				VARS( "intersections", "%p", intersections );
				VARS( "size", "%d", pregex_ccl_size( intersections ) + 1 + 1 );

				if( !( intersections = (pregex_ccl)prealloc(
						(pregex_ccl)intersections,
							( pregex_ccl_size( intersections ) + 1 + 1 )
								* sizeof( pregex_cr ) ) ) )
				{
					RETURN( (pregex_ccl)NULL );
				}

				memcpy( &( intersections[cnt++] ),
							&inter, sizeof( pregex_cr ) );
				intersections[cnt].begin = PREGEX_CCL_MAX;
			}
		}
	}

	if( intersections )
		intersections = pregex_ccl_normalize( intersections, TRUE );

	RETURN( intersections );
}

/** Returns the difference quantity of two character classes. All elements from
//second// will be removed from //first//.

//first// is the pointer to the first character class.
//second// is the pointer to the second character class.

Returns a new pointer to a copy of //first//, without the ranges contained in
//second//. Returns (pregex_ccl)NULL in case of memory allocation or parameter
error.
*/
pregex_ccl pregex_ccl_diff( pregex_ccl first, pregex_ccl second )
{
	pregex_ccl		i;
	pregex_ccl		difference;

	PROC( "pregex_ccl_diff" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( !( difference = pregex_ccl_dup( first ) ) )
		return (pregex_ccl)NULL;

	for( i = second; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( !( difference = pregex_ccl_delrange(
								difference, i->begin, i->end ) ) )
			return (pregex_ccl)NULL;
	}

	return difference;
}

