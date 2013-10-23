/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	ccl.c
Author:	Jan Max Meyer
Usage:	Charclass-Handling
----------------------------------------------------------------------------- */

#define pregex_LOCAL
#include "phorward.h"

/* TEMPORARY ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
typedef struct
{
	pchar			begin;
	pchar			end;
} Npregex_cr;

typedef struct
{
	int				min;
	int				max;

	plist*			ranges;
} Npregex_ccl;

#undef PREGEX_CCL_MAX


#define pregex_cr		Npregex_cr
#define pregex_ccl		Npregex_ccl

#define PREGEX_CCL_MAX	NPREGEX_CCL_MAX()

#define pregex_ccl_size Npregex_ccl_size
#define pregex_ccl_count Npregex_ccl_count
#define pregex_ccl_dup Npregex_ccl_dup
#define pregex_ccl_testrange Npregex_ccl_testrange
#define pregex_ccl_addrange Npregex_ccl_addrange
#define pregex_ccl_delrange Npregex_ccl_delrange
#define pregex_ccl_create Npregex_ccl_create
#define pregex_ccl_free Npregex_ccl_free
#define pregex_ccl_print Npregex_ccl_print
#define pregex_ccl_negate Npregex_ccl_negate

/* PROTO */
pregex_ccl* pregex_ccl_create( char* ccldef );

/* Internal */
pchar NPREGEX_CCL_MAX( void )
{
#ifdef UNICODE
	return (pchar)0xffff;
#else
	return (pchar)0xff;
#endif
}

/* END OF TEMPORARY +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


pregex_ccl* pregex_ccl_free( pregex_ccl* ccl )
{

	return (pregex_ccl*)pfree( ccl );
}

pboolean pregex_ccl_compat( pregex_ccl* l, pregex_ccl* r )
{
	if( !( l && r ) )
	{
		WRONGPARAM;
		return FALSE;
	}

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

	if( !ccl )
	{
		WRONGPARAM;
		return (pregex_ccl*)NULL;
	}

	dup = pregex_ccl_create( (char*)NULL );
	dup->ranges = plist_dup( ccl->ranges );

	return dup;
}


/* Sort-function required for quick sort */
static pboolean ccl_SORTFUNC( void* v_r1, void* v_r2 )
{
	pregex_cr*	r1	= (pregex_cr*)v_r1;
	pregex_cr*	r2	= (pregex_cr*)v_r2;

	if( r1->begin < r2->begin )
		return TRUE;

	return FALSE;
}

/* Normalizes a pre-parsed or modified character-class.

Normalization means, that duplicate elements will be removed, the range pairs
are sorted and intersections are resolved. The result is a unique, normalized
character-class to be used for further operations.

//ccl// is the character-class to be normalized.
//mem_opt// specifies if memory optimization should be performed, so that
exactly the required size of memory is used that is exactly holds the character
class.

Returns the pointer //ccl//.
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
		plist_sort( ccl->ranges, ccl_SORTFUNC );

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
pboolean pregex_ccl_testrange( pregex_ccl* ccl, pchar begin, pchar end )
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

/* Internal function without normalization */
static pboolean pregex_ccl_ADDRANGE( pregex_ccl* ccl, pchar begin, pchar end )
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

Returns a pointer to the extended version of //ccl// on success, or
(pregex_cr)NULL on error.
*/
pboolean pregex_ccl_addrange( pregex_ccl* ccl, pchar begin, pchar end )
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

/** Removes a character range from a character-class.

//ccl// is the pointer to the character-class to be affected.
//begin// is the begin of character range to be removed.
//end// is the end of character range to be removed.

Returns a pointer to the modified version of //ccl// on success, or
(pregex_cr)NULL on error.
*/
pboolean pregex_ccl_delrange( pregex_ccl* ccl, pchar begin, pchar end )
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

/** Negates all ranges in a character-class.

//ccl// is the pointer to the character-class to be negated.

Returns a pointer to //ccl//.
*/
pboolean pregex_ccl_negate( pregex_ccl* ccl )
{
	pchar		start;
	pchar		end;
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

/** Parses a character-class definition and returns a normalized character-class
to be used for further operations.

//ccldef// is the character-class definition string, accepting UTF-8 formatted
input.

Returns a pointer to the newly created character-class. This pointer should be
released with pregex_ccl_free() when its existence is no longer required.
*/
pregex_ccl* pregex_ccl_create( char* ccldef )
{
	char*		cclptr;
	pregex_ccl*	ccl;
	pregex_cr	cr;
	pchar		begin;
	pchar		end;
	pchar		swap;
	pchar		size	= 0;

	PROC( "pregex_ccl_create" );
	PARMS( "ccldef", "%s", ccldef );

	ccl = (pregex_ccl*)pmalloc( sizeof( pregex_ccl ) );
	ccl->ranges = plist_create( sizeof( pregex_cr ), PLIST_MOD_RECYCLE );
	ccl->min = 0;
	ccl->max = 255; /* TODO */

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

	RETURN( ccl );
}

void pregex_ccl_print( pregex_ccl* ccl )
{
	plistel*	e;
	pregex_cr*	cr;

	if( !plist_count( ccl->ranges ) )
		fprintf( stderr, "No ranges\n" );

	for( e = plist_first( ccl->ranges ); e; e = plist_next( e ) )
	{
		cr = (pregex_cr*)plist_access( e );
		fprintf( stderr, "%d '%c' - %d '%c'\n",
			cr->begin, (char)cr->begin,
				cr->end, (char)cr->end );
	}


}


int main( int argc, char** argv )
{
	pregex_ccl*	c;

	c = pregex_ccl_create( "A-Za-z0-9" );
	/*pregex_ccl_addrange( c, 0, 10 );*/
	pregex_ccl_print( c );

	pregex_ccl_negate( c );

	/* pregex_ccl_delrange( c, 0, 255 ); */
	pregex_ccl_print( c );

}























#if 0


/** Unions two character-classes into a normalized one.

//first// is the pointer to the first character-class
//second// is the pointer to the second character-class to be unioned with
//first//.

Returns a pointer to //first//, after //first// has been exteded to the
amount of characters from //second//. //second// remains untouched.
*/
pregex_cr pregex_ccl_union( pregex_cr first, pregex_cr second )
{
	int			fsize;
	int			size;
	pregex_cr	ret;

	PROC( "pregex_ccl_union" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( !( first && second ) )
	{
		WRONGPARAM;
		RETURN( (pregex_cr)NULL );
	}

	ret = first;
	size = ( fsize = pregex_ccl_size( first ) ) +
				pregex_ccl_size( second ) + 1;

	if( !( ret = prealloc( (pregex_cr)ret,
			( size  + 2 ) * sizeof( pregex_cr ) ) ) )
	{
		MSG( "Ran out of memory" );
		RETURN( (pregex_cr)NULL );
	}

	memcpy( ret + fsize, second,
		( ( pregex_ccl_size( second ) ) + 1 ) * sizeof( pregex_cr ) );

	pregex_ccl_normalize( ret, FALSE );

	RETURN( ret );
}



/** Print character-class to output stream.
This function is for debug-purposes only.

//stream// is the output stream to dump the character-class to; This can be
left (FILE*)NULL, so stderr will be used.
//ccl// is the pointer to character-class

//break_after// defines:
- if < 0 print with pointer info
- if 0 print all into one line
- if > 0 print linewise
-
*/
void pregex_ccl_print( FILE* stream, pregex_cr ccl, int break_after )
{
	pregex_cr	i;
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

/** Converts a character-class back to a string representation of the
character-class definition, which in turn can be converted back into a
character-class using pregex_ccl_create().

//ccl// is the pointer to character-class to be converted.
//escape//, if TRUE, escapes "unprintable" characters in their hexadecimal
representation. If FALSE, it prints all characters, except the zero, which will
be returned as "\0"

Returns the generated string that represents the charclass. The returned pointer
must be released with pfree() after its existence is no longer required.
*/
char* pregex_ccl_to_str( pregex_cr ccl, pboolean escape )
{
	pregex_cr	i;
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





/** Integrates a character into a character-class.

//ccl// is the pointer to the character-class to be affected. If //ccl// is
provided as (pregex_cr)NULL, it will be created by the function.
//ch// is the character to be integrated.

The function is a shortcut for pregex_ccl_addrange().

Returns a pointer to the extended version of //ccl// on success, or
(pregex_cr)NULL on error.
*/
pregex_cr pregex_ccl_add( pregex_cr ccl, pchar ch )
{
	return pregex_ccl_addrange( ccl, ch, ch );
}

/** Removes a character from a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be removed from //ccl//.

The function is a shortcut for pregex_ccl_delrange().

Returns a pointer to the modified version of //ccl// on success, or
(pregex_cr)NULL on error.
*/
pregex_cr pregex_ccl_del( pregex_cr ccl, pchar ch )
{
	return pregex_ccl_delrange( ccl, ch, ch );
}

/** Tests a character-class if it cointains a character.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

Returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean pregex_ccl_test( pregex_cr ccl, pchar ch )
{
	pregex_cr	i;

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( i->begin <= ch && i->end >= ch )
			return TRUE;
	}

	return FALSE;
}

/** Tests a character-class to match a character range.

//ccl// is a pointer to the character-class to be tested.
//begin// is the begin of character-range to be tested.
//end// is the end of character-range to be tested.

Returns TRUE if the entire character range matches the class, and FALSE if not.
*/
pboolean pregex_ccl_testrange( pregex_cr ccl, pchar begin, pchar end )
{
	pregex_cr	i;

	for( i = ccl; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( begin >= i->begin && end <= i->end )
			return TRUE;
	}

	return FALSE;
}

/** Tests for a case-insensitive character to match a character-class.

//ccl// is the pointer to character-class to be tested,
//ch// is the character to be tested,

Returns TRUE if the character matches the character-class and FALSE if not.
*/
pboolean pregex_ccl_instest( pregex_cr ccl, pchar ch )
{
	if( pregex_ccl_test( ccl, ch ) )
		return TRUE;

	if( iswupper( ch ) )
		ch = towlower( ch );
	else
		ch = towupper( ch );

	return pregex_ccl_test( ccl, ch );
}

/** Checks for differences in two character-classes.

//first// is the pointer to the first character-class.
//second// is the pointer to the second character-class.

Returns a value < 0 if //first// is lower than //second//, 0 if //first// is
equal to //second// or a value > 0 if //first// is greater than //second//.
*/
int pregex_ccl_compare( pregex_cr first, pregex_cr second )
{
	pregex_cr	i;
	pregex_cr	j;
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

/** Returns a new character-class with all characters that exist in both
provided character-classes.

//first// is the pointer to the first character-class.
//second// is the pointer to the second character-class.

Returns a new character-class containing the insersections from //first//
and //second//.
*/
pregex_cr pregex_ccl_intersect( pregex_cr first, pregex_cr second )
{
	psize		cnt				= 0;
	pregex_cr	i;
	pregex_cr	j;
	pregex_cr	intersections	= (pregex_cr)NULL;
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

				if( !( intersections = (pregex_cr)prealloc(
						(pregex_cr)intersections,
							( pregex_ccl_size( intersections ) + 1 + 1 )
								* sizeof( pregex_cr ) ) ) )
				{
					RETURN( (pregex_cr)NULL );
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

/** Returns the difference quantity of two character-classes. All elements from
//second// will be removed from //first//.

//first// is the pointer to the first character-class.
//second// is the pointer to the second character-class.

Returns a new pointer to a copy of //first//, without the ranges contained in
//second//. Returns (pregex_cr)NULL in case of memory allocation or parameter
error.
*/
pregex_cr pregex_ccl_diff( pregex_cr first, pregex_cr second )
{
	pregex_cr		i;
	pregex_cr		difference;

	PROC( "pregex_ccl_diff" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( !( difference = pregex_ccl_dup( first ) ) )
		return (pregex_cr)NULL;

	for( i = second; i && i->begin != PREGEX_CCL_MAX; i++ )
	{
		if( !( difference = pregex_ccl_delrange(
								difference, i->begin, i->end ) ) )
			return (pregex_cr)NULL;
	}

	return difference;
}
#endif
