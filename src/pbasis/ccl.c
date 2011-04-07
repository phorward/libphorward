/* -MODULE----------------------------------------------------------------------
Phorward Basis Library :: Chraracter class handling
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail@phorward-software.com

File:	ccl.c
Author:	Jan Max Meyer
Usage:	Charclass-Handling
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
	Function:		ccl_size()
	
	Author:			Jan Max Meyer
	
	Usage:			Return the number of range pairs within a character-class.
					
	Parameters:		CCL			ccl				Pointer to the character-class
	
	Returns:		int						Number of pairs the charclass
												holds.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int ccl_size( CCL ccl )
{
	CCL		i;
	int		cnt	= 0;

	for( i = ccl; i && i->begin != CCL_MAX; i++ )
		cnt++;

	return cnt;
}

/* Sort-function for quick sort */
static int ccl_sortfunc( const void* v_r1, const void* v_r2 )
{
	CCL		r1	= (CCL)v_r1;
	CCL		r2	= (CCL)v_r2;

	if( r1->begin == r2->begin )
		return 0;
	else if( r1->begin < r2->begin )
		return -1;

	return 1;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_normalize()
	
	Author:			Jan Max Meyer
	
	Usage:			Normalizes a pre-parsed or modified character-class.
					Normalization means, that duplicate elements will be
					removed, the range pairs will be sorted and intersections
					will be resolved. The result is a unique, normalized
					character class to be used for further operations.
					
	Parameters:		CCL			ccl				The character-class to be
												normalized.
					BOOLEAN		mem_opt			Run memory optimization, so
												that only memory is used
												that is exactly holds the
												character class.
												
	
	Returns:		CCL							Pointer to ccl or to optimized
												character class memory address.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static CCL ccl_normalize( CCL ccl, BOOLEAN mem_opt )
{
	CCL		nccl;
	CCL		i;
	CCL		j;
	int		count		= 0;
	int		oldcount	= 0;

	PROC( "ccl_normalize" );
	PARMS( "ccl", "%p", ccl );

	do
	{
		oldcount = count;

		/* First sort the ccl! */
		qsort( ccl, ccl_size( ccl ), sizeof( CRANGE ), ccl_sortfunc );

		/* Then, find intersections and... */
		for( i = ccl; i->begin != CCL_MAX; i++ )
		{
			j = i + 1;

			if( j->begin != CCL_MAX )
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
		for( i = ccl; i && i->begin != CCL_MAX; i++ )
		{
			for( j = i + 1; ; j++ )
			{
				if( j == i + 1 )
				{
					if( j->begin == CCL_MAX || i->begin != j->begin )
						break;
				}
				else
				{
					if( ( ( j - 1 )->begin = j->begin ) != CCL_MAX )
						( j - 1 )->end = j->end;
					else
						break;
				}
			}
		}
	}
	while( ( count = ccl_size( ccl ) ) != oldcount );

	/* Memory optimization */
	if( mem_opt )
	{
		nccl = ccl_dup( ccl );
		pfree( ccl );

		ccl = nccl;
	}

	RETURN( ccl );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_dup()
	
	Author:			Jan Max Meyer
	
	Usage:			Duplicates one character class to new memory. Returns
					(wchar*)NULL if no more memory is left.
					
	Parameters:		CCL			ccl				Pointer to the character-class
												to be duplicated
	
	Returns:		CCL							Pointer to the duplicate of ccl,
												(wchar*)NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_dup( CCL ccl )
{
	return (CCL)memdup( ccl, ( ( ccl_size( ccl ) + 1 ) + 1 )
								* sizeof( CRANGE ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_negate()
	
	Author:			Jan Max Meyer
	
	Usage:			Negates all ranges in a character class.
					
	Parameters:		CCL			ccl				Pointer to the character-class
												to be negated.
	
	Returns:		CCL							Returns a pointer to the same
												class.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_negate( CCL ccl )
{
	wchar	size	= 0;
	wchar	start;
	wchar	end		= CCL_MIN;
	CCL		i;
	CCL		j;

	PROC( "ccl_negate" );
	PARMS( "ccl", "%p", ccl );
	
	for( i = j = ccl; i; i++, size++ )
	{
		if( i->begin > end )
		{
			start = i->begin;
			j->begin = end;
			end = i->end + 1;
			j->end = start - 1;

			j++;

			if( start == CCL_MAX
				|| end == CCL_MAX )
			{
				j->begin = CCL_MAX;
				break;
			}
		}
		else
			end = i->end + 1;
	}

	RETURN( ccl );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_union()
	
	Author:			Jan Max Meyer
	
	Usage:			Unions two character classes into one normalized one.
					
	Parameters:		CCL			first			Pointer to the first 
												character-class
					CCL			second			Pointer to the second
												character-class
	
	Returns:		wchar*						Returns a pointer to the
												unioned character class.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_union( CCL first, CCL second )
{
	int		fsize;
	int		size;
	CCL		ret;

	PROC( "ccl_union" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	ret = first;
	size = ( fsize = ccl_size( first ) ) + ccl_size( second ) + 1;

	if( !( ret = prealloc( (CCL)ret,
			( size  + 2 ) * sizeof( CRANGE ) ) ) )
	{
		MSG( "Ran out of memory" );
		RETURN( (CCL)NULL );
	}

	memcpy( ret + fsize, second,
		( ( ccl_size( second ) ) + 1 ) * sizeof( CRANGE ) );

	ccl_normalize( ret, FALSE );

	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_create()
	
	Author:			Jan Max Meyer
	
	Usage:			Parses a character-class definition and returns a
					normalized character class to be used for further
					operations.
					
	Parameters:		uchar*		ccldef			character class definition
												string, in UTF-8.
	
	Returns:		CCL							Returns a pointer to the newly
												created character class.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_create( uchar* ccldef )
{
	uchar*	cclptr;
	CCL		ccl		= (CCL)NULL;
	wchar	begin, end, swap;
	wchar	size	= 0;

	PROC( "ccl_create" );
	PARMS( "ccldef", "%s", ccldef );
	
	/*
		Run this twice ... first one to obtain the required size
			second one to parse the charclass
	*/
	do
	{
		for( cclptr = ccldef; *cclptr; )
		{
			VARS( "cclptr", "%s", cclptr );

			begin = end = u8_parse_char( &cclptr );
			
			VARS( "begin", "%d", begin );
			VARS( "end", "%d", end );

			/* Is this a range def? */
			if( *cclptr == '-' )
			{
				MSG( "This is a range!" );

				cclptr++;
				
				end = u8_parse_char( &cclptr );
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
			if( !( ccl = (CCL)pmalloc( ( size + 2 )
											* sizeof( CRANGE ) ) ) )
			{
				MSG( "I think we have a memory problem..." );
				RETURN( ccl );
			}

			size = 0;
		}
		else
			ccl[ size++ ].begin = CCL_MAX;
	}
	while( !size );

	ccl = ccl_normalize( ccl, TRUE );

	RETURN( ccl );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_print()
	
	Author:			Jan Max Meyer
	
	Usage:			Print character class to output stream
					For Debug-Purposes only!
					
	Parameters:		FILE*		stream			Output stream; Can be
												left (FILE*)NULL, so stderr
												will be used.
					CCL			ccl				Pointer to character-class
					int			break_after		< 0 Print with pointer info
												0 Print all into one line 
												> 0 Print linewise
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void ccl_print( FILE* stream, CCL ccl, int break_after )
{
	CCL		i;
	int		cnt;
	uchar	outstr[ 2 ] [ 10 + 1 ];

	if( !stream )
		stream = stderr;

	if( break_after < 0 )
		fprintf( stream, "*** begin of ccl %p ***\n", ccl );

	for( i = ccl, cnt = 0; i && i->begin != CCL_MAX; i++, cnt++ )
	{
		u8_toutf8( outstr[0], sizeof( outstr[0] ), &( i->begin ), 1 );

		if( i->begin != i->end )
		{
			u8_toutf8( outstr[1], sizeof( outstr[1] ), &( i->end ), 1 );
			fprintf( stream, "'%s' [%d] to '%s' [%d] ",
				outstr[0], i->begin, outstr[1], i->end );
		}
		else
			fprintf( stream, "'%s' [%d] ", outstr[0], i->begin );

		if( break_after > 0 && cnt % break_after == 0 )
			fprintf( stream, "\n" );
	}

	if( break_after < 0 )
		fprintf( stream, "*** end of ccl %p ***\n", ccl );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_to_str()
	
	Author:			Jan Max Meyer
	
	Usage:			Converts a character-class back to a string again, which
					in turn can be converted into a character class.
					
	Parameters:		CCL			ccl				Pointer to character-class
												to be converted.
					pboolean	escape			TRUE: Escapes "unprintable"
												characters in their
												hexadecimal representation
												FALSE: Prints all characters,
												except the zero, which
												will be returned as "\0"
	
	Returns:		uchar*						Returns the generated string
												that represents the charclass.
												Must be freed with pfree().
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
uchar* ccl_to_str( CCL ccl, pboolean escape )
{
	CCL		i;
	uchar	from	[ 40 + 1 ];
	uchar	to		[ 20 + 1 ];
	uchar*	ret		= (uchar*)NULL;

	PROC( "ccl_to_str" );
	PARMS( "ccl", "%p", ccl );
	PARMS( "escape", "%d", escape );

	for( i = ccl; i && i->begin != CCL_MAX; i++ )
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

			/* can't use pstring-tools here... they are one level below! */
			sprintf( from + strlen( from ), "-%s", to );
		}

		if( !ret )
		{
			ret = (uchar*)pmalloc( ( strlen( from ) + 1 ) * sizeof( uchar ) );
			*ret = '\0';
		}
		else
			ret = (uchar*)prealloc( (uchar*)ret,
					( strlen( ret ) + strlen( from ) + 1 ) * sizeof( uchar ) );

		if( !ret )
		{
			MSG( "Out of memory?" );
			RETURN( (uchar*)NULL );
		}

		strcat( ret, from );
	}

	VARS( "ret", "%s", ret );
	RETURN( ret ? ret : (uchar*)strdup( "" ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_addrange()
	
	Author:			Jan Max Meyer
	
	Usage:			Integrates a character range into a character-class.
					
	Parameters:		CCL			ccl				Pointer to the character-class
												to be affected.
					wchar		begin			Begin of character range to be
												integrated.
					wchar		end				End of character range to
												be integrated.
	
	Returns:		CCL							Pointer to the enhanced and
												normalized character-class,
												(CCL)NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_addrange( CCL ccl, wchar begin, wchar end )
{
	int		size	= ccl_size( ccl );
	wchar	tmp;

	PROC( "ccl_addrange" );
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
	if( end >= CCL_MAX )
		end = CCL_MAX - 1;

	/* Is something really to do? */
	for( tmp = begin; tmp <= end; tmp++ )
		if( !ccl_test( ccl, tmp ) )
			break;

	if( ccl && tmp > end )
	{
		MSG( "Nothing to do!" );
		RETURN( ccl );
	}

	/* Re-allocate */
	if( !( ccl = (CCL)prealloc( ccl,
			( ccl_size( ccl ) + 1 + 2 )
				* sizeof( CRANGE ) ) ) )
	{
		MSG( "Can't (re)allocate memory" );
		RETURN( (CCL)NULL );
	}

	VARS( "size", "%d", size );
	ccl[ size ].begin = begin;
	ccl[ size ].end = end;

	ccl[ ++size ].begin = CCL_MAX;

	MSG( "Normalizing" );
	RETURN( ccl_normalize( ccl, FALSE ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_delrange()
	
	Author:			Jan Max Meyer
	
	Usage:			Removes a character range from a character-class.
					
	Parameters:		CCL			ccl				Pointer to the character-class
												to be affected.
					wchar		begin			Begin of character range to be
												integrated.
					wchar		end				End of character range to
												be integrated.
	
	Returns:		CCL							Pointer to the enhanced and
												normalized character-class,
												(CCL)NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_delrange( CCL ccl, wchar begin, wchar end )
{
	int		size;
	wchar	tmp;
	CCL		i;

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
		size = ccl_size( ccl );

		for( i = ccl; i && i->begin != CCL_MAX; i++ )
		{
			if( begin <= i->end && end >= i->begin )
			{
				if( begin > i->begin && end < i->end )
				{
					/* Splitting required! */
					tmp = i->end;
					i->end = begin - 1;
					if( !( ccl = ccl_addrange( ccl, end + 1, tmp ) ) )
						return (CCL)NULL;

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
					memcpy( i, &( ccl[ size - 1 ] ), sizeof( CRANGE ) );
					ccl[ size - 1 ].begin = CCL_MAX;
					ccl_normalize( ccl, FALSE );
				}
			}
		}
	}
	while( size != ccl_size( ccl ) );

	return ccl_normalize( ccl, FALSE );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_add()
	
	Author:			Jan Max Meyer
	
	Usage:			Integrates a character into a character-class.
					
	Parameters:		CCL			ccl				Pointer to the character-class
												to be affected.
					wchar		ch				Character to be integrated.
	
	Returns:		CCL							Pointer to the enhanced and
												normalized character-class,
												(CCL)NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_add( CCL ccl, wchar ch )
{
	return ccl_addrange( ccl, ch, ch );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_del()
	
	Author:			Jan Max Meyer
	
	Usage:			Removes one character from a 
					
	Parameters:		CCL			ccl				Pointer to the character-class
												to be affected.
					wchar		ch				Character to be removed from
												the character class.
	
	Returns:		CCL							Pointer to the enhanced and
												normalized character-class,
												(CCL)NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
#if 0
CCL ccl_del( CCL ccl, wchar ch )
{
	CCL		i;
	wchar	size	= ccl_size( ccl );

	if( !size || !ccl_test( ccl, ch ) )
		return ccl;

	for( i = ccl; i && i->begin != CCL_MAX; i++ )
	{
		if( i->begin <= ch && i->end >= ch )
			if( i->begin == i->end )
				memcpy( &( ccl[ size - 1 ] )
	}

	if( !( ccl = (CCL)prealloc( ccl,
			( ccl_size( ccl ) + 1 + 1 )
				* sizeof( CRANGE ) ) ) )
		return (CCL)NULL;

	ccl[ size ].begin = begin;
	ccl[ size ].end = end;

	ccl[ ++size ].begin = CCL_MAX;

	return ccl_normalize( ccl, FALSE );
}
#endif

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_test()
	
	Author:			Jan Max Meyer
	
	Usage:			Tests for a character to matche the character class.
					
	Parameters:		wchar*		ccl				Pointer to character-class
												to be tested
					wchar		ch				Character to be tested
	
	Returns:		pboolean					TRUE: Character matches class
												FALSE: the opposite.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean ccl_test( CCL ccl, wchar ch )
{
	CCL		i;

	for( i = ccl; i && i->begin != CCL_MAX; i++ )
	{
		if( i->begin <= ch && i->end >= ch )
			return TRUE;
	}

	return FALSE;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_compare()
	
	Author:			Jan Max Meyer
	
	Usage:			Checks for differences in two character classes.
					
	Parameters:		CCL			first			Pointer to the first 
												character-class
					CCL			second			Pointer to the second
												character-class
	
	Returns:		int							<0 if first is < second
												0 if first == second
												>0 if first > second
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int ccl_compare( CCL first, CCL second )
{
	CCL		i;
	CCL		j;
	int		ret		= 0;

	PROC( "ccl_compare" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( ( ret = (int)ccl_size( first ) - (int)ccl_size( second ) != 0 ) )
		RETURN( ret );

	for( i = first; i && i->begin != CCL_MAX; i++ )
		for( j = second; j && j->begin != CCL_MAX; j++ )
			if( !( i->begin == j->begin && i->end == j->end ) )
				RETURN( ( ( i->begin > j->begin ) ? 1 : -1 ) );

	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_intersect()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns a new character class with all characters that exist
					in both provided character classes.
					
	Parameters:		CCL			first			Pointer to the first 
												character-class
					CCL			second			Pointer to the second
												character-class
	
	Returns:		CCL			insersections	Returns a new character-class
												that contains all intersecting
												characters.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_intersect( CCL first, CCL second )
{
	wchar	cnt				= 0;
	CCL		i;
	CCL		j;
	CCL		intersections	= (CCL)NULL;
	CRANGE	inter;

	PROC( "ccl_intersect" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	for( i = first; i && i->begin != CCL_MAX; i++ )
		for( j = second; j && j->begin != CCL_MAX; j++ )
			if( j->begin <= i->end && j->end >= i->begin )
			{
				inter.begin = ( i->begin > j->begin ) ? i->begin : j->begin;
				inter.end = ( i->end > j->end ) ? j->end : i->end;
				
				VARS( "intersections", "%p", intersections );
				if( !( intersections = (CCL)prealloc( (CCL)intersections,
						( ccl_size( intersections ) + 1 + 2 )
							* sizeof( CRANGE ) ) ) )
					return (CCL)NULL;
					
				memcpy( &( intersections[cnt++] ), &inter, sizeof( CRANGE ) );
				intersections[cnt].begin = CCL_MAX;
			}

	RETURN( intersections ? ccl_normalize( intersections, TRUE )
				: intersections );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_diff()
	
	Author:			Jan Max Meyer
	
	Usage:			Returns the difference quantity of two character classes.
					All elements from second will be removed from first.
					
	Parameters:		CCL			first			Pointer to the first 
												character-class
					CCL			second			Pointer to the second
												character-class
	
	Returns:		CCL			difference		Returns a pointer to a new
												charset, which is a copy
												of first without the ranges
												contained in second.
												(CCL)NULL in case of memory
												allocation error.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
CCL ccl_diff( CCL first, CCL second )
{
	CCL		i;
	CCL		difference;

	PROC( "ccl_diff" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( !( difference = ccl_dup( first ) ) )
		return (CCL)NULL;

	for( i = second; i && i->begin != CCL_MAX; i++ )
	{
		if( !( difference = ccl_delrange( difference, i->begin, i->end ) ) )
			return (CCL)NULL;
	}

	return difference;
}

