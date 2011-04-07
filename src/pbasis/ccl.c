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
	Function:		ccl_char()
	
	Author:			Jan Max Meyer
	
	Usage:			Read one character from an UTF-8 input stream.
					This character can be escaped, a UTF-8 character or an
					ordinary ASCII-char.
					
	Parameters:		uchar**		chr				Input-/Output-Pointer
	
	Returns:		u_int						Returns the character code
												of the parsed character.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static u_int ccl_char( uchar** ch )
{
	u_int	ret;

	if( u8_char( *ch ) == (u_int)'\\' )
	{
		(*ch)++;
		(*ch) += u8_read_escape_sequence( *ch, &ret );
	}
	else
	{
		ret = u8_char( *ch );
		(*ch) += u8_seqlen( *ch );
	}

	return ret;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_size()
	
	Author:			Jan Max Meyer
	
	Usage:			Return the number of range pairs within a character-class.
					
	Parameters:		u_int*		ccl				Pointer to the character-class
	
	Returns:		u_int						Number of pairs the charclass
												holds.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
u_int ccl_size( u_int* ccl )
{
	u_int*	i;
	u_int	cnt	= 0;

	for( i = ccl; i && *i != CCL_MAX; i += 2 )
		cnt++;

	return cnt;
}

/* Sort-function for quick sort */
static int ccl_sortfunc( const void* v_r1, const void* v_r2 )
{
	u_int*	r1	= (u_int*)v_r1;
	u_int*	r2	= (u_int*)v_r2;

	if( *r1 == *r2 )
		return 0;
	else if( *r1 < *r2 )
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
					
	Parameters:		u_int*		ccl				Pointer to the character-class
												to be normalized.
					BOOLEAN		mem_opt			Run memory optimization, so
												that only memory is used
												that is exactly holds the
												character class.
												
	
	Returns:		u_int*						Pointer to ccl or to optimized
												character class memory address.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
static u_int* ccl_normalize( u_int* ccl, BOOLEAN mem_opt )
{
	u_int*	nccl;
	u_int*	i;
	u_int*	j;

	PROC( "ccl_normalize" );
	PARMS( "ccl", "%p", ccl );

	/* First sort the ccl! */
	qsort( ccl, ccl_size( ccl ), 2 * sizeof( u_int ), ccl_sortfunc );

	/* Then, find intersections and... */
	for( i = ccl; i && *i != CCL_MAX; i += 2 )
	{
		j = i + 2;

		if( *j != CCL_MAX )
		{
			if( *j <= *(i+1) && *(j+1) >= *i )
			{
				/*
				printf( "overlap! %d %d - %d %d\n",  
					*i, *(i+1), *j, *(j+1) );
				*/

				if( *i > *j )
					*i = *j;
				else
					*j = *i;

				if( *(i+1) > *(j+1) )
					*(j+1) = *(i+1);
				else
					*(i+1) = *(j+1);
			}
		}
	}

	/* ...remove them! */
	for( i = ccl; i && *i != CCL_MAX; i += 2 )
	{
		for( j = i + 2; ; j += 2 )
		{
			if( j == i + 2 )
			{
				if( *j == CCL_MAX || *i != *j )
					break;
			}
			else
			{
				if( ( *( j-2 ) = *j ) != CCL_MAX )
					*( j-1 ) = *( j+1 );
				else
					break;
			}
		}
	}

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
					(u_int*)NULL if no more memory is left.
					
	Parameters:		u_int*		ccl				Pointer to the character-class
												to be duplicated
	
	Returns:		u_int*						Pointer to the duplicate of ccl,
												(u_int*)NULL in error case.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
u_int* ccl_dup( u_int* ccl )
{
	return (u_int*)memdup( ccl, ( ( ccl_size( ccl ) + 1 ) * 2 + 1 )
								* sizeof( u_int ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_negate()
	
	Author:			Jan Max Meyer
	
	Usage:			Negates all ranges in a character class.
					
	Parameters:		u_int*		ccl				Pointer to the character-class
												to be negated.
	
	Returns:		u_int*						Returns a pointer to the same
												class.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
u_int* ccl_negate( u_int* ccl )
{
	u_int	size	= 0;
	u_int	start;
	u_int	end		= CCL_MIN;
	u_int*	i;
	u_int*	j;

	PROC( "ccl_negate" );
	PARMS( "ccl", "%p", ccl );
	
	for( i = j = ccl; i; i += 2, size++ )
	{
		if( *i > end )
		{
			start = *i;
			*j = end;
			end = *(i+1) + 1;
			*(j+1) = start - 1;

			j += 2;

			if( start == CCL_MAX
				|| end == CCL_MAX )
			{
				*j = CCL_MAX;
				break;
			}
		}
		else
			end = *(i+1) + 1;
	}

	RETURN( ccl );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_union()
	
	Author:			Jan Max Meyer
	
	Usage:			Unions two character classes into one normalized one.
					
	Parameters:		u_int*		first			Pointer to the first 
												character-class
					u_int*		second			Pointer to the second
												character-class
					BOOLEAN		freeboth		Frees both character-classes,
												if TRUE. FALSE does not
												free any of the two classes,
												so they remain as given.
	
	Returns:		u_int*						Returns a pointer to the
												unioned character class.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
u_int* ccl_union( u_int* first, u_int* second, BOOLEAN freeboth )
{
	u_int	size;
	u_int*	i;
	u_int*	j;

	PROC( "ccl_union" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );
	PARMS( "freeboth", "%d", freeboth );

	size = ccl_size( first ) + ccl_size( second ) + 1;

	if( !freeboth )
		first = ccl_dup( first );

	if( !( first = prealloc( (u_int*)first,
			( size * 2 + 1 ) * sizeof( u_int ) ) ) )
	{
		MSG( "Ran out of memory" );
		RETURN( (u_int*)NULL );
	}

	memcpy( first + ccl_size( first ) * 2, second,
		( ( ccl_size( second ) ) * 2 + 1 ) * sizeof( u_int ) );

	if( freeboth )
		pfree( second );

	ccl_normalize( first, FALSE );

	RETURN( first );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_create()
	
	Author:			Jan Max Meyer
	
	Usage:			Parses a character-class definition and returns a
					normalized character class to be used for further
					operations.
					
	Parameters:		uchar*		ccldef			character class definition
												string, in UTF-8.
	
	Returns:		u_int*						Returns a pointer to the newly
												created character class.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
u_int* ccl_create( uchar* ccldef )
{
	uchar*	cclptr;
	u_int*	ccl		= (u_int*)NULL;
	BOOLEAN	neg		= FALSE;
	u_int	begin, end, swap;
	u_int	size	= 0;

	PROC( "ccl_create" );
	PARMS( "ccldef", "%s", ccldef );
	
	if( *ccldef == '^' )
	{
		MSG( "Detected negative charclass!" );
		ccldef += u8_seqlen( ccldef );
		neg = TRUE;
	}

	/*
		Run this twice ... first one to obtain the required size
			second one to parse the charclass
	*/
	do
	{
		for( cclptr = ccldef; *cclptr; )
		{
			VARS( "cclptr", "%s", cclptr );

			begin = end = ccl_char( &cclptr );
			VARS( "begin", "%d", begin );
			VARS( "end", "%d", end );

			/* Is this a range def? */
			if( *cclptr == '-' )
			{
				MSG( "This is a range!" );

				cclptr++;
				end = ccl_char( &cclptr );

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
				ccl[size++] = begin;
				ccl[size++] = end;
			}
		}

		if( !ccl )
		{
			/*
				We allocate one extra-pair to avoid reallocation
				at charclass negation
			*/
			if( !( ccl = (u_int*)pmalloc( ( ( size + 1 ) * 2 + 1 )
											* sizeof( u_int ) ) ) )
			{
				MSG( "I think we have a memory problem..." );
				RETURN( ccl );
			}

			size = 0;
		}
		else
			ccl[ size++ ] = CCL_MAX;
	}
	while( !size );

	ccl = ccl_normalize( ccl, TRUE );

	if( neg )
		ccl_negate( ccl );

	RETURN( ccl );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_test()
	
	Author:			Jan Max Meyer
	
	Usage:			Tests for a character to matche the character class.
					
	Parameters:		u_int*		ccl				Pointer to character-class
												to be tested
					u_int		ch				Character to be tested
	
	Returns:		pboolean					TRUE: Character matches class
												FALSE: the opposite.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pboolean ccl_test( u_int* ccl, u_int ch )
{
	u_int*	i;

	for( i = ccl; *i != CCL_MAX; i += 2 )
	{
		if( *i <= ch && *(i+1) >= ch )
			return TRUE;
	}

	return FALSE;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		ccl_print()
	
	Author:			Jan Max Meyer
	
	Usage:			Print character class to output stream
					For Debug-Purposes only!
					
	Parameters:		FILE*		stream			Output stream; Can be
												left (FILE*)NULL, so stderr
												will be used.
					u_int*		ccl				Pointer to character-class
	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void ccl_print( FILE* stream, u_int* ccl )
{
	u_int*	i;
	uchar	outstr[ 2 ] [ 10 + 1 ];

	if( !stream )
		stream = stderr;

	fprintf( stream, "*** begin of ccl %p ***\n", ccl );
	for( i = ccl; *i != CCL_MAX; i += 2 )
	{
		u8_toutf8( outstr[0], sizeof( outstr[0] ), i, 1 );

		if( *i != *(i+1) )
		{
			u8_toutf8( outstr[1], sizeof( outstr[1] ), i+1, 1 );
			fprintf( stream, "'%s' [%d] to '%s' [%d]\n",
				outstr[0], *i, outstr[1], *(i+1) );
		}
		else
			fprintf( stream, "'%s' [%d]\n", outstr[0], *i );
	}
	fprintf( stream, "*** end of ccl %p ***\n", ccl );
}

