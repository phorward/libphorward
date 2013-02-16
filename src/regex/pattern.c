/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pattern.c
Author:	Jan Max Meyer
Usage:	Regular expression pattern construction and conversion functions
----------------------------------------------------------------------------- */

#include <phorward.h>

#define INC( i )			(i)++
#define VALID_CHAR( ch )	( !pstrchr( "|()[]*+?", (ch) ) )

/* Local prototypes */
static int parse_char( pregex_ptn** ptn, uchar** pstr,
							pregex_accept* accept, int flags );
static int parse_factor( pregex_ptn** ptn, uchar** pstr,
							pregex_accept* accept, int flags );
static int parse_sequence( pregex_ptn** ptn, uchar** pstr,
							pregex_accept* accept, int flags );
static int parse_alter( pregex_ptn** ptn, uchar** pstr,
							pregex_accept* accept, int flags );

/* Create a pattern object of type //type//; Internal constructor! */
static pregex_ptn* pregex_ptn_create( pregex_ptntype type )
{
	pregex_ptn*		pattern;

	pattern = pmalloc( sizeof( pregex_ptn ) );
	pattern->type = type;

	return pattern;
}

/** Constructs a character-class pattern.

//ccl// is the pointer to a character class. This pointer is not duplicated,
and will be directly assigned to the object.

Returns a pregex_ptn-node which can be child of another pattern construct or
part of a sequence.
*/
pregex_ptn* pregex_ptn_create_char( CCL ccl )
{
	pregex_ptn*		pattern;

	if( !( ccl ) )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	pattern = pregex_ptn_create( PREGEX_PTN_CHAR );
	pattern->ccl = ccl;

	return pattern;
}

/** Constructs a pattern for a static string.

//str// is the input string to be converted.
//flags// are optional flags for wide-character support.

Returns a pregex_ptn-node which can be child of another pattern construct or
part of a sequence.
*/
pregex_ptn* pregex_ptn_create_string( uchar* str, int flags )
{
	uchar*		ptr;
	wchar		ch;
	pregex_ptn*	chr;
	pregex_ptn*	seq		= (pregex_ptn*)NULL;
	pregex_ccl	ccl;

	PROC( "pregex_ptn_create_string" );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );

	/* Check parameters */
	if( !( str ) )
	{
		WRONGPARAM;
		RETURN( (pregex_ptn*)NULL );
	}

	/* Convert string to UTF-8, if in wide-character mode */
	if( flags & PREGEX_MOD_WCHAR )
	{
		if( !( str = pchar_to_uchar( (pchar*)str, FALSE ) ) )
			RETURN( (pregex_ptn*)NULL );
	}

	/* Loop through the string */
	for( ptr = str; *ptr; )
	{
		VARS( "ptr", "%s", ptr );
		ch = u8_parse_char( &ptr );

		VARS( "ch", "%d", ch );

		if( !( ccl = ccl_addrange( (CCL)NULL, ch, ch ) ) )
			RETURN( (pregex_ptn*)NULL );

		/* Is case-insensitive flag set? */
		if( flags & PREGEX_MOD_INSENSITIVE )
		{
#ifdef UTF8
			MSG( "UTF-8 mode, trying to convert" );
			if( iswupper( ch ) )
				ch = towlower( ch );
			else
				ch = towupper( ch );
#else
			MSG( "non UTF-8 mode, trying to convert" );
			if( isupper( ch ) )
				ch = (uchar)tolower( (int)ch );
			else
				ch = (uchar)toupper( (int)ch );
#endif

			MSG( "Case-insensity set, new character evaluated is:" );
			VARS( "ch", "%d", ch );

			if( !( ccl = ccl_addrange( ccl, ch, ch ) ) )
				RETURN( (pregex_ptn*)NULL );
		}

		if( !( chr = pregex_ptn_create_char( ccl ) ) )
			RETURN( (pregex_ptn*)NULL );

		if( ! seq )
			seq = chr;
		else
			seq = pregex_ptn_create_seq( seq, chr, (pregex_ptn*)NULL );
	}

	/* Free duplicated string */
	if( flags & PREGEX_MOD_WCHAR )
		pfree( str );

	RETURN( seq );
}

/** Constructs a sub-pattern (like with parantheses).

//ptn// is the pattern that becomes the sub-ordered pattern.

Returns a pregex_ptn-node which can be child of another pattern construct
or part of a sequence.
*/
pregex_ptn* pregex_ptn_create_sub( pregex_ptn* ptn )
{
	pregex_ptn*		pattern;

	if( !ptn )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	pattern = pregex_ptn_create( PREGEX_PTN_SUB );
	pattern->child[0] = ptn;

	return pattern;
}

/** Constructs alternations of multiple patterns.

//left// is the first pattern of the alternation.
//...// are multiple pregex_ptn-pointers follow which become part of the
alternation. The last node must be specified as (pregex_ptn*)NULL.

Returns a pregex_ptn-node which can be child of another pattern construct or
part of a sequence. If there is only //left// assigned without other alternation
patterns, //left// will be returned back.
*/
pregex_ptn* pregex_ptn_create_alt( pregex_ptn* left, ...  )
{
	pregex_ptn*		pattern;
	pregex_ptn*		alter;
	va_list			alt;

	if( !( left ) )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	va_start( alt, left );

	while( ( alter = va_arg( alt, pregex_ptn* ) ) )
	{
		pattern = pregex_ptn_create( PREGEX_PTN_ALT );
		pattern->child[0] = left;
		pattern->child[1] = alter;

		left = pattern;
	}

	va_end( alt );

	return left;
}

/** Constructs a kleene-closure repetition, allowing for multiple or none
repetitions of the specified pattern.

//ptn// is the pattern that will be configured for kleene-closure.

Returns a pregex_ptn-node which can be child of another pattern construct or
part of a sequence.
*/
pregex_ptn* pregex_ptn_create_kle( pregex_ptn* ptn )
{
	pregex_ptn*		pattern;

	if( !ptn )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	pattern = pregex_ptn_create( PREGEX_PTN_KLE );
	pattern->child[0] = ptn;

	return pattern;
}

/** Constructs an positive-closure, allowing for one or multiple specified
pattern.

//ptn// is the pattern to be configured for positive closure.

Returns a pregex_ptn-node which can be child of another pattern construct or
part of a sequence.
*/
pregex_ptn* pregex_ptn_create_pos( pregex_ptn* ptn )
{
	pregex_ptn*		pattern;

	if( !ptn )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	pattern = pregex_ptn_create( PREGEX_PTN_POS );
	pattern->child[0] = ptn;

	return pattern;
}

/** Constructs an optional-closure, allowing for one or none specified pattern.

//ptn// is the pattern to be configured for optional closure.

Returns a pregex_ptn-node which can be child of another pattern construct or
part of a sequence.
*/
pregex_ptn* pregex_ptn_create_opt( pregex_ptn* ptn )
{
	pregex_ptn*		pattern;

	if( !ptn )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	pattern = pregex_ptn_create( PREGEX_PTN_OPT );
	pattern->child[0] = ptn;

	return pattern;
}

/** Constructs a sequence of multiple patterns.

//first// is the beginning pattern of the sequence.
//...// follows as parameter list of multiple patterns that become part of the
sequence. The last pointer must be specified as (pregex_ptn*)NULL to mark the
end of the list.

Always returns the pointer to //first//.
*/
pregex_ptn* pregex_ptn_create_seq( pregex_ptn* first, ... )
{
	pregex_ptn*		prev	= first;
	pregex_ptn*		next;
	va_list			chain;

	if( !first )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	va_start( chain, first );

	while( ( next = va_arg( chain, pregex_ptn* ) ) )
	{
		for( ; prev->next; prev = prev->next )
			;

		/* Alternatives must be put in brackets */
		if( next->type == PREGEX_PTN_ALT )
			next = pregex_ptn_create_sub( next );

		prev->next = next;
		prev = next;
	}

	va_end( chain );

	return first;
}

/** Releases memory of a pattern including all its subsequent and following
patterns.

//ptn// is the pattern object to be released.

Always returns (pregex_ptn*)NULL.
*/
pregex_ptn* pregex_ptn_free( pregex_ptn* ptn )
{
	pregex_ptn*		next;

	if( !ptn )
		return (pregex_ptn*)NULL;

	do
	{
		if( ptn->type == PREGEX_PTN_CHAR )
			ccl_free( ptn->ccl );

		if( ptn->child[0] )
			pregex_ptn_free( ptn->child[0] );

		if( ptn->child[1] )
			pregex_ptn_free( ptn->child[1] );


		next = ptn->next;
		pfree( ptn );

		ptn = next;
	}
	while( ptn );

	return (pregex_ptn*)NULL;
}

/** A debug function to print a pattern's hierarchical structure to stderr.

//ptn// is the pattern object to be printed.
//rec// is the recursion depth, set this to 0 at initial call.
*/
void pregex_ptn_print( pregex_ptn* ptn, int rec )
{
	int			i;
	char		gap		[ 100+1 ];
	char*		ptr;
	static char	types[][20]	= { "PREGEX_PTN_NULL", "PREGEX_PTN_CHAR",
								"PREGEX_PTN_SUB", "PREGEX_PTN_ALT",
								"PREGEX_PTN_KLE", "PREGEX_PTN_POS",
								"PREGEX_PTN_OPT"
							};

	for( i = 0; i < rec; i++ )
		gap[i] = '.';
	gap[i] = 0;

	do
	{
		fprintf( stderr, "%s%s", gap, types[ ptn->type ] );

		if( ptn->type == PREGEX_PTN_CHAR )
		{
			ptr = ccl_to_str( ptn->ccl, FALSE );
			fprintf( stderr, " %s", ptr );
			pfree( ptr );
		}

		fprintf( stderr, "\n" );

		if( ptn->child[0] )
		{
			pregex_ptn_print( ptn->child[0], rec + 1 );
		}

		if( ptn->child[1] )
		{
			if( ptn->type == PREGEX_PTN_ALT )
				fprintf( stderr, "%s\n", gap );

			pregex_ptn_print( ptn->child[1], rec + 1 );
		}

		ptn = ptn->next;
	}
	while( ptn );
}

/* Internal function for pregex_ptn_to_regex() */
static void pregex_char_to_REGEX( uchar* str, int size,
				pchar ch, pboolean escape, pboolean in_range )
{
	if( ( !in_range && ( !VALID_CHAR( ch ) || ch == '.' ) ) ||
			( in_range && ch == ']' ) )
		psprintf( str, "\\%c", (uchar)ch );
	else if( escape )
		u8_escape_wchar( str, size, ch );
	else
		u8_toutf8( str, size, &ch, 1 );
}

/* Internal function for pregex_ptn_to_regex() */
static void pregex_ccl_to_REGEX( uchar** str, pregex_ccl ccl )
{
	pregex_ccl		neg		= (pregex_ccl)NULL;
	pregex_ccl		i;
	uchar			from	[ 40 + 1 ];
	uchar			to		[ 20 + 1 ];
	pboolean		range	= FALSE;

	/*
	 * If this caracter class contains CCL_MAX characters, then simply
	 * print a dot.
	 */
	if( ccl_count( ccl ) == CCL_MAX )
	{
		*str = pstrcatchar( *str, '.' );
		return;
	}

	/*
	 * Always duplicate character-class,
	 * we sometimes will modify it
	 */
	ccl = ccl_dup( ccl );

	if( ccl_count( ccl ) > 128 )
		ccl = neg = ccl_negate( ccl );

	if( neg || ccl_count( ccl) > 1 )
	{
		range = TRUE;
		*str = pstrcatchar( *str, '[' );
		if( neg )
			*str = pstrcatchar( *str, '^' );
	}

	/*
	 * If ccl contains a dash,
	 * it should be printed first!
	 */
	if( ccl_test( ccl, '-' ) )
	{
		*str = pstrcatchar( *str, '-' );
		ccl = ccl_delrange( ccl, '-', '-' );
	}

	/* Go trough ccl... */
	for( i = ccl; !ccl_end( i ); i++ )
	{
		pregex_char_to_REGEX( from, (int)sizeof( from ),
								i->begin, TRUE, range );

		if( i->begin != i->end )
		{
			pregex_char_to_REGEX( to, (int)sizeof( to ),
									i->end, TRUE, range );
			psprintf( from + strlen( from ), "-%s", to );
		}

		*str = pstrcatstr( *str, from, FALSE );
	}

	if( range )
		*str = pstrcatchar( *str, ']' );

	ccl_free( ccl );
}

/* Internal function for pregex_ptn_to_regex() */
static int pregex_ptn_to_REGEX( uchar** regex, pregex_ptn* ptn )
{
	int		ret;

	if( !( regex && ptn ) )
	{
		WRONGPARAM;
		return ERR_PARMS;
	}

	while( ptn )
	{
		switch( ptn->type )
		{
			case PREGEX_PTN_NULL:
				return ERR_OK;

			case PREGEX_PTN_CHAR:
				pregex_ccl_to_REGEX( regex, ptn->ccl );
				break;

			case PREGEX_PTN_SUB:
				*regex = pstrcatchar( *regex, '(' );

				if( ( ret = pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
						!= ERR_OK )
					return ret;

				*regex = pstrcatchar( *regex, ')' );
				break;

			case PREGEX_PTN_ALT:
				if( ( ret = pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
						!= ERR_OK )
					return ret;

				*regex = pstrcatchar( *regex, '|' );

				if( ( ret = pregex_ptn_to_REGEX( regex, ptn->child[ 1 ] ) )
						!= ERR_OK )
					return ret;

				break;

			case PREGEX_PTN_KLE:

				if( ( ret = pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
						!= ERR_OK )
					return ret;

				*regex = pstrcatchar( *regex, '*' );
				break;

			case PREGEX_PTN_POS:

				if( ( ret = pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
						!= ERR_OK )
					return ret;

				*regex = pstrcatchar( *regex, '+' );
				break;

			case PREGEX_PTN_OPT:

				if( ( ret = pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
						!= ERR_OK )
					return ret;

				*regex = pstrcatchar( *regex, '?' );
				break;

			default:
				return ERR_UNIMPL;
		}

		ptn = ptn->next;
	}

	return ERR_OK;
}

/** Turns a regular expression pattern back into a regular expression string.

//regex// is the return pointer for the regular expression string. This must be
released by the caller with pfree(), if the function returns ERR_OK.
//ptn// is the pattern object to be converted into a regex.

Returns a int Returns a standard error define on failure, and ERR_OK on success.
*/
int pregex_ptn_to_regex( uchar** regex, pregex_ptn* ptn )
{
	if( !( regex && ptn ) )
	{
		WRONGPARAM;
		return ERR_PARMS;
	}

	*regex = (uchar*)NULL;

	return pregex_ptn_to_REGEX( regex, ptn );
}

/* Internal function for pregex_ptn_to_nfa() */
static int pregex_ptn_to_NFA( pregex_nfa* nfa, pregex_ptn* pattern,
	pregex_nfa_st** start, pregex_nfa_st** end )
{
	pregex_nfa_st*	n_start	= (pregex_nfa_st*)NULL;
	pregex_nfa_st*	n_end	= (pregex_nfa_st*)NULL;
	int				ret;

	if( !( pattern && nfa && start && end ) )
	{
		WRONGPARAM;
		return ERR_PARMS;
	}

	*end = *start = (pregex_nfa_st*)NULL;

	while( pattern )
	{
		switch( pattern->type )
		{
			case PREGEX_PTN_NULL:
				return ERR_OK;

			case PREGEX_PTN_CHAR:
				n_start = pregex_nfa_create_state( nfa,
							(char*)NULL, PREGEX_MOD_NONE );
				n_end = pregex_nfa_create_state( nfa,
							(uchar*)NULL, PREGEX_MOD_NONE );

				n_start->ccl = ccl_dup( pattern->ccl );
				n_start->next = n_end;
				break;

			case PREGEX_PTN_SUB:
				nfa->ref_cur++;
				nfa->ref_count++;

				if( ( ret = pregex_ptn_to_NFA( nfa, pattern->child[ 0 ],
						&n_start, &n_end ) ) != ERR_OK )
					return ret;

				/* Patch the last transition to previous reference */
				n_end->ref = --nfa->ref_cur;
				break;

			case PREGEX_PTN_ALT:
			{
				pregex_nfa_st*	a_start;
				pregex_nfa_st*	a_end;

				n_start = pregex_nfa_create_state( nfa,
							(char*)NULL, PREGEX_MOD_NONE );
				n_end = pregex_nfa_create_state( nfa,
							(uchar*)NULL, PREGEX_MOD_NONE );

				if( ( ret = pregex_ptn_to_NFA( nfa, pattern->child[ 0 ],
						&a_start, &a_end ) ) != ERR_OK )
					return ret;

				n_start->next = a_start;
				a_end->next= n_end;

				if( ( ret = pregex_ptn_to_NFA( nfa, pattern->child[ 1 ],
						&a_start, &a_end ) ) != ERR_OK )
					return ret;

				n_start->next2 = a_start;
				a_end->next= n_end;
				break;
			}

			case PREGEX_PTN_KLE:
			case PREGEX_PTN_POS:
			case PREGEX_PTN_OPT:
			{
				pregex_nfa_st*	m_start;
				pregex_nfa_st*	m_end;

				n_start = pregex_nfa_create_state( nfa,
							(char*)NULL, PREGEX_MOD_NONE );
				n_end = pregex_nfa_create_state( nfa,
							(uchar*)NULL, PREGEX_MOD_NONE );

				if( ( ret = pregex_ptn_to_NFA( nfa, pattern->child[ 0 ],
								&m_start, &m_end ) ) != ERR_OK )
					return ret;

				/* Standard chain linking */
				n_start->next = m_start;
				m_end->next = n_end;

				switch( pattern->type )
				{
					case PREGEX_PTN_KLE:
						/*
								 ____________________________
								|                            |
								|                            v
							n_start -> m_start -> m_end -> n_end
										  ^         |
										  |_________|
						*/
						n_start->next2 = n_end;
						m_end->next2 = m_start;
						break;

					case PREGEX_PTN_POS:
						/*
								n_start -> m_start -> m_end -> n_end
											  ^         |
											  |_________|
						*/
						m_end->next2 = m_start;
						break;

					case PREGEX_PTN_OPT:
						/*
									_____________________________
								   |                             |
								   |                             v
								n_start -> m_start -> m_end -> n_end
						*/
						n_start->next2 = n_end;
						break;

					default:
						break;
				}
				break;
			}

			default:
				return ERR_UNIMPL;
		}

		if( ! *start )
		{
			*start = n_start;
			*end = n_end;
		}
		else
		{
			memcpy( *end, n_start, sizeof( pregex_nfa_st ) );
			memset( n_start, 0, sizeof( pregex_nfa_st ) );

			nfa->states = list_remove( nfa->states, (void*)n_start );
			if( !( nfa->empty = list_push( nfa->empty, (void*)n_start ) ) )
				return ERR_MEM;

			*end = n_end;
		}

		pattern = pattern->next;
	}

	return ERR_OK;
}

/** Converts a pattern-structure into a NFA state machine.

//nfa// is the NFA state machine structure that receives the compiled result of
the pattern. This machine will be extended to the pattern if it already contains
states.
//pattern// is the pattern structure that will be converted and extended into
the NFA state machine.
//accept// is the accept structure that will be assigned to the last NFA node.
This structure is optional, and can be left-out as (pregex_accept*)NULL.

Returns a standard error define on failure, and ERR_OK on success.
*/
int pregex_ptn_to_nfa( pregex_nfa* nfa, pregex_ptn* pattern,
							pregex_accept* accept )
{
	int				ret;
	pregex_nfa_st*	start;
	pregex_nfa_st*	end;
	pregex_nfa_st*	first;
	pregex_nfa_st*	n_first;

	PROC( "pregex_ptn_to_nfa" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "pattern", "%p", pattern );
	PARMS( "accept->anchors", "%d", accept ? accept->anchors : -999 );
	PARMS( "accept->accept", "%d", accept ? accept->accept : -999 );
	PARMS( "accept->greedy", "%s", BOOLEAN_STR(
									accept ? accept->greedy : FALSE ) );

	if( !( nfa && pattern ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	/* Find last first node ;) ... */
	for( n_first = (pregex_nfa_st*)list_access( nfa->states );
		n_first && n_first->next2; n_first = n_first->next2 )
			;

	/* Reference counter */
	nfa->ref_cur = nfa->ref_count++;

	/* Create first epsilon node */
	if( !( first = pregex_nfa_create_state( nfa,
			(uchar*)NULL, PREGEX_MOD_NONE ) ) )
		RETURN( ERR_MEM );

	/* Turn pattern into NFA */
	if( ( ret = pregex_ptn_to_NFA( nfa, pattern, &start, &end ) ) != ERR_OK )
		RETURN( ret );

	/* start is next of first */
	first->next = start;

	/* Chaining into big state machine */
	if( n_first )
		n_first->next2 = first;

	/* end becomes the accepting state */
	if( accept )
		memcpy( &( end->accept ), accept, sizeof( pregex_accept ) );

	RETURN( ERR_OK );
}

/** Parse a regular expression pattern string into a pregex_ptn structure.

//ptn// is the return pointer receiving the root node of the generated pattern.

//accept// is the pointer to a pregex_accept structure that receives the members
//greedy// and //anchors//. The //accept// member must be changed by the caller.
This parameter is optional, and can be left-out as (pregex_accept*)NULL.

//str// is the pointer to the string which contains the pattern to be parsed. If
PREGEX_MOD_WCHAR is assigned in //flags//, this pointer must be set to a
pchar-array holding wide-character strings.

//flags// provides compile-time flags.

Returns a standard error define on failure, and ERR_OK on success.
*/
int pregex_ptn_parse( pregex_ptn** ptn, pregex_accept* accept,
						uchar* str, int flags )
{
	int			ret;
	uchar*		ptr;

	PROC( "pregex_ptn_parse" );
	PARMS( "ptn", "%p", ptn );
	PARMS( "str", "%s", str );
	PARMS( "accept", "%p", accept );
	PARMS( "flags", "%d", flags );

	if( !( ptn && str ) )
	{
		WRONGPARAM;
		RETURN( ERR_PARMS );
	}

	/* Set default values into accept structure, except accept member! */
	if( accept )
	{
		accept->greedy = TRUE;
		accept->anchors = PREGEX_ANCHOR_NONE;
	}

	/* If PREGEX_MOD_STATIC is set, parsing is not required! */
	if( flags & PREGEX_MOD_STATIC )
	{
		if( !( *ptn = pregex_ptn_create_string( str, flags ) ) )
			RETURN( ERR_MEM );

		RETURN( ERR_OK );
	}

	/* Copy input string - this is required,
		because of memory modification during the parse */
	if( flags & PREGEX_MOD_WCHAR )
	{
		if( !( ptr = str = pchar_to_uchar( (pchar*)str, FALSE ) ) )
			RETURN( ERR_MEM );
	}
	else
	{
		if( !( ptr = str = pstrdup( str ) ) )
			RETURN( ERR_MEM );
	}

	VARS( "ptr", "%s", ptr );

	/* Parse anchor at begin of regular expression */
	if( accept && !( flags & PREGEX_MOD_NO_ANCHORS ) )
	{
		MSG( "Anchors at begin" );

		if( *ptr == '^' )
		{
			accept->anchors |= PREGEX_ANCHOR_BOL;
			ptr++;
		}
		else if( !pstrncmp( ptr, "\\<", 2 ) )
			/* This is a GNU-like extension */
		{
			accept->anchors |= PREGEX_ANCHOR_BOW;
			ptr += 2;
		}
	}

	/* Run the recursive descent parser */
	MSG( "Starting the parser" );
	VARS( "ptr", "%s", ptr );

	if( ( ret = parse_alter( ptn, &ptr, accept, flags ) ) != ERR_OK )
		RETURN( ret );

	VARS( "ptr", "%s", ptr );

	/* Parse anchor at end of regular expression */
	if( accept && !( flags & PREGEX_MOD_NO_ANCHORS ) )
	{
		MSG( "Anchors at end" );
		if( !pstrcmp( ptr, "$" ) )
			accept->anchors |= PREGEX_ANCHOR_EOL;
		else if( !pstrcmp( ptr, "\\>" ) )
			/* This is a GNU-style extension */
			accept->anchors |= PREGEX_ANCHOR_EOW;
	}

	/* Free duplicated string */
	pfree( str );

	RETURN( ERR_OK );
}

/******************************************************************************
 *      RECURSIVE DESCENT PARSER FOR REGULAR EXPRESSIONS FOLLOWS HERE...      *
 ******************************************************************************/

static int parse_char( pregex_ptn** ptn, uchar** pstr,
		pregex_accept* accept, int flags )
{
	CCL			ccl;
	int			ret;
	pregex_ptn*	alter;
	wchar		single;
	uchar		restore;
	uchar*		zero;
	pboolean	neg		= FALSE;

	switch( **pstr )
	{
		case '(':
			INC( *pstr );

			if( ( ret = parse_alter( &alter, pstr, accept, flags ) )
					!= ERR_OK )
				return ret;

			if( !( *ptn = pregex_ptn_create_sub( alter ) ) )
				return ERR_MEM;

			if( **pstr != ')' && !( flags & PREGEX_MOD_NO_ERRORS ) )
				return 1;

			INC( *pstr );
			break;

		case '.':
			/*
				If ANY_CHAR is used, then greedyness should be set to
				non-greedy by default
			*/
			if( accept )
				accept->greedy = FALSE;

			if( !( ccl = ccl_addrange( (CCL)NULL, CCL_MIN, CCL_MAX ) ) )
				return ERR_MEM;

			if( !( *ptn = pregex_ptn_create_char( ccl ) ) )
				return ERR_MEM;

			INC( *pstr );
			break;

		case '[':
			if( ( zero = pstrchr( (*pstr)+1, ']' ) ) )
			{
				restore = *zero;
				*zero = '\0';

				if( (*pstr) + 1 < zero && *((*pstr)+1) == '^' )
				{
					neg = TRUE;
					(*pstr)++;
				}

				if( !( ccl = ccl_create( (*pstr) + 1 ) ) )
					return ERR_MEM;

				if( neg )
					ccl = ccl_negate( ccl );

				if( !( *ptn = pregex_ptn_create_char( ccl ) ) )
					return ERR_MEM;

				*zero = restore;
				*pstr = zero + 1;
				break;
			}
			/* No break here! */

		default:
			*pstr += pstrparsechar( &single, *pstr, TRUE );

			if( !( ccl = ccl_add( (CCL)NULL, single ) ) )
				return ERR_MEM;

			if( !( *ptn = pregex_ptn_create_char( ccl ) ) )
				return ERR_MEM;

			break;
	}

	return ERR_OK;
}

static int parse_factor( pregex_ptn** ptn, uchar** pstr,
		pregex_accept* accept, int flags )
{
	int				ret;

	if( ( ret = parse_char( ptn, pstr, accept, flags ) ) != ERR_OK )
		return ret;

	switch( **pstr )
	{
		case '*':
		case '+':
		case '?':

			switch( **pstr )
			{
				case '*':
					*ptn = pregex_ptn_create_kle( *ptn );
					break;
				case '+':
					*ptn = pregex_ptn_create_pos( *ptn );
					break;
				case '?':
					*ptn = pregex_ptn_create_opt( *ptn );
					break;

				default:
					break;
			}

			if( ! *ptn )
				return ERR_MEM;

			INC( *pstr );
			break;

		default:
			break;
	}

	return ERR_OK;
}

static int parse_sequence( pregex_ptn** ptn, uchar** pstr,
		pregex_accept* accept, int flags )
{
	int			ret;
	pregex_ptn*	next;

	if( ( ret = parse_factor( ptn, pstr, accept, flags ) ) != ERR_OK )
		return ret;

	while( !( **pstr == '|' || **pstr == ')' || **pstr == '\0' ) )
	{
		if( !( flags & PREGEX_MOD_NO_ANCHORS ) )
		{
			if( !pstrcmp( *pstr, "$" ) || !pstrcmp( *pstr, "\\>" ) )
				break;
		}

		if( ( ret = parse_factor( &next, pstr, accept, flags ) ) != ERR_OK )
			return ret;

		*ptn = pregex_ptn_create_seq( *ptn, next, (pregex_ptn*)NULL );
	}

	return ERR_OK;
}

static int parse_alter( pregex_ptn** ptn, uchar** pstr,
		pregex_accept* accept, int flags )
{
	int			ret;
	pregex_ptn*	seq;

	if( ( ret = parse_sequence( ptn, pstr, accept, flags ) ) != ERR_OK )
		return ret;

	while( **pstr == '|' )
	{
		INC( *pstr );

		if( ( ret = parse_sequence( &seq, pstr, accept, flags ) ) != ERR_OK )
			return ret;

		if( !( *ptn = pregex_ptn_create_alt( *ptn, seq, (pregex_ptn*)NULL ) ) )
			return ERR_MEM;
	}

	return ERR_OK;
}
