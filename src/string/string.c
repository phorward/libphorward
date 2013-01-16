/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2012 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	string.c
Author:	Jan Max Meyer
Usage:	Some extended functions for zero-terminated byte- and wide-character
		strings that can be universally used. The library was build from
		several UniCC and RapidBATCH 6 functions, and is now assembled into
		the pbasis-library now.
----------------------------------------------------------------------------- */

#include <phorward.h>

#define MAX_SIZE		64
#define MALLOC_STEP		32

/** Dynamically appends a character to a string.

//str// is the pointer to a string to be appended. If this is (uchar*)NULL,
the string will be newly allocated. //chr// is the the character to be appended
to str.

Returns a uchar*-pointer to the (possibly re-)allocated and appended string.
(uchar*)NULL is returned if no memory could be (re)allocated. This pointer must
be released with pfree() when its existence is no longer required.
*/
uchar* pstrcatchar( uchar* str, wchar chr )
{
	PROC( "pstrcatchar" );
	PARMS( "str", "%p", str );
	PARMS( "chr", "%d", chr );

	if( !chr )
		return str;

	if( !str )
	{
		MSG( "Allocating new string" );
		str = (uchar*)pmalloc( ( 1 + 1 ) * sizeof( uchar ) );

		if( str )
			*str = '\0';
	}
	else
	{
		MSG( "Reallocating existing string" );
		str = (uchar*)prealloc( (uchar*)str,
				( pstrlen( str ) + 1 + 1) * sizeof( uchar ) );
	}

	VARS( "str", "%p", str );
	if( !str )
	{
		MSG( "Pointer is null, critical error" );
		exit( 1 );
	}

	sprintf( str + pstrlen( str ), "%c", (int)chr );

	RETURN( str );
}

/** Dynamically appends a zero-terminated string to a dynamic string.

//str// is the pointer to a zero-terminated string to be appended.
If this is (uchar*)NULL, the string is newly allocated.

//append// is the string to be appended at the end of //str//.

//release_append// frees the pointer provided as //append// automatically by
this function, if set to TRUE. This parameter has only a comfort-function.

Returns a uchar*-pointer to (possibly re-)allocated and appended string.
(uchar*)NULL is returned if no memory could be (re)allocated, or both strings
where NULL. If //dest// is NULL and //freesrc// is FALSE, the function
automatically returns the pointer //src//. This pointer must be released with
pfree() when its existence is no longer required.
*/
uchar* pstrcatstr( uchar* dest, uchar* src, boolean freesrc )
{
	PROC( "pstrcatstr" );
	PARMS( "dest", "%p", dest );
	PARMS( "src", "%p", src );
	PARMS( "freesrc", "%d", freesrc );

	if( src )
	{
		if( !dest )
		{
			if( freesrc )
			{
				dest = src;
				freesrc = FALSE;
			}
			else
				dest = pstrdup( src );
		}
		else
		{
			dest = (uchar*)prealloc( (uchar*)dest,
					( pstrlen( dest ) + pstrlen( src ) + 1 )
						* sizeof( uchar ) );
			strcat( dest, src );
		}

		if( freesrc )
			pfree( src );
	}

	RETURN( dest );
}

/** Dynamicaly appends a number of n-characters from one string to another
string.

The function works similar to pstrcatstr(), but allows to copy only a maximum
of //n// characters from //append//.

//str// is the pointer to a string to be appended. If this is (uchar*)NULL,
the string is newly allocated. //append// is the begin of character sequence to
be appended. //n// is the number of characters to be appended to //str//.

Returns a uchar*-pointer to (possibly re-)allocated and appended string.
(uchar*)NULL is returned if no memory could be (re)allocated, or both strings
where NULL. This pointer must be released with pfree() when its existence
is no longer required.
*/
uchar* pstrncatstr( uchar* str, uchar* append, psize n )
{
	psize	len		= 0;

	PROC( "pstrncatstr" );
	PARMS( "str", "%p", str );
	PARMS( "append", "%p", append );
	PARMS( "n", "%d", n );

	if( append )
	{
		if( !str )
		{
			if( !( str = (uchar*)pmalloc( ( n + 1 ) * sizeof( uchar ) ) ) )
				RETURN( (uchar*)NULL );
		}
		else
		{
			len = pstrlen( str );

			if( !( str = (uchar*)prealloc( (uchar*)str,
					( len + n + 1 ) * sizeof( uchar ) ) ) )
				RETURN( (uchar*)NULL );
		}

		strncpy( str + len, append, n );
		str[ len + n ] = '\0';
	}

	RETURN( str );
}

/** Replace a substring sequence within a string.

//str// is the string to be replaced in. //find// is the substring to be
matched. //replace// is the the string to be inserted for each match of the
substring //find//.

Returns a pointer to uchar* containing the allocated string which is the
resulting source. This pointer must be released with pfree() when its existence
is no longer required.
*/
uchar* pstrreplace( uchar* str, uchar* find, uchar* replace )
{
	uchar*			match;
	uchar*			str_ptr			= str;
	uchar*			result			= (uchar*)NULL;
	uchar*			result_end		= (uchar*)NULL;
	unsigned long	len;
	unsigned long	rlen;
	unsigned long	size			= 0L;

	PROC( "pstrreplace" );
	PARMS( "str", "%s", str );
	PARMS( "find", "%s", find );
	PARMS( "replace", "%s", replace );

	len = pstrlen( find );
	rlen = pstrlen( replace );

	while( 1 )
	{
		VARS( "str_ptr", "%s", str_ptr );
		if( !( match = pstrstr( str_ptr, find ) ) )
		{
			size = 0;
			match = str_ptr + pstrlen( str_ptr );
		}
		else
			size = rlen;

		size += (unsigned long)( match - str_ptr );

		VARS( "size", "%ld", size );
		VARS( "match", "%s", match );

		if( !result )
			result = result_end = (uchar*)pmalloc(
				( size + 1 ) * sizeof( uchar ) );
		else
		{
			result = (uchar*)prealloc( (uchar*)result,
				( result_end - result + size + 1 ) * sizeof( uchar ) );
			result_end = result + pstrlen( result );
		}

		if( !result )
		{
			MSG( "Ran out of memory!" );
			exit( 1 );
		}

		pstrncpy( result_end, str_ptr, match - str_ptr );
		result_end += match - str_ptr;
		*result_end = '\0';

		VARS( "result", "%s", result );

		if( !*match )
			break;

		strcat( result_end, replace );
		VARS( "result", "%s", result );

		result_end += rlen;
		str_ptr = match + len;
	}

	RETURN( result );
}

/** Duplicate a string in memory.

//str// is the string to be copied in memory. If //str// is provided as NULL,
the function will also return NULL.

Returns a pchar*-pointer to the newly allocated copy of //str//. This pointer
must be released with pfree() when its existence is no longer required.
*/
uchar* pstrdup( uchar* str )
{
	if( !str )
		return (uchar*)NULL;

	return (uchar*)pmemdup( str, ( pstrlen( str ) + 1 ) * sizeof( uchar ) );
}

/** Duplicate //n// characters from a string in memory.

The function mixes the functionalities of strdup() and strncpy().
The resulting string will be zero-terminated.

//str// is the parameter string to be duplicated. If this is provided as
(uchar*)NULL, the function will also return (uchar*)NULL.
//n// is the the number of characters to be copied and duplicated from //str//.
If //n// is greater than the length of //str//, copying will stop at the zero
terminator.

Returns a uchar*-pointer to the allocated memory holding the zero-terminated
string duplicate. This pointer must be released with pfree() when its existence
is no longer required.
*/
uchar* pstrndup( uchar* str, psize len )
{
	uchar*	ret;

	if( !str )
		return (uchar*)NULL;

	if( pstrlen( str ) < len )
		len = pstrlen( str );

	ret = (uchar*)pmalloc( ( len + 1 ) * sizeof( uchar ) );
	pstrncpy( ret, str, len );
	ret[ len ] = '\0';

	return ret;
}

/** Return length of a string.

//str// is the parameter string to be evaluated. If (pchar*)NULL, the function
returns 0. pstrlen() is much more saver than strlen() because it returns 0 when
a NULL-pointer is provided.

Returns the length of the string //str//.
*/
psize pstrlen( uchar* str )
{
	if( !str )
		return 0;

	return strlen( (char*)str );
}

/*REMOVE!*/
/** Replacement for sprintf.

//res// is the result string
//fmt// is the format string
//...// are the parameters according to the placeholders set in //fmt//.

Returns a the number of characters written, -1 in error case.
*/
int psprintf( uchar* res, uchar* fmt, ... )
{
	int ret;
	va_list	args;

	PROC( "psprintf" );
	PARMS( "res", "%p", res );
	PARMS( "fmt", "%s", fmt );

	if( !( res && fmt ) )
		RETURN( -1 );

	va_start( args, fmt );

	ret = vsprintf( res, fmt, args );

	va_end( args );

	VARS( "ret", "%d", ret );
	RETURN( ret );
}

/*REMOVE?*/
/** Implementation and replacement for vasprintf.

//str// is the pointer receiving the resultung, allocated string pointer.
//fmt// is the the format string.
//...// are the parameters according to the placeholders set in //fmt//.

Returns the number of characters written.
*/
int pvasprintf( uchar** str, uchar* fmt, va_list ap )
{
	uchar*		istr;
	int			ilen;
	int			len;
	va_list		w_ap;

	PROC( "pvasprintf" );
	PARMS( "str", "%p", str );
	PARMS( "fmt", "%s", fmt );
	PARMS( "ap", "%p", ap );

	if( !( istr = (uchar*)pmalloc( MALLOC_STEP * sizeof( uchar ) ) ) )
		RETURN( ERR_MEM );

	va_copy( w_ap, ap );

	MSG( "Invoking vsnprintf() for the first time" );
	len = vsnprintf( istr, MALLOC_STEP, fmt, w_ap );
	VARS( "len", "%d", len );

	if( len >= 0 && len < MALLOC_STEP )
		*str = istr;
	else if( len == INT_MAX || len < 0 )
	{
		MSG( "ret is negative or too big - can't handle this!" );
		va_end( w_ap );
		RETURN( ERR_MEM );
	}
	else
	{
		if( !( istr = prealloc( istr, ( ilen = len + 1 ) * sizeof( uchar ) ) ) )
		{
			va_end( w_ap );
			RETURN( ERR_MEM );
		}

		va_end( w_ap );
		va_copy( w_ap, ap );

		MSG( "Invoking vsnprintf() for the second time" );
		len = vsnprintf( istr, ilen, fmt, w_ap );
		VARS( "len", "%d", len );

		if( len >= 0 && len < ilen )
			*str = istr;
		else
		{
			pfree( istr );
			RETURN( ERR_MEM );
		}
	}

	va_end( w_ap );
	RETURN( len );
}

/*REMOVE?*/
/** Implementation and replacement for asprintf. pasprintf() takes only the
format-string and various arguments. It outputs an allocated string to be freed
later on.

//fmt// is the format string.
//...// are the parameters according to the placeholders set in //fmt//.

Returns a uchar* Returns the allocated string which cointains the format string with inserted values.
*/
uchar* pasprintf( uchar* fmt, ... )
{
	uchar*	str;
	va_list	args;

	PROC( "pasprintf" );
	PARMS( "fmt", "%s", fmt );

	if( !( fmt ) )
		RETURN( (uchar*)NULL );

	va_start( args, fmt );
	pvasprintf( &str, fmt, args );
	va_end( args );

	VARS( "str", "%s", str );
	RETURN( str );
}


/** Assign a string into a dynamically allocated pointer.

psetstr() manages the assignment of an dynamically allocated string.

//str// is a pointer receiving the target pointer to be (re)allocated. If
//str// already references a string, this pointer will be freed and reassigned
to a copy of //val//.

//val// is the the string to be assigned to //str// (as a independent copy).

Returns a pointer to the allocated heap memory on success, (uchar*)NULL else.
This is the same pointer as returned like calling ``*str``. The returned pointer
must be released with pfree() or another call of psetstr(). Calling psetstr()
as ``psetstr( &p, (uchar*)NULL );`` is equivalent to ``p = pfree( &p )``.
*/
uchar* psetstr( uchar** str, uchar* val )
{
	if( *str )
	{
		if( pstrcmp( *str, val ) == 0 )
			return *str;

		pfree( *str );
	}

	*str = pstrdup( val );
	return *str;
}

/** Savely reads a string.

//str// is the string pointer to be savely read. If //str// is NULL, the
function returns a pointer to a static address holding an empty string.
*/
uchar* pgetstr( uchar* str )
{
	if( !str )
		return "";

	return str;
}

/*REMOVE!*/
/** Convert string to long.

The function is a saver replacement for atol() 

//str// is the parameter string to be converted to long.

Returns a long Same as atol, 0L in case of (uchar*)NULL.
*/
long patol( uchar* str )
{
	if( !str )
		return 0L;

	return atol( str );
}

/*REMOVE!*/
/** Convert string to double.

//str// is the parameter string to be converted to double.

Returns a double same as atol, 0.0 in case of (uchar*)NULL.
*/

double patof( uchar* str )
{
	if( !str )
		return (double)0.0;

	return atof( str );
}

/*REMOVE!*/
/** Convert string to integer.

//str// is the parameter string to be converted to int.

Returns a integer same as atoi, 0 in case of (uchar*)NULL.
*/
int patoi( uchar* str )
{
	if( !str )
		return 0;

	return atoi( str );
}

/** String rendering function.

Inserts multiple values dynamically into the according wildcards positions of
a template string. The function can be compared to the function of
pstrreplace(), but allows to replace multiple substrings by multiple replacement
strings.

//tpl// is the template string to be rendered with values.
//...// are the set of values to be inserted into the desired position;

These must be always consist of three values each:

- //uchar* name// as a wildcard-name
- //uchar* value// as the replacement value for the wildcard
- //pboolean freeflag// defines if //value// shall be freed after processing
-

Returns an allocated string which is the resulting source. This string must be
release by pfree() or another function releasing heap memory when its existence
is no longer required.
*/
uchar* pstrrender( uchar* tpl, ... )
{
	struct
	{
		uchar*	wildcard;
		uchar*	value;
		BOOLEAN	clear;
		uchar*	_match;
	} values[MAX_SIZE];

	va_list	args;
	int		i;
	int		vcount;
	int		match;
	uchar*	tpl_ptr			= tpl;
	uchar*	result			= (uchar*)NULL;
	long	copy_size;
	long	prev_size;
	long	size			= 0L;

	if( !tpl )
		return (uchar*)NULL;

	va_start( args, tpl );

	for( vcount = 0; vcount < MAX_SIZE; vcount++ )
	{
		if( !( values[vcount].wildcard = va_arg( args, uchar* ) ) )
			break;

		values[vcount].value = va_arg( args, uchar* );
		values[vcount].clear = (pboolean)va_arg( args, int );

		if( !values[vcount].value )
		{
			values[vcount].value = pstrdup( "" );
			values[vcount].clear = TRUE;
		}
	}

	do
	{
		for( i = 0; i < vcount; i++ )
			values[i]._match = pstrstr( tpl_ptr, values[i].wildcard );

		match = vcount;
		for( i = 0; i < vcount; i++ )
		{
			if( values[i]._match != (uchar*)NULL )
			{
				if( match == vcount || values[match]._match > values[i]._match )
					match = i;
			}
		}

		prev_size = size;
		if( match < vcount )
		{
			copy_size = (long)( values[match]._match - tpl_ptr );
			size += (long)strlen( values[match].value );
		}
		else
			copy_size = (long)strlen( tpl_ptr );

		size += copy_size;

		if( result )
			result = (uchar*)prealloc( (uchar*)result,
				( size + 1 ) * sizeof( uchar ) );
		else
			result = (uchar*)pmalloc( ( size + 1 ) * sizeof( uchar ) );

		memcpy( result + prev_size, tpl_ptr, copy_size * sizeof( uchar ) );

		if( match < vcount )
			memcpy( result + prev_size + copy_size, values[match].value,
				strlen( values[match].value ) * sizeof( uchar ) );

		result[size] = '\0';

		if( match < vcount )
			tpl_ptr += copy_size + strlen( values[match].wildcard );
	}
	while( match < vcount );


	for( i = 0; i < vcount; i++ )
		if( values[i].clear )
			free( values[i].value );

	va_end( args );

	return result;
}

/** Removes whitespace on the left of a string.

//s// is the string to be left-trimmed.

Returns //s//.
*/
uchar* pstrltrim( uchar* s )
{
	uchar*	c;

	if( !( s && *s ) )
		return pgetstr( s );

	for( c = s; *c; c++ )
		if( !( *c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' ) )
			break;

	memmove( s, c, ( pstrlen( c ) + 1 ) * sizeof( uchar ) );

	return s;
}

/** Removes trailing whitespace on the right of a string.

//s// is the string to be right-trimmed.

Returns //s//.
*/
uchar* pstrrtrim( uchar* s )
{
	uchar*	c;

	if( !( s && *s ) )
		return pgetstr( s );

	for( c = s + pstrlen( s ) - 1; c > s; c-- )
		if( !( *c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' ) )
			break;

	*( c + 1 ) = '\0';

	return s;
}

/** Removes beginning and trailing whitespace from a string.

//s// is the string to be trimmed.

Returns //s//. 
*/
uchar* pstrtrim( uchar* s )
{
	if( !( s && *s ) )
		return s;

	return pstrltrim( pstrrtrim( s ) );
}

/** Splits a string at a delimiting token and returns an allocated array of
token reference pointers.

//tokens// is the an allocated array of tokenized array values.
Requires a pointer to uchar**.
//str// is the input string to be tokenized.
//sep// is the token separation substring.
//limit// is the token limit; If set to 0, there is no token limit available, so
that as much as possible tokens are read.

Returns the number of separated tokens, or negative values with errorcode
in error case.
*/
int pstrsplit( uchar*** tokens, uchar* str, uchar* sep, int limit )
{
	uchar*	next;
	uchar*	tok		= str;
	int		cnt		= 0;

	PROC( "pstrsplit" );
	PARMS( "tokens", "%p", tokens );
	PARMS( "str", "%s", str );
	PARMS( "sep", "%p", sep );
	PARMS( "limit", "%d", limit );

	if( !( tokens && str && sep && *sep ) )
		RETURN( ERR_PARMS );

	if( !( *tokens = (uchar**)pmalloc( MALLOC_STEP * sizeof( uchar* ) ) ) )
		RETURN( ERR_MEM );

	VARS( "cnt", "%d", cnt );
	VARS( "tok", "%s", tok );
	(*tokens)[ cnt++ ] = tok;

	while( ( next = pstrstr( tok, sep ) )
			&& ( ( limit > 0 ) ? cnt < limit : 1 ) )
	{
		tok = next + pstrlen( sep );
		VARS( "next", "%s", next );
		*next = '\0';

		if( ( cnt % MAX_SIZE ) == 0 )
		{
			MSG( "realloc required!" );
			if( !( *tokens = (uchar**)prealloc( (uchar**)*tokens,
					( cnt + MALLOC_STEP ) * sizeof( uchar* ) ) ) )
				RETURN( ERR_MEM );
		}

		VARS( "cnt", "%d", cnt );
		VARS( "tok", "%s", tok );
		(*tokens)[ cnt++ ] = tok;
	}

	if( limit > 0 )
		*next = '\0';

	RETURN( cnt );
}

/** Convert a string to upper-case order.

//s// is the acts both as input and output-string.

Returns //s//.
*/
uchar* pstrupr( uchar* s )
{
	uchar*	ptr;

	if( !s )
		return (uchar*)NULL;

	for( ptr = s; *ptr; ptr++ )
		if( pislower( *ptr ) )
			*ptr = ptoupper( *ptr );

	return s;
}

/** Convert a string to lower-case order.

//s// is the acts both as input and output-string.

Returns //s//.
*/
uchar* pstrlwr( uchar* s )
{
	uchar*	ptr;

	if( !s )
		return (uchar*)NULL;

	for( ptr = s; *ptr; ptr++ )
		if( pisupper( *ptr ) )
			*ptr = ptolower( *ptr );

	return s;
}

/** Compare a string by ignoring case-order.

//s1// is the string to compare with //s2//.
//s2// is the string to compare with //s1//.

Returns 0 if both strings are equal. Returns a value <0 if //s1// is lower than
//s2// or a value >0 if //s1// is greater than //s2//.
*/
int	pstrcasecmp( uchar* s1, uchar* s2 )
{
	if( !( s1 && s2 ) )
		return -1;

	for( ; *s1 && *s2 && ptoupper( *s1 ) == ptoupper( *s2 ); s1++, s2++ )
		;

	return (int)( ptoupper( *s1 ) - ptoupper( *s2 ) );
}

/** Compare a string by ignoring case-order about a maximum of //n// bytes.

//s1// is the string to compare with //s2//.
//s2// is the string to compare with //s1//.
//n// is the number of bytes to compare.

Returns 0 if both strings are equal. Returns a value <0 if //s1// is lower than
//s2// or a value >0 if //s1// is greater than //s2//.
*/
int	pstrncasecmp( uchar* s1, uchar* s2, pint n )
{
	if( !( s1 && s2 && n < 1 ) )
	{
		WRONGPARAM;
		return -1;
	}

	for( ; n > 0 && *s1 && *s2 && ptoupper( *s1 ) == ptoupper( *s2 );
			s1++, s2++, n-- )
		;

	return (int)( ( !n ) ? 0 : ( ptoupper( *s1 ) - ptoupper( *s2 ) ) );
}

/** Reads a character from a string. The character may exist of one single
character or it may be made up of an escape sequence or UTF-8 character.
The function returns the number of bytes read.

//retc// is the return pointer for the character code of the escaped string.
//str// is the begin pointer of the string at which character parsing begins.
If //escapeseq// is TRUE, the function regards escape sequences, else it ignores
them.

Returns the number of bytes that had been read for the character.
*/
int pstrparsechar( wchar* retc, uchar *str, pboolean escapeseq )
{
	wchar	ch;
    uchar 	digs[9]		=	"\0\0\0\0\0\0\0\0";
    int		dno 		= 0;
	uchar*	p			= str;

	PROC( "pstrparsechar" );
	PARMS( "retc", "%p", retc );
	PARMS( "str", "%s", str );
	PARMS( "escapeseq", "%s", BOOLEAN_STR( escapeseq ) );

	if( escapeseq && *p == '\\' )
	{
		p++;

		switch( *p )
		{
			case 'n':
				ch = '\n';
				p++;
				break;
			case 't':
				ch = '\t';
				p++;
				break;
			case 'r':
				ch = '\r';
				p++;
				break;
			case 'b':
				ch = '\b';
				p++;
				break;
			case 'f':
				ch = '\f';
				p++;
				break;
			case 'v':
				ch = '\v';
				p++;
				break;
			case 'a':
				ch = '\a';
				p++;
				break;

			default:
				if( octal_digit( *p ) )
				{
					do
						digs[dno++] = *( p++ );
					while( octal_digit( *p ) && dno < 3 );
					ch = strtol( digs, (uchar**)NULL, 8 );
				}
				else if( *p == 'x' )
				{
					p++;
					while( hex_digit( *p ) && dno < 2 )
						digs[ dno++ ] = *( p++ );

					if (dno > 0)
						ch = strtol( digs, (uchar**)NULL, 16 );
				}
#ifdef UTF8
				else if( *p == 'u' )
				{
					p++;
					while( hex_digit( *p ) && dno < 4 )
						digs[dno++] = *( p++ );

					if( dno > 0 )
						ch = strtol( digs, (uchar**)NULL, 16 );
				}
				else if( *p == 'U' )
				{
					p++;
					while( hex_digit( *p ) && dno < 8 )
						digs[dno++] = *( p++ );

					if( dno > 0 )
						ch = strtol( digs, (uchar**)NULL, 16 );
				}
#endif
				else
				{
#ifdef UTF8
					ch = u8_char( p );
					p += u8_seqlen( p );
#else
					ch = *( p++ );
#endif
				}
				break;
		}
	}
	else
	{
#ifdef UTF8
		ch = u8_char( p );
		p += u8_seqlen( p );
#else
		ch = *( p++ );
#endif
	}

	VARS( "ch", "%d", ch );
	VARS( "len", "%d", (int)( p - str ) );
	*retc = ch;

    RETURN( (int)( p - str ) );
}

/******************************************************************************
 * FUNCTIONS FOR UNICODE PROCESSING (pchar)                                   *
 ******************************************************************************/

#ifdef UNICODE

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrdup()

	Author:			Jan Max Meyer

	Usage:			Character-set independent strdup replacement.

	Parameters:		pchar*		str					String to be copied in
														memory.

	Returns:		pchar*							Returns a copy of 'str'.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstrdup( pchar* str )
{
	if( !str )
		return (pchar*)NULL;

	return (pchar*)pmemdup( str, ( Pstrlen( str ) + 1 ) * sizeof( pchar ) );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Psprintf()

	Author:			Jan Max Meyer

	Usage:			Psprintf() is a repleacement for a parameter-compatible
					sprintf-behavior function for wide-character.

					Yes, it's unsafe, but the same behavior as sprintf().

	Parameters:		pchar*		res					Result string
					pchar*		fmt					Format string
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		int								Returns the number of
													characters written, -1
													in error case.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int Psprintf( pchar* res, pchar* fmt, ... )
{
	int ret;
	va_list	args;

	PROC( "psprintf" );
	PARMS( "res", "%p", res );
	PARMS( "fmt", "%ls", fmt );

	if( !( res && fmt ) )
		RETURN( -1 );

	va_start( args, fmt );

	ret = 
#ifdef __MINGW32__
	_vsnwprintf
#else
	vswprintf
#endif
	( res, 256 * 1024 * 1024, fmt, args );

	va_end( args );

	VARS( "ret", "%d", ret );
	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pvasprintf()

	Author:			Jan Max Meyer

	Usage:			Wide-character implementation of pasprintf().

	Parameters:		pchar**		str					A pointer receiving the
													resultung, allocated string
													pointer.
	 				pchar*		fmt					The format string.
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		int								Returns the number of
													characters written.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int Pvasprintf( pchar** str, pchar* fmt, va_list ap )
{
	pchar*		istr;
	int			ilen;
	int			len;
	va_list		w_ap;

	PROC( "Pvasprintf" );
	PARMS( "str", "%p", str );
	PARMS( "fmt", "%ls", fmt );
	PARMS( "ap", "%p", ap );

	if( !( istr = (pchar*)pmalloc( ( ilen = MALLOC_STEP + 1 )
				* sizeof( pchar ) ) ) )
		RETURN( ERR_MEM );

	do
	{
		va_copy( w_ap, ap );

		len = 
#ifdef __MINGW32__
		_vsnwprintf
#else
		vswprintf
#endif
		( istr, (psize)ilen, fmt, w_ap );

		va_end( w_ap );
		VARS( "len", "%d", len );

		if( len < 0 )
		{
			if( !( istr = prealloc( istr, ( ilen = ilen + MALLOC_STEP + 1 )
					* sizeof( pchar ) ) ) )
			{
				RETURN( ERR_MEM );
			}
		}
	}
	while( len < 0 || len >= ilen );

	*str = istr;

	RETURN( len );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pasprintf()

	Author:			Jan Max Meyer

	Usage:			An implementation of pasprintf() for wide-character pchar.
					pasprintf() takes only the format-string and various
					arguments. It outputs an allocated string to be freed
					later on.

	Parameters:		pchar*		fmt					Format string
					...								Parameters according to the
													placeholders set in fmt.

	Returns:		pchar*							Returns the allocated string
													which cointains the format
													string with inserted values.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pasprintf( pchar* fmt, ... )
{
	pchar*	str;
	va_list	args;

	PROC( "pasprintf" );
	PARMS( "fmt", "%ls", fmt );

	if( !( fmt ) )
		RETURN( (pchar*)NULL );

	va_start( args, fmt );
	Pvasprintf( &str, fmt, args );
	va_end( args );

	VARS( "str", "%ls", str );
	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrcatchar()

	Author:			Jan Max Meyer

	Usage:			Appends a character to a dynamic wide-character string.

	Parameters:		pchar*		str				Pointer to a pchar-string to be
												appended.
												If this is (pchar*)NULL, the
												string is newly allocated.
					wchar		chr				The character to be appended to
												str.

	Returns:		pchar*						Pointer to (possibly re-)allo-
												cated and appended string.
												(pchar*)NULL is returned if no
												memory could be (re)allocated.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstrcatchar( pchar* str, wchar chr )
{
	PROC( "Pstrcatchar" );
	PARMS( "str", "%p", str );
	PARMS( "chr", "%d", chr );

	if( !str )
	{
		MSG( "Allocating new string" );
		str = (pchar*)pmalloc( ( 1 + 1 ) * sizeof( pchar ) );

		if( str )
			*str = L'\0';
	}
	else
	{
		MSG( "Reallocating existing string" );
		str = (pchar*)prealloc( (pchar*)str,
				( Pstrlen( str ) + 1 + 1) * sizeof( pchar ) );
	}

	VARS( "str", "%p", str );
	if( !str )
	{
		MSG( "Pointer is null, critical error" );
		exit( 1 );
	}

	Psprintf( str + Pstrlen( str ), L"%lc", chr );

	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrcatstr()

	Author:			Jan Max Meyer

	Usage:			Appends a (possibly dynamic) wide-character string to a
					dynamic wide-character string.

	Parameters:		pchar*		str				Pointer to a pchar-string to be
												appended.
												If this is (pchar*)NULL, the
												string is newly allocated.
					pchar*		append			The string to be appended.
					boolean		release_append	If true, append is free'd
												automatically by this function.

	Returns:		pchar*						Pointer to (possibly re-)allo-
												cated and appended string.
												(pchar*)NULL is returned if no
												memory could be (re)allocated,
												or both strings where NULL.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstrcatstr( pchar* dest, pchar* src, boolean freesrc )
{
	PROC( "Pstrcatstr" );
	PARMS( "dest", "%p", dest );
	PARMS( "src", "%p", src );
	PARMS( "freesrc", "%d", freesrc );

	if( src )
	{
		if( !dest )
		{
			if( freesrc )
			{
				dest = src;
				freesrc = FALSE;
			}
			else
				dest = Pstrdup( src );
		}
		else
		{
			dest = (pchar*)prealloc( (pchar*)dest,
					( Pstrlen( dest ) + Pstrlen( src ) + 1 )
						* sizeof( pchar ) );
			Pstrcat( dest, src );
		}

		if( freesrc )
			pfree( src );
	}

	RETURN( dest );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrncatstr()

	Author:			Jan Max Meyer

	Usage:			Appends a number of N characters from one wide-character
					string to a dynamic string.

	Parameters:		pchar*		str				Pointer to a pchar-string to be
												appended.
												If this is (pchar*)NULL, the
												string is newly allocated.
					pchar*		append			Begin of character sequence
												to be appended.
					psize		num				Number of characters to be
												appended to str.

	Returns:		pchar*						Pointer to (possibly re-)allo-
												cated and appended string.
												(pchar*)NULL is returned if no
												memory could be (re)allocated,
												or both strings where NULL.

	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
pchar* Pstrncatstr( pchar* str, pchar* append, psize num )
{
	psize	len		= 0;

	PROC( "Pstrncatstr" );
	PARMS( "str", "%p", str );
	PARMS( "str", "%ls", str );
	PARMS( "append", "%p", append );
	PARMS( "append", "%ls", append );
	PARMS( "num", "%d", num );

	if( append )
	{
		if( !str )
		{
			if( !( str = (pchar*)pmalloc( ( num + 1 ) * sizeof( pchar ) ) ) )
				RETURN( (pchar*)NULL );
		}
		else
		{
			len = Pstrlen( str );

			VARS( "len", "%d", len );
			VARS( "( len + num + 1 ) * sizeof( pchar )",
						"%d", ( len + num + 1 ) * sizeof( pchar ) );

			if( !( str = (pchar*)prealloc( (pchar*)str,
					( len + num + 1 ) * sizeof( pchar ) ) ) )
				RETURN( (pchar*)NULL );
		}

		Pstrncpy( str + len, append, num );
		str[ len + num ] = 0;
	}

	RETURN( str );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		Pstrlen()

	Author:			Jan Max Meyer

	Usage:			Saver strlen replacement for wide-character.

	Parameters:		pchar*		str					Parameter string to be
													evaluated. If (pchar*)NULL,
													the function returns 0.

	Returns:		psize							Same as strlen, 0 in case
													of (pchar*)NULL.
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
psize Pstrlen( pchar* str )
{
	if( !str )
		return 0;

	return wcslen( str );
}

#endif
