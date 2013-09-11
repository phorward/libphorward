/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
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

//str// is the pointer to a string to be appended. If this is (char*)NULL,
the string will be newly allocated. //chr// is the the character to be appended
to str.

Returns a char*-pointer to the (possibly re-)allocated and appended string.
(char*)NULL is returned if no memory could be (re)allocated. This pointer must
be released with pfree() when its existence is no longer required.
*/
char* pstrcatchar( char* str, wchar chr )
{
	PROC( "pstrcatchar" );
	PARMS( "str", "%p", str );
	PARMS( "chr", "%d", chr );

	if( !chr )
		return str;

	if( !str )
	{
		MSG( "Allocating new string" );
		str = (char*)pmalloc( ( 1 + 1 ) * sizeof( char ) );

		if( str )
			*str = '\0';
	}
	else
	{
		MSG( "Reallocating existing string" );
		str = (char*)prealloc( (char*)str,
				( pstrlen( str ) + 1 + 1) * sizeof( char ) );
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
If this is (char*)NULL, the string is newly allocated.

//append// is the string to be appended at the end of //str//.

//release_append// frees the pointer provided as //append// automatically by
this function, if set to TRUE. This parameter has only a comfort-function.

Returns a char*-pointer to (possibly re-)allocated and appended string.
(char*)NULL is returned if no memory could be (re)allocated, or both strings
where NULL. If //dest// is NULL and //freesrc// is FALSE, the function
automatically returns the pointer //src//. This pointer must be released with
pfree() when its existence is no longer required.
*/
char* pstrcatstr( char* dest, char* src, boolean freesrc )
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
			dest = (char*)prealloc( (char*)dest,
					( pstrlen( dest ) + pstrlen( src ) + 1 )
						* sizeof( char ) );
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

//str// is the pointer to a string to be appended. If this is (char*)NULL,
the string is newly allocated. //append// is the begin of character sequence to
be appended. //n// is the number of characters to be appended to //str//.

Returns a char*-pointer to (possibly re-)allocated and appended string.
(char*)NULL is returned if no memory could be (re)allocated, or both strings
where NULL. This pointer must be released with pfree() when its existence
is no longer required.
*/
char* pstrncatstr( char* str, char* append, psize n )
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
			if( !( str = (char*)pmalloc( ( n + 1 ) * sizeof( char ) ) ) )
				RETURN( (char*)NULL );
		}
		else
		{
			len = pstrlen( str );

			if( !( str = (char*)prealloc( (char*)str,
					( len + n + 1 ) * sizeof( char ) ) ) )
				RETURN( (char*)NULL );
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

Returns a pointer to char* containing the allocated string which is the
resulting source. This pointer must be released with pfree() when its existence
is no longer required.
*/
char* pstrreplace( char* str, char* find, char* replace )
{
	char*			match;
	char*			str_ptr			= str;
	char*			result			= (char*)NULL;
	char*			result_end		= (char*)NULL;
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
		if( !( match = strstr( str_ptr, find ) ) )
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
			result = result_end = (char*)pmalloc(
				( size + 1 ) * sizeof( char ) );
		else
		{
			result = (char*)prealloc( (char*)result,
				( result_end - result + size + 1 ) * sizeof( char ) );
			result_end = result + pstrlen( result );
		}

		if( !result )
		{
			MSG( "Ran out of memory!" );
			exit( 1 );
		}

		strncpy( result_end, str_ptr, match - str_ptr );
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

Returns a char*-pointer to the newly allocated copy of //str//. This pointer
must be released with pfree() when its existence is no longer required.
*/
char* pstrdup( char* str )
{
	if( !str )
		return (char*)NULL;

	return (char*)pmemdup( str, ( pstrlen( str ) + 1 ) * sizeof( char ) );
}

/** Duplicate //n// characters from a string in memory.

The function mixes the functionalities of strdup() and strncpy().
The resulting string will be zero-terminated.

//str// is the parameter string to be duplicated. If this is provided as
(char*)NULL, the function will also return (char*)NULL.
//n// is the the number of characters to be copied and duplicated from //str//.
If //n// is greater than the length of //str//, copying will stop at the zero
terminator.

Returns a char*-pointer to the allocated memory holding the zero-terminated
string duplicate. This pointer must be released with pfree() when its existence
is no longer required.
*/
char* pstrndup( char* str, psize len )
{
	char*	ret;

	if( !str )
		return (char*)NULL;

	if( pstrlen( str ) < len )
		len = pstrlen( str );

	ret = (char*)pmalloc( ( len + 1 ) * sizeof( char ) );
	strncpy( ret, str, len );
	ret[ len ] = '\0';

	return ret;
}

/** Return length of a string.

//str// is the parameter string to be evaluated. If (char*)NULL, the function
returns 0. pstrlen() is much more saver than strlen() because it returns 0 when
a NULL-pointer is provided.

Returns the length of the string //str//.
*/
psize pstrlen( char* str )
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
int psprintf( char* res, char* fmt, ... )
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
int pvasprintf( char** str, char* fmt, va_list ap )
{
	char*		istr;
	int			ilen;
	int			len;
	va_list		w_ap;

	PROC( "pvasprintf" );
	PARMS( "str", "%p", str );
	PARMS( "fmt", "%s", fmt );
	PARMS( "ap", "%p", ap );

	if( !( istr = (char*)pmalloc( MALLOC_STEP * sizeof( char ) ) ) )
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
		if( !( istr = prealloc( istr, ( ilen = len + 1 ) * sizeof( char ) ) ) )
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

Returns a char* Returns the allocated string which cointains the format string with inserted values.
*/
char* pasprintf( char* fmt, ... )
{
	char*	str;
	va_list	args;

	PROC( "pasprintf" );
	PARMS( "fmt", "%s", fmt );

	if( !( fmt ) )
		RETURN( (char*)NULL );

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

Returns a pointer to the allocated heap memory on success, (char*)NULL else.
This is the same pointer as returned like calling ``*str``. The returned pointer
must be released with pfree() or another call of psetstr(). Calling psetstr()
as ``psetstr( &p, (char*)NULL );`` is equivalent to ``p = pfree( &p )``.
*/
char* psetstr( char** str, char* val )
{
	if( *str )
	{
		if( strcmp( *str, val ) == 0 )
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
char* pgetstr( char* str )
{
	if( !str )
		return "";

	return str;
}

/** String rendering function.

Inserts multiple values dynamically into the according wildcards positions of
a template string. The function can be compared to the function of
pstrreplace(), but allows to replace multiple substrings by multiple replacement
strings.

//tpl// is the template string to be rendered with values.
//...// are the set of values to be inserted into the desired position;

These must be always consist of three values each:

- //char* name// as a wildcard-name
- //char* value// as the replacement value for the wildcard
- //pboolean freeflag// defines if //value// shall be freed after processing
-

Returns an allocated string which is the resulting source. This string must be
release by pfree() or another function releasing heap memory when its existence
is no longer required.
*/
char* pstrrender( char* tpl, ... )
{
	struct
	{
		char*	wildcard;
		char*	value;
		BOOLEAN	clear;
		char*	_match;
	} values[MAX_SIZE];

	va_list	args;
	int		i;
	int		vcount;
	int		match;
	char*	tpl_ptr			= tpl;
	char*	result			= (char*)NULL;
	long	copy_size;
	long	prev_size;
	long	size			= 0L;

	if( !tpl )
		return (char*)NULL;

	va_start( args, tpl );

	for( vcount = 0; vcount < MAX_SIZE; vcount++ )
	{
		if( !( values[vcount].wildcard = va_arg( args, char* ) ) )
			break;

		values[vcount].value = va_arg( args, char* );
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
			values[i]._match = strstr( tpl_ptr, values[i].wildcard );

		match = vcount;
		for( i = 0; i < vcount; i++ )
		{
			if( values[i]._match != (char*)NULL )
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
			result = (char*)prealloc( (char*)result,
				( size + 1 ) * sizeof( char ) );
		else
			result = (char*)pmalloc( ( size + 1 ) * sizeof( char ) );

		memcpy( result + prev_size, tpl_ptr, copy_size * sizeof( char ) );

		if( match < vcount )
			memcpy( result + prev_size + copy_size, values[match].value,
				strlen( values[match].value ) * sizeof( char ) );

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
char* pstrltrim( char* s )
{
	char*	c;

	if( !( s && *s ) )
		return pgetstr( s );

	for( c = s; *c; c++ )
		if( !( *c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' ) )
			break;

	memmove( s, c, ( pstrlen( c ) + 1 ) * sizeof( char ) );

	return s;
}

/** Removes trailing whitespace on the right of a string.

//s// is the string to be right-trimmed.

Returns //s//.
*/
char* pstrrtrim( char* s )
{
	char*	c;

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
char* pstrtrim( char* s )
{
	if( !( s && *s ) )
		return s;

	return pstrltrim( pstrrtrim( s ) );
}

/** Splits a string at a delimiting token and returns an allocated array of
token reference pointers.

//tokens// is the an allocated array of tokenized array values.
Requires a pointer to char**.
//str// is the input string to be tokenized.
//sep// is the token separation substring.
//limit// is the token limit; If set to 0, there is no token limit available, so
that as much as possible tokens are read.

Returns the number of separated tokens, or negative values with errorcode
in error case.
*/
int pstrsplit( char*** tokens, char* str, char* sep, int limit )
{
	char*	next;
	char*	tok		= str;
	int		cnt		= 0;

	PROC( "pstrsplit" );
	PARMS( "tokens", "%p", tokens );
	PARMS( "str", "%s", str );
	PARMS( "sep", "%p", sep );
	PARMS( "limit", "%d", limit );

	if( !( tokens && str && sep && *sep ) )
		RETURN( ERR_PARMS );

	if( !( *tokens = (char**)pmalloc( MALLOC_STEP * sizeof( char* ) ) ) )
		RETURN( ERR_MEM );

	VARS( "cnt", "%d", cnt );
	VARS( "tok", "%s", tok );
	(*tokens)[ cnt++ ] = tok;

	while( ( next = strstr( tok, sep ) )
			&& ( ( limit > 0 ) ? cnt < limit : 1 ) )
	{
		tok = next + pstrlen( sep );
		VARS( "next", "%s", next );
		*next = '\0';

		if( ( cnt % MAX_SIZE ) == 0 )
		{
			MSG( "realloc required!" );
			if( !( *tokens = (char**)prealloc( (char**)*tokens,
					( cnt + MALLOC_STEP ) * sizeof( char* ) ) ) )
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
char* pstrupr( char* s )
{
	char*	ptr;

	if( !s )
		return (char*)NULL;

	for( ptr = s; *ptr; ptr++ )
		if( islower( *ptr ) )
			*ptr = toupper( *ptr );

	return s;
}

/** Convert a string to lower-case order.

//s// is the acts both as input and output-string.

Returns //s//.
*/
char* pstrlwr( char* s )
{
	char*	ptr;

	if( !s )
		return (char*)NULL;

	for( ptr = s; *ptr; ptr++ )
		if( isupper( *ptr ) )
			*ptr = tolower( *ptr );

	return s;
}

/** Compare a string by ignoring case-order.

//s1// is the string to compare with //s2//.
//s2// is the string to compare with //s1//.

Returns 0 if both strings are equal. Returns a value <0 if //s1// is lower than
//s2// or a value >0 if //s1// is greater than //s2//.
*/
int	pstrcasecmp( char* s1, char* s2 )
{
	if( !( s1 && s2 ) )
		return -1;

	for( ; *s1 && *s2 && toupper( *s1 ) == toupper( *s2 ); s1++, s2++ )
		;

	return (int)( toupper( *s1 ) - toupper( *s2 ) );
}

/** Compare a string by ignoring case-order about a maximum of //n// bytes.

//s1// is the string to compare with //s2//.
//s2// is the string to compare with //s1//.
//n// is the number of bytes to compare.

Returns 0 if both strings are equal. Returns a value <0 if //s1// is lower than
//s2// or a value >0 if //s1// is greater than //s2//.
*/
int	pstrncasecmp( char* s1, char* s2, pint n )
{
	if( !( s1 && s2 && n < 1 ) )
	{
		WRONGPARAM;
		return -1;
	}

	for( ; n > 0 && *s1 && *s2 && toupper( *s1 ) == toupper( *s2 );
			s1++, s2++, n-- )
		;

	return (int)( ( !n ) ? 0 : ( toupper( *s1 ) - toupper( *s2 ) ) );
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
int pstrparsechar( wchar* retc, char *str, pboolean escapeseq )
{
	wchar	ch;
    char 	digs[9]		=	"\0\0\0\0\0\0\0\0";
    int		dno 		= 0;
	char*	p			= str;

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

/** Duplicate a wide-character string in memory.

//str// is the string to be copied in memory. If //str// is provided as NULL,
the function will also return NULL.

Returns a pchar*-pointer to the newly allocated copy of //str//. This pointer
must be released with pfree() when its existence is no longer required.
*/
pchar* pwcsdup( pchar* str )
{
	if( !str )
		return (pchar*)NULL;

	return (pchar*)pmemdup( str, ( pwcslen( str ) + 1 ) * sizeof( pchar ) );
}

/** pwcsprintf() is a repleacement for a parameter-compatible sprintf-behavior
function for wide-character.
Yes, it's unsafe, but the same behavior as sprintf().

//res// is the result string
//fmt// is the format string
//// is the parameters according to the placeholders set in fmt.

Returns a int Returns the number of characters written, -1 in error case.
*/
int pwcsprintf( pchar* res, pchar* fmt, ... )
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

/** Wide-character implementation of pasprintf().

//str// is the a pointer receiving the resultung, allocated string pointer.
//fmt// is the the format string.
//...// is the parameters according to the placeholders set in //fmt//.

Returns a int Returns the number of characters written.
*/
int pvawcsprintf( pchar** str, pchar* fmt, va_list ap )
{
	pchar*		istr;
	int			ilen;
	int			len;
	va_list		w_ap;

	PROC( "pvawcsprintf" );
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

/** An implementation of pasprintf() for wide-character pchar. pasprintf() takes
only the format-string and various arguments. It outputs an allocated string to
be released with pfree() later on.

//fmt// is the format string.
//...// are the parameters according to the placeholders set in //fmt//.

Returns a pchar* Returns the allocated string which cointains the format string
with inserted values.
*/
pchar* pawcsprintf( pchar* fmt, ... )
{
	pchar*	str;
	va_list	args;

	PROC( "pasprintf" );
	PARMS( "fmt", "%ls", fmt );

	if( !( fmt ) )
		RETURN( (pchar*)NULL );

	va_start( args, fmt );
	pvawcsprintf( &str, fmt, args );
	va_end( args );

	VARS( "str", "%ls", str );
	RETURN( str );
}

/** Appends a character to a dynamic wide-character string.

//str// is the pointer to a pchar-string to be appended. If this is
(pchar*)NULL, the string is newly allocated. //chr// is the the character to be
appended to str.

Returns a pchar* Pointer to (possibly re-)allo- cated and appended string.
(pchar*)NULL is returned if no memory could be (re)allocated.
*/
pchar* pwcscatchar( pchar* str, wchar chr )
{
	PROC( "pwcscatchar" );
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
				( pwcslen( str ) + 1 + 1) * sizeof( pchar ) );
	}

	VARS( "str", "%p", str );
	if( !str )
	{
		MSG( "Pointer is null, critical error" );
		exit( 1 );
	}

	pwcsprintf( str + pwcslen( str ), L"%lc", chr );

	RETURN( str );
}

/** Appends a (possibly dynamic) wide-character string to a dynamic
wide-character string.

//str// is the pointer to a pchar-string to be appended. If this is
(pchar*)NULL, the string is newly allocated.
//append// is the the string to be appended.
//freesrc// if true, //append// is free'd automatically by this function.

Returns a pchar* Pointer to (possibly re-)allo- cated and appended string.
(pchar*)NULL is returned if no memory could be (re)allocated, or both strings
where NULL.
*/
pchar* pwcscatstr( pchar* dest, pchar* src, boolean freesrc )
{
	PROC( "pwcscatstr" );
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
				dest = pwcsdup( src );
		}
		else
		{
			dest = (pchar*)prealloc( (pchar*)dest,
					( pwcslen( dest ) + pwcslen( src ) + 1 )
						* sizeof( pchar ) );
			wcscat( dest, src );
		}

		if( freesrc )
			pfree( src );
	}

	RETURN( dest );
}

/** Appends a number of N characters from one wide-character string to a dynamic
string.

//str// is the pointer to a pchar-string to be appended. If this is
(pchar*)NULL, the string is newly allocated.
//append// is the begin of character sequence to be appended.
//num// is the number of characters to be appended to str.

Returns a pchar* Pointer to (possibly re-)allo- cated and appended string.
(pchar*)NULL is returned if no memory could be (re)allocated, or both strings
where NULL.
*/
pchar* pwcsncatstr( pchar* str, pchar* append, psize num )
{
	psize	len		= 0;

	PROC( "pwcsncatstr" );
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
			len = pwcslen( str );

			VARS( "len", "%d", len );
			VARS( "( len + num + 1 ) * sizeof( pchar )",
						"%d", ( len + num + 1 ) * sizeof( pchar ) );

			if( !( str = (pchar*)prealloc( (pchar*)str,
					( len + num + 1 ) * sizeof( pchar ) ) ) )
				RETURN( (pchar*)NULL );
		}

		wcsncpy( str + len, append, num );
		str[ len + num ] = 0;
	}

	RETURN( str );
}

/*REMOVE?*/
/** Saver strlen replacement for wide-character.

//str// is the parameter string to be evaluated. If (pchar*)NULL,
the function returns 0. */
psize pwcslen( pchar* str )
{
	if( !str )
		return 0;

	return wcslen( str );
}

#endif
