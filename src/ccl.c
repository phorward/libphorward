/* -MODULE----------------------------------------------------------------------
Phorward C/C++ Library
Copyright (C) 2006-2019 by Phorward Software Technologies, Jan Max Meyer
https://phorward.info ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.n.

File:	ccl.c
Author:	Jan Max Meyer
Usage:	Character classes
----------------------------------------------------------------------------- */

#include "phorward.h"

/* Prototype */
static pboolean pccl_ADDRANGE( pccl* ccl, wchar_t begin, wchar_t end );

/* Sort-function required for quick sort */
static int ccl_SORTFUNC( parray* arr, void* a, void* b )
{
	return ((pcrange*)b)->begin - ((pcrange*)a)->begin;
}

/** Constructor function to create a new character-class.

//min// and //max// can either be specified as -1, so the configured default
constants PCCL_MIN and PCCL_MAX will be used. The values can also be inverted.

Returns a pointer to the newly created character-class. This pointer should be
released with pccl_free() when its existence is no longer required.
*/
pccl* pccl_create( int min, int max, char* ccldef )
{
	pccl*	ccl;

	if( min < 0 )
		min = PCCL_MIN;
	if( max < 0 )
		max = PCCL_MAX;

	ccl = (pccl*)pmalloc( sizeof( pccl ) );

	parray_init( &ccl->ranges, sizeof( pcrange ), 0 );
	parray_set_sortfn( &ccl->ranges, ccl_SORTFUNC );

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
		pccl_parse( ccl, ccldef, FALSE );

	return ccl;
}

/** Checks if the character-classes //l// and //r// are configured
to be in the same character universe and compatible for operations. */
pboolean pccl_compat( pccl* l, pccl* r )
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
pccl_count() instead.

Returns the number of pairs the charclass holds.
*/
size_t pccl_size( pccl* ccl )
{
	return parray_count( &ccl->ranges );
}

/** Returns the number of characters within a character-class.

//ccl// is a pointer to the character-class to be processed.

Returns the total number of characters the class is holding.
*/
size_t pccl_count( pccl* ccl )
{
	pcrange*	cr;
	size_t		cnt	= 0;

	if( !ccl )
		return 0;

	parray_for( &ccl->ranges, cr )
		cnt += ( cr->end - cr->begin ) + 1;

	return cnt;
}

/** Duplicates a character-class into a new one.

//ccl// is the pointer to the character-class to be duplicated.

Returns a pointer to the duplicate of //ccl//, or (pcrange)NULL
in error case.
*/
pccl* pccl_dup( pccl* ccl )
{
	pccl* 		dup;
	pcrange*	cr;

	/* Create new, empty ccl */
	dup = pccl_create( ccl->min, ccl->max, (char*)NULL );

	parray_reserve( &dup->ranges, parray_count( &ccl->ranges ) );

	/* Copy elements */
	parray_for( &ccl->ranges, cr )
		pccl_ADDRANGE( dup, cr->begin, cr->end );

	return dup;
}

/* Normalizes a pre-parsed or modified character-class.

Normalization means, that duplicate elements will be removed, the range pairs
are sorted and intersections are resolved. The result is a unique, normalized
character-class to be used for further operations.

//ccl// is the character-class to be normalized.

Returns the number of cycles used for normalization.
*/
static size_t pccl_normalize( pccl* ccl )
{
	pcrange*	l;
	pcrange*	r;
	size_t		count;
	size_t		oldcount	= 0;
	size_t		cycles		= 0;

	while( ( count = pccl_size( ccl ) ) != oldcount )
	{
		oldcount = count;

		/* First sort the character ranges */
		parray_sort( &ccl->ranges );

		/* Then, find intersections and... */
		parray_for( &ccl->ranges, l )
		{
			if( ( r = l + 1 ) < (pcrange*)ccl->ranges.top )
			{
				if( r->begin <= l->end && r->end >= l->begin )
				{
					if( r->end > l->end )
						l->end = r->end;

					parray_remove( &ccl->ranges,
							parray_offset( &ccl->ranges, r ), NULL );
					break;
				}
				else if( l->end + 1 == r->begin )
				{
					l->end = r->end;
					parray_remove( &ccl->ranges,
							parray_offset( &ccl->ranges, r ), NULL );
					break;
				}
			}
		}

		cycles++;
	}

	return cycles;
}

/** Tests a character-class to match a character range.

//ccl// is a pointer to the character-class to be tested.
//begin// is the begin of character-range to be tested.
//end// is the end of character-range to be tested.

Returns TRUE if the entire character range matches the class, and FALSE if not.
*/
pboolean pccl_testrange( pccl* ccl, wchar_t begin, wchar_t end )
{
	pcrange*	cr;

	parray_for( &ccl->ranges, cr )
		if( begin >= cr->begin && end <= cr->end )
			return TRUE;

	return FALSE;
}

/** Tests a character-class if it contains a character.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

The function is a shortcut for pccl_testrange().

It returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean pccl_test( pccl* ccl, wchar_t ch )
{
	return pccl_testrange( ccl, ch, ch );
}

/** Tests for a character in case-insensitive-mode if it matches
a character-class.

//ccl// is the pointer to character-class to be tested.
//ch// is the character to be tested.

The function is a shortcut for pccl_testrange().

It returns TRUE, if the character matches the class, and FALSE if not.
*/
pboolean pccl_instest( pccl* ccl, wchar_t ch )
{
	if( pccl_test( ccl, ch ) )
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

	return pccl_test( ccl, ch );
}

/* Internal function without normalization */
static pboolean pccl_ADDRANGE( pccl* ccl, wchar_t begin, wchar_t end )
{
	pcrange		cr;

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

	if( pccl_testrange( ccl, cr.begin, cr.end ) )
		/* Range already in character-class */
		return TRUE;

	if( cr.begin > ccl->max || cr.end < ccl->min )
		/* Character-range not in the universe of character-class */
		return FALSE;

	return parray_push( &ccl->ranges, &cr ) ? TRUE : FALSE;
}

/** Integrates a character range into a character-class.

//ccl// is the pointer to the character-class to be affected. If //ccl// is
provided as (pccl*)NULL, it will be created by the function.

//begin// is the begin of character range to be integrated.
//end// is the end of character range to be integrated.

If //begin// is greater than //end//, the values will be swapped.
*/
pboolean pccl_addrange( pccl* ccl, wchar_t begin, wchar_t end )
{
	if( !pccl_ADDRANGE( ccl, begin, end ) )
		return FALSE;

	pccl_normalize( ccl );
	return TRUE;
}


/** Integrates a single character into a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be integrated.

The function is a shortcut for pccl_addrange().
*/
pboolean pccl_add( pccl* ccl, wchar_t ch )
{
	return pccl_addrange( ccl, ch, ch );
}

/** Removes a character range from a character-class.

//ccl// is the pointer to the character-class to be affected.
//begin// is the begin of character range to be removed.
//end// is the end of character range to be removed.
*/
pboolean pccl_delrange( pccl* ccl, wchar_t begin, wchar_t end )
{
	pcrange		d;
	pcrange*	r;

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
		parray_for( &ccl->ranges, r )
		{
			if( d.begin <= r->end && end >= r->begin )
			{
				/* Slitting required? */
				if( d.begin > r->begin && d.end < r->end )
				{
					/* Split current range */
					end = r->end;
					r->end = d.begin - 1;

					if( !pccl_addrange( ccl, d.end + 1, end ) )
						return FALSE;

					break;
				}
				/* Move end of current range */
				else if( d.begin > r->begin )
					r->end = d.begin - 1;
				/* Change begin of current range */
				else if( d.end < r->end )
					r->begin = d.end + 1;
				/* Remove entire range */
				else
				{
					parray_remove( &ccl->ranges,
							parray_offset( &ccl->ranges, r ), NULL );

					r = parray_first( &ccl->ranges );
					break;
				}
			}
		}
	}
	while( r );

	pccl_normalize( ccl );
	return TRUE;
}

/** Removes a character from a character-class.

//ccl// is the pointer to the character-class to be affected.
//ch// is the character to be removed from //ccl//.

The function is a shortcut for pccl_delrange().
*/
pboolean pccl_del( pccl* ccl, wchar_t ch )
{
	return pccl_delrange( ccl, ch, ch );
}

/** Negates all ranges in a character-class.

//ccl// is the pointer to the character-class to be negated.

Returns a pointer to //ccl//.
*/
pccl* pccl_negate( pccl* ccl )
{
	wchar_t		start;
	wchar_t		end;
	pcrange*	r;

	start = end = ccl->min;

	do
	{
		parray_for( &ccl->ranges, r )
		{
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
				parray_remove( &ccl->ranges,
						parray_offset( &ccl->ranges, r ), NULL );
				break;
			}
		}
	}
	while( r );

	if( end < ccl->max )
		pccl_addrange( ccl, end, ccl->max );

	pccl_normalize( ccl );

	return ccl;
}

/** Creates the union of two character-classes and returns the newly created,
normalized character-class.

//ccl// is the pointer to the character-class that will be extended to all
ranges contained in //add//. //add// is the character-class that will be joined
with //ccl//.

The function creates and returns a new character-class that is the union
of //ccl// and //add//.
*/
pccl* pccl_union( pccl* ccl, pccl* add )
{
	pccl*		un;
	pcrange*	r;

	if( !pccl_compat( ccl, add ) )
		/* Incompatible character-classes */
		return NULL;

	un = pccl_dup( ccl );

	parray_for( &add->ranges, r )
		if( !pccl_ADDRANGE( un, r->begin, r->end ) )
			return NULL;

	pccl_normalize( un );

	return un;
}


/** Returns the difference quantity of two character-classes.
All elements from //rem// will be removed from //ccl//, and put into a
new character-class.

//ccl// is the pointer to the first character-class.
//rem// is the pointer to the second character-class.

Returns a new pointer to a copy of //ccl//, without the ranges contained in
//rem//. Returns (pccl*)NULL in case of memory allocation or parameter
error.
*/
pccl* pccl_diff( pccl* ccl, pccl* rem )
{
	pcrange*	r;
	pccl*		diff;

	if( !pccl_compat( ccl, rem ) )
		/* Incompatible character-classes */
		return NULL;

	if( !( diff = pccl_dup( ccl ) ) )
		return diff;

	parray_for( &rem->ranges, r )
		pccl_delrange( diff, r->begin, r->end );

	return diff;
}

/** Checks for differences in two character-classes.

//left// is the pointer to the first character-class.
//right// is the pointer to the second character-class.

Returns a value < 0 if //left// is lower than //right//, 0 if //left// is
equal to //right// or a value > 0 if //left// is greater than //right//.
*/
int pccl_compare( pccl* left, pccl* right )
{
	size_t	ret;

	if( !pccl_compat( left, right ) )
		/* Incompatible character-classes */
		return left->max - right->max;

	if( ( ret = pccl_size( left ) - pccl_size( right ) ) != 0 )
		/* Unequal number of range pairs */
		return ret < 0 ? -1 : 1;

	return parray_diff( &left->ranges, &right->ranges );
}

/** Returns a new character-class with all characters that exist in both
provided character-classes.

//ccl// is the pointer to the first character-class.
//within// is the pointer to the second character-class.

Returns a new character-class containing the intersection of //ccl//
and //within//. If there is no intersection between both character-classes,
the function returns (pccl*)NULL.
*/
pccl* pccl_intersect( pccl* ccl, pccl* within )
{
	pcrange*	r;
	pcrange*	s;
	pccl*		in	= (pccl*)NULL;

	if( !pccl_compat( ccl, within ) )
		/* Incompatible character-classes */
		return NULL;

	parray_for( &ccl->ranges, r )
	{
		parray_for( &within->ranges, s )
		{
			if( s->begin <= r->end && s->end >= r->begin )
			{
				if( !in )
					in = pccl_create( ccl->min, ccl->max, (char*)NULL );

				pccl_addrange( in,
					( r->begin > s->begin ) ? r->begin : s->begin,
					( r->end > s->end ) ? s->end : r->end );
			}
		}
	}

	if( in )
		pccl_normalize( in );

	return in;
}

/** Return a character or a character-range by its offset.

If the function is called only with pointer //from// provided, and //to// as
(wchar_t*)NULL, it writes the character in //offset//th position of the
character-class into from.

If the function is called both with pointer //from// and //to// provided,
it writes the //begin// and //end// character of the character-range in the
//offset//th position of the character-class into //from// and //to//.

If no character or range with the given offset was found, the function
returns FALSE, meaning that the end of the characters is reached.
On success, the function will always return TRUE. */
pboolean pccl_get( wchar_t* from, wchar_t* to, pccl* ccl, size_t offset )
{
	pcrange*	cr;

	if( !to )
	{
		/* Single-character retrieval */

		parray_for( &ccl->ranges, cr )
		{
			if( offset >= ( cr->end - cr->begin ) + 1 )
			{
				/* Offset not in this range */
				offset -= ( cr->end - cr->begin ) + 1;
			}
			else if( offset < ( cr->end - cr->begin ) + 1 )
			{
				/* Offset is within this class */
				*from = cr->begin + offset;
				return TRUE;
			}
		}
	}
	else
	{
		/* Range retrieval */

		if( ( cr = (pcrange*)parray_get( &ccl->ranges, offset ) ) )
		{
			*from = cr->begin;
			*to = cr->end;
			return TRUE;
		}
	}

	/* Offset not available */
	return FALSE;
}

/** Reads a character from a string. The character may consist of one single
character or it may be made up of an escape sequence or UTF-8 character.
The function returns the number of bytes read.

//retc// is the return pointer for the character code of the escaped string.
//str// is the begin pointer of the string at which character parsing begins.
If //escapeseq// is TRUE, the function regards escape sequences, else it ignores
them.

Returns the number of bytes that had been read for the character. */
size_t pccl_parsechar( wchar_t* retc, char *str, pboolean escapeseq )
{
	wchar_t	ch;
	char* 	esc;
    char 	digs[9]		=	"\0\0\0\0\0\0\0\0";
    int		dno 		= 0;
	char*	p			= str;

	if( escapeseq && *p == '\\' )
	{
		p++;

		if( *p >= 'a' &&  *p <= 'z'
			&& ( esc = strchr( "n\nt\tr\rb\bf\fv\va\a", *p ) ) )
		{
			ch = *( esc + 1 );
			p++;
		}
		else if( octal_digit( *p ) )
		{
			do
				digs[dno++] = *( p++ );
			while( octal_digit( *p ) && dno < 3 );

			ch = strtol( digs, (char**)NULL, 8 );
		}
		else if( *p == 'x' )
		{
			p++;
			while( hex_digit( *p ) && dno < 2 )
				digs[ dno++ ] = *( p++ );

			if (dno > 0)
				ch = strtol( digs, (char**)NULL, 16 );
		}
#ifdef UTF8
		else if( *p == 'u' )
		{
			p++;
			while( hex_digit( *p ) && dno < 4 )
				digs[dno++] = *( p++ );

			if( dno > 0 )
				ch = strtol( digs, (char**)NULL, 16 );
		}
		else if( *p == 'U' )
		{
			p++;
			while( hex_digit( *p ) && dno < 8 )
				digs[dno++] = *( p++ );

			if( dno > 0 )
				ch = strtol( digs, (char**)NULL, 16 );
		}
#endif
		else
		{
#ifdef UTF8
			ch = putf8_char( p );
			p += putf8_seqlen( p );
#else
			ch = *( p++ );
#endif
		}
	}
	else
	{
#ifdef UTF8
		ch = putf8_char( p );
		p += putf8_seqlen( p );
#else
		ch = *( p++ );
#endif
	}

	*retc = ch;

    return (size_t)( p - str );
}

/**  Tries to parse a shorthand sequence from a string. This matches the
shorthands \w, \W, \d, \D, \s and \S. If it matches, all characters are
added to //ccl//.

The function returns TRUE in case a shorthand has been parsed. If so,
the pointer //str// is moved the characters consumed.

If no shorthand sequence could be found, it returns FALSE, leaving //ccl//
untouched. */
pboolean pccl_parseshorthand( pccl* ccl, char** str )
{
	pccl*		sh;
	pboolean	neg	= FALSE;
	int			i;
	wchar_t		begin;
	wchar_t		end;

	/* Check for shorthand */
	if( **str != '\\' )
		return FALSE;

	switch( *(*str + 1) )
	{
		/* This solution is ugly and does not support any Unicode features.
			So it would be nice to find out a cooler solution in future. */
		case 'D':
			neg = TRUE;
		case 'd':
			sh = pccl_create( ccl->min, ccl->max, "0-9" );
			break;

		case 'W':
			neg = TRUE;
		case 'w':
			sh = pccl_create( ccl->min, ccl->max, "a-zA-Z_0-9" );
			break;

		case 'S':
			neg = TRUE;
		case 's':
			sh = pccl_create( ccl->min, ccl->max, " \f\n\r\t\v\a" );
			break;

		default:
			/* This is not a supported shorthand! */
			return FALSE;
	}

	if( neg )
		pccl_negate( sh );

	for( i = 0; pccl_get( &begin, &end, sh, i ); i++ )
		pccl_ADDRANGE( ccl, begin, end );

	pccl_free( sh );
	pccl_normalize( ccl );

	*str += 2;
	return TRUE;
}

/** Parses the character-class definition provided in //ccldef// and assigns
this definition to the character-class //ccl//.

If //ccl// is NULL, a new character-class with the PCCL_MIN/PCCL_MAX
configuration will be created.

//ccldef// may contain UTF-8 formatted input. Escape-sequences will be
 interpreted to their correct character representations.

A typical character-class definition simply exists of single characters and
range definitions. For example, "$A-Z#0-9" defines a character-class that
consists of the characters "$#0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ".

The parameter //extend// specifies, if the provided character-class overwrites
(//extend// = FALSE) or extends (//extend// = TRUE) the provided
character-class. This means that definitions that already exist in the
character-class, should be erased first or not.

The function returns TRUE on success, and FALSE on an error.
*/
pccl* pccl_parse( pccl* ccl, char* ccldef, pboolean extend )
{
	pccl*		own;
	char*		cclptr;
	wchar_t		begin;
	wchar_t		end;

	if( !ccl )
		ccl = own = pccl_create( 0, 0, NULL );
	else if( !extend )
		pccl_erase( ccl );

	for( cclptr = ccldef; *cclptr; )
	{
		if( pccl_parseshorthand( ccl, &cclptr ) )
			continue;

		cclptr += pccl_parsechar( &begin, cclptr, TRUE );
		end = begin;

		/* Is this a range def? */
		if( *cclptr == '-' )
		{
			cclptr++;
			cclptr += pccl_parsechar( &end, cclptr, TRUE );
		}

		if( !pccl_ADDRANGE( ccl, begin, end ) )
		{
			if( own )
				pccl_free( own );

			return NULL;
		}
	}

	pccl_normalize( ccl );

	return ccl;
}

/** Erases a character-class //ccl//.

The function sets a character-class to zero, as it contains no character range
definitions. The object //ccl// will be still alive. To delete the entire
object, use pccl_free().
*/
void pccl_erase( pccl* ccl )
{
	parray_erase( &ccl->ranges );
	ccl->str = pfree( ccl->str );
}

/** Frees a character-class //ccl// and all its used memory.

The function always returns (pccl*)NULL.
*/
pccl* pccl_free( pccl* ccl )
{
	if( !ccl )
		return (pccl*)NULL;

	pccl_erase( ccl );
	pfree( ccl );

	return (pccl*)NULL;
}

/** Converts a character-class back to a string representation of the
character-class definition, which in turn can be converted back into a
character-class using pccl_create().

//ccl// is the pointer to character-class to be converted.
//escape//, if TRUE, escapes "unprintable" characters in their hexadecimal
representation. If FALSE, it prints all characters, except the zero, which will
be returned as "\0"

Returns a pointer to the generated string that represents the charclass.
The returned pointer belongs to the //ccl// and is managed by the
character-class handling functions, so it should not be freed manually.
*/
char* pccl_to_str( pccl* ccl, pboolean escape )
{
	size_t		len		= 1;
	pcrange*	r;

	ccl->str = pfree( ccl->str );

	/* Examine required size of the resulting string */
	parray_for( &ccl->ranges, r )
		len += ( ( r->begin != r->end ? 1 : 2 )			/* Two or one char? */
					* ( escape ? 10 : 6 ) ) 			/* Escaped or not? */
				+ ( ( r->begin != r->end ) ? 1 : 0 );	/* Range-dash? */

	if( !( ccl->str = (char*)pmalloc( len * sizeof( char ) ) ) )
		return NULL;

	*ccl->str = '\0';

	parray_for( &ccl->ranges, r )
	{
		if( escape )
			putf8_escape_wchar(
					ccl->str + strlen( ccl->str ),
						len - strlen( ccl->str ) - 1, r->begin );
		else
			putf8_toutf8( ccl->str + strlen( ccl->str ),
					len - strlen( ccl->str ) - 1,
						&( r->begin ), 1 );

		if( r->begin != r->end )
		{
			strcat( ccl->str, "-" );

			if( escape )
				putf8_escape_wchar(
						ccl->str + strlen( ccl->str ),
							len - strlen( ccl->str ) - 1, r->end );
			else
				putf8_toutf8( ccl->str + strlen( ccl->str ),
							  len - strlen( ccl->str ) - 1,
							  &( r->end ), 1 );
		}
	}

	return ccl->str ? ccl->str : "";
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
void pccl_print( FILE* stream, pccl* ccl, int break_after )
{
	pcrange*	r;
	int			cnt			= 0;
	char		outstr[ 2 ] [ 10 + 1 ];

	if( !( ccl ) )
		return;

	if( !stream )
		stream = stderr;

	if( break_after < 0 )
		fprintf( stream, "*** begin of ccl %p ***\n", ccl );

	parray_for( &ccl->ranges, r )
	{
		putf8_toutf8( outstr[0], sizeof( outstr[0] ), &( r->begin ), 1 );

		if( r->begin != r->end )
		{
			putf8_toutf8( outstr[1], sizeof( outstr[1] ), &( r->end ), 1 );
			fprintf( stream, "'%s' [%d] to '%s' [%d] ",
				outstr[0], (int)r->begin, outstr[1], (int)r->end );
		}
		else
			fprintf( stream, "'%s' [%d] ", outstr[0], (int)r->begin );

		if( break_after > 0 && cnt++ % break_after == 0 )
			fprintf( stream, "\n" );
	}

	if( break_after < 0 )
		fprintf( stream, "\n*** end of ccl %p ***\n", ccl );
}

#if 0
int main( int argc, char** argv )
{
	pccl*	c;
	pccl*	d;
	pccl*	e;

	c = pccl_create( 0, 255, "A-Za-z0-9" );
	d = pccl_create( 0, 255, "A-@." );

	/*pccl_addrange( c, 0, 10 );*/
	pccl_print( NULL, c, 1 );
	pccl_print( NULL, d, 1 );

	e = pccl_union( c, d );
	pccl_print( NULL, e, 1 );

	printf( "%s\n", pccl_to_str( e, TRUE ) );

	/*
	pccl_negate( c );
	pccl_delrange( c, 0, 255 );
	pccl_print( c );
	*/

	return 0;
}
#endif

/*TESTCASE
#include <phorward.h>

void testcase()
{
	pccl*		c;
	pccl*		d;
	pccl*		e;
	char* 		x;

	c = pccl_create( PCCL_MIN, PCCL_MAX, "^ €A-Z\n" );
	d = pccl_create( PCCL_MIN, PCCL_MAX, "A-NXYZ\n" );

	d = pccl_create( PCCL_MIN, PCCL_MAX,
					 "^alles richtig! :)" );
	pccl_print( stderr, c, 0 );
	pccl_print( stderr, d, 0 );

	e = pccl_union( c, d );
	pccl_print( stderr, e, 0 );

	pccl_negate( e );
	pccl_print( stderr, e, 0 );

	pccl_negate( e );
	pccl_print( stderr, e, 0 );

	pccl_delrange( d, '\0', PCCL_MAX );
	printf( "e = >%s<\n", pccl_to_str( d, TRUE ) );

	pccl_free( c );
	pccl_free( d );
	pccl_free( e );
}
---
todo
*/
