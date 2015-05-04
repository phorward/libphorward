/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pattern.c
Author:	Jan Max Meyer
Usage:	Internal regular expression pattern
		construction and conversion functions
----------------------------------------------------------------------------- */

#include "phorward.h"

/*NO_DOC*/
/* No documentation for the entire module, all here is only interally used. */

/* Local prototypes */
static pboolean parse_char( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags );
static pboolean parse_factor( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags );
static pboolean parse_sequence( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags );
static pboolean parse_alter( pregex_ptn** ptn, char** pstr,
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
pregex_ptn* pregex_ptn_create_char( pccl* ccl )
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
pregex_ptn* pregex_ptn_create_string( char* str, int flags )
{
	char*		ptr;
	wchar_t		ch;
	pregex_ptn*	chr;
	pregex_ptn*	seq		= (pregex_ptn*)NULL;
	pccl*		ccl;

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
	if( flags & PREGEX_COMP_WCHAR )
	{
		if( !( str = pwcs_to_str( (wchar_t*)str, FALSE ) ) )
			RETURN( (pregex_ptn*)NULL );
	}

	/* Loop through the string */
	for( ptr = str; *ptr; )
	{
		VARS( "ptr", "%s", ptr );
		ch = u8_parse_char( &ptr );

		VARS( "ch", "%d", ch );

		ccl = p_ccl_create( -1, -1, (char*)NULL );

		if( !( ccl && p_ccl_add( ccl, ch ) ) )
		{
			p_ccl_free( ccl );
			RETURN( (pregex_ptn*)NULL );
		}

		/* Is case-insensitive flag set? */
		if( flags & PREGEX_COMP_INSENSITIVE )
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
				ch = (char)tolower( (int)ch );
			else
				ch = (char)toupper( (int)ch );
#endif

			MSG( "Case-insensity set, new character evaluated is:" );
			VARS( "ch", "%d", ch );

			if( !p_ccl_add( ccl, ch ) )
			{
				p_ccl_free( ccl );
				RETURN( (pregex_ptn*)NULL );
			}
		}

		if( !( chr = pregex_ptn_create_char( ccl ) ) )
		{
			p_ccl_free( ccl );
			RETURN( (pregex_ptn*)NULL );
		}

		if( ! seq )
			seq = chr;
		else
			seq = pregex_ptn_create_seq( seq, chr, (pregex_ptn*)NULL );
	}

	/* Free duplicated string */
	if( flags & PREGEX_COMP_WCHAR )
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

/** Constructs a sub-pattern as backreference (like with parantheses).

//ptn// is the pattern that becomes the sub-ordered pattern.

Returns a pregex_ptn-node which can be child of another pattern construct
or part of a sequence.
*/
pregex_ptn* pregex_ptn_create_refsub( pregex_ptn* ptn )
{
	pregex_ptn*		pattern;

	if( !ptn )
	{
		WRONGPARAM;
		return (pregex_ptn*)NULL;
	}

	pattern = pregex_ptn_create( PREGEX_PTN_REFSUB );
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
			p_ccl_free( ptn->ccl );

		if( ptn->child[0] )
			pregex_ptn_free( ptn->child[0] );

		if( ptn->child[1] )
			pregex_ptn_free( ptn->child[1] );

		pfree( ptn->accept );
		pfree( ptn->str );

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
								"PREGEX_PTN_SUB", "PREGEX_PTN_REFSUB",
								"PREGEX_PTN_ALT",
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
			fprintf( stderr, " %s", p_ccl_to_str( ptn->ccl, FALSE ) );

		fprintf( stderr, "\n" );

		if( ptn->child[0] )
			pregex_ptn_print( ptn->child[0], rec + 1 );

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
static void pregex_char_to_REGEX( char* str, int size,
				wchar_t ch, pboolean escape, pboolean in_range )
{
	if( ( !in_range && ( strchr( "|()[]*+?", ch ) || ch == '.' ) ) ||
			( in_range && ch == ']' ) )
		psprintf( str, "\\%c", (char)ch );
	else if( escape )
		u8_escape_wchar( str, size, ch );
	else
		u8_toutf8( str, size, &ch, 1 );
}

/* Internal function for pregex_ptn_to_regex() */
static void p_ccl_to_REGEX( char** str, pccl* ccl )
{
	int				i;
	pcrange*		cr;
	wchar_t			cfrom;
	char			from	[ 40 + 1 ];
	wchar_t			cto;
	char			to		[ 20 + 1 ];
	pboolean		range	= FALSE;
	pboolean		neg		= FALSE;
	pboolean		is_dup	= FALSE;

	/*
	 * If this caracter class contains PCCL_MAX characters, then simply
	 * print a dot.
	 */
	if( p_ccl_count( ccl ) > PCCL_MAX )
	{
		*str = pstrcatchar( *str, '.' );
		return;
	}

	/* Better negate if more than 128 chars */
	if( p_ccl_count( ccl ) > 128 )
	{
		ccl = p_ccl_dup( ccl );
		is_dup = TRUE;

		neg = TRUE;
		p_ccl_negate( ccl );
	}

	/* Char-class or single char? */
	if( neg || p_ccl_count( ccl ) > 1 )
	{
		range = TRUE;
		*str = pstrcatchar( *str, '[' );
		if( neg )
			*str = pstrcatchar( *str, '^' );
	}

	/*
	 * If ccl contains a dash,
	 * this must be printed first!
	 */
	if( p_ccl_test( ccl, '-' ) )
	{
		if( !is_dup )
		{
			ccl = p_ccl_dup( ccl );
			is_dup = TRUE;
		}

		*str = pstrcatchar( *str, '-' );
		p_ccl_del( ccl, '-' );
	}

	/* Go trough ccl... */
	for( i = 0; p_ccl_get( &cfrom, &cto, ccl, i ); i++ )
	{
		pregex_char_to_REGEX( from, (int)sizeof( from ), cfrom, TRUE, range );

		if( cfrom < cto )
		{
			pregex_char_to_REGEX( to, (int)sizeof( to ), cto, TRUE, range );
			psprintf( from + strlen( from ), "%s%s",
				cfrom + 1 < cto ? "-" : "", to );
		}

		*str = pstrcatstr( *str, from, FALSE );
	}

	if( range )
		*str = pstrcatchar( *str, ']' );

	if( is_dup )
		p_ccl_free( ccl );
}

/* Internal function for pregex_ptn_to_regex() */
static pboolean pregex_ptn_to_REGEX( char** regex, pregex_ptn* ptn )
{
	if( !( regex && ptn ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	while( ptn )
	{
		switch( ptn->type )
		{
			case PREGEX_PTN_NULL:
				return TRUE;

			case PREGEX_PTN_CHAR:
				p_ccl_to_REGEX( regex, ptn->ccl );
				break;

			case PREGEX_PTN_SUB:
			case PREGEX_PTN_REFSUB:
				*regex = pstrcatchar( *regex, '(' );

				if( !pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
					return FALSE;

				*regex = pstrcatchar( *regex, ')' );
				break;

			case PREGEX_PTN_ALT:
				if( !pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
					return FALSE;

				*regex = pstrcatchar( *regex, '|' );

				if( !pregex_ptn_to_REGEX( regex, ptn->child[ 1 ] ) )
					return FALSE;

				break;

			case PREGEX_PTN_KLE:

				if( !pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
					return FALSE;

				*regex = pstrcatchar( *regex, '*' );
				break;

			case PREGEX_PTN_POS:

				if( !pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
					return FALSE;

				*regex = pstrcatchar( *regex, '+' );
				break;

			case PREGEX_PTN_OPT:

				if( !pregex_ptn_to_REGEX( regex, ptn->child[ 0 ] ) )
					return FALSE;

				*regex = pstrcatchar( *regex, '?' );
				break;

			default:
				MISSINGCASE;
				return FALSE;
		}

		ptn = ptn->next;
	}

	return TRUE;
}

/** Turns a regular expression pattern back into a regular expression string.

//ptn// is the pattern object to be converted into a regex.

The returned pointer is dynamically allocated but part of //ptn//, so it should
not be freed by the caller. It is automatically freed when the pattern object
is released.
*/
char* pregex_ptn_to_regex( pregex_ptn* ptn )
{
	if( !( ptn ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	ptn->str = pfree( ptn->str );

	if( !pregex_ptn_to_REGEX( &ptn->str, ptn ) )
	{
		ptn->str = pfree( ptn->str );
		return (char*)NULL;
	}

	return ptn->str;
}

/* Internal function for pregex_ptn_to_nfa() */
static pboolean pregex_ptn_to_NFA( pregex_nfa* nfa, pregex_ptn* pattern,
	pregex_nfa_st** start, pregex_nfa_st** end, int* ref_count )
{
	pregex_nfa_st*	n_start	= (pregex_nfa_st*)NULL;
	pregex_nfa_st*	n_end	= (pregex_nfa_st*)NULL;
	int				ref		= 0;
	int				i;

	if( !( pattern && nfa && start && end ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	*end = *start = (pregex_nfa_st*)NULL;

	while( pattern )
	{
		switch( pattern->type )
		{
			case PREGEX_PTN_NULL:
				return TRUE;

			case PREGEX_PTN_CHAR:
				n_start = pregex_nfa_create_state( nfa, (char*)NULL, 0 );
				n_end = pregex_nfa_create_state( nfa, (char*)NULL, 0 );

				n_start->ccl = p_ccl_dup( pattern->ccl );
				n_start->next = n_end;
				break;

			case PREGEX_PTN_REFSUB:
				if( !ref_count || ( ref = ++(*ref_count) ) > PREGEX_MAXREF )
					ref = 0;
				/* NO break! */

			case PREGEX_PTN_SUB:
				if( !pregex_ptn_to_NFA( nfa,
						pattern->child[ 0 ], &n_start, &n_end, ref_count ) )
					return FALSE;

				if( ref )
				{
					n_start->refs |= 1 << ref;
					n_end->refs |= 1 << ref;
				}

				break;

			case PREGEX_PTN_ALT:
			{
				pregex_nfa_st*	a_start;
				pregex_nfa_st*	a_end;

				n_start = pregex_nfa_create_state( nfa, (char*)NULL, 0 );
				n_end = pregex_nfa_create_state( nfa, (char*)NULL, 0 );

				if( !pregex_ptn_to_NFA( nfa,
						pattern->child[ 0 ], &a_start, &a_end, ref_count ) )
					return FALSE;

				n_start->next = a_start;
				a_end->next= n_end;

				if( !pregex_ptn_to_NFA( nfa,
						pattern->child[ 1 ], &a_start, &a_end, ref_count ) )
					return FALSE;

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

				n_start = pregex_nfa_create_state( nfa, (char*)NULL, 0 );
				n_end = pregex_nfa_create_state( nfa, (char*)NULL, 0 );

				if( !pregex_ptn_to_NFA( nfa,
						pattern->child[ 0 ], &m_start, &m_end, ref_count ) )
					return FALSE;

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
				MISSINGCASE;
				return FALSE;
		}

		if( ! *start )
		{
			*start = n_start;
			*end = n_end;
		}
		else
		{
			/*
				In sequences, when start has already been assigned,
				end will always be the pointer to an epsilon node.
				So the previous end can be replaced by the current
				start, to minimize the usage of states.

				This only if the state does not contain any additional
				informations (e.g. references).
			*/
			if( !( (*end)->refs ) )
			{
				memcpy( *end, n_start, sizeof( pregex_nfa_st ) );

				plist_remove( nfa->states,
					plist_get_by_ptr( nfa->states, n_start ) );
			}
			else
				(*end)->next = n_start;

			*end = n_end;
		}

		pattern = pattern->next;
	}

	return TRUE;
}

/** Converts a pattern-structure into a NFA state machine.

//nfa// is the NFA state machine structure that receives the compiled result of
the pattern. This machine will be extended to the pattern if it already contains
states. //nfa// must be initialized!

//ptn// is the pattern structure that will be converted and extended into
the NFA state machine.

//flags// are compile-time flags.

Returns TRUE on success.
*/
pboolean pregex_ptn_to_nfa( pregex_nfa* nfa, pregex_ptn* ptn )
{
	int				ret;
	pregex_nfa_st*	start;
	pregex_nfa_st*	end;
	pregex_nfa_st*	first;
	pregex_nfa_st*	n_first;
	int				ref_count	= 0;

	PROC( "pregex_ptn_to_nfa" );
	PARMS( "nfa", "%p", nfa );
	PARMS( "ptn", "%p", ptn );

	if( !( nfa && ptn ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	/* Find last first node ;) ... */
	for( n_first = (pregex_nfa_st*)plist_access( plist_first( nfa->states ) );
		n_first && n_first->next2; n_first = n_first->next2 )
			;

	/* Create first epsilon node */
	if( !( first = pregex_nfa_create_state( nfa, (char*)NULL, 0 ) ) )
		RETURN( FALSE );

	/* Turn pattern into NFA */
	if( !pregex_ptn_to_NFA( nfa, ptn, &start, &end, &ref_count ) )
		RETURN( FALSE );

	/* start is next of first */
	first->next = start;

	/* Chaining into big state machine */
	if( n_first )
		n_first->next2 = first;

	/* end becomes the accepting state */
	if( ptn->accept )
	{
		MSG( "Accepting information available" );

		VARS( "flags", "%d", ptn->accept->flags );
		VARS( "accept", "%d", ptn->accept->accept );

		memcpy( &( end->accept ), ptn->accept, sizeof( pregex_accept ) );
	}

	RETURN( TRUE );
}

/** Converts a pattern-structure into a DFA state machine.

//dfa// is the DFA state machine structure that receives the compiled result of
the pattern. //dfa// must be initialized!
//ptn// is the pattern structure that will be converted and extended into
the DFA state machine.

Returns TRUE on success.
*/
pboolean pregex_ptn_to_dfa( pregex_dfa* dfa, pregex_ptn* ptn )
{
	pregex_nfa*	nfa;

	PROC( "pregex_ptn_to_dfa" );
	PARMS( "dfa", "%p", dfa );
	PARMS( "ptn", "%p", ptn );

	if( !( dfa && ptn ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	pregex_dfa_reset( dfa );
	nfa = pregex_nfa_create();

	if( !pregex_ptn_to_nfa( nfa, ptn ) )
	{
		pregex_nfa_free( nfa );
		RETURN( FALSE );
	}

	if( pregex_dfa_from_nfa( dfa, nfa ) < 0 )
	{
		pregex_nfa_free( nfa );
		RETURN( FALSE );
	}

	pregex_nfa_free( nfa );

	if( pregex_dfa_minimize( dfa ) < 0 )
	{
		pregex_dfa_free( dfa );
		RETURN( FALSE );
	}

	RETURN( TRUE );
}

/** Converts a pattern-structure into a DFA state machine dfatab.

//dfatab// receives the allocated dfa content. Its made-up the same way
as described in the documentation of the function pregex_dfa_to_dfatab().
//ptn// is the pattern structure that will be converted into a DFA state
machine.

Returns the number of rows in //dfatab//, or a negative value in error case.
*/
int pregex_ptn_to_dfatab( wchar_t*** dfatab, pregex_ptn* ptn )
{
	pregex_dfa*	dfa;
	int			states;

	PROC( "pregex_ptn_to_dfatab" );
	PARMS( "dfatab", "%p", dfatab );
	PARMS( "ptn", "%p", ptn );

	if( !( ptn ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	dfa = pregex_dfa_create();

	if( !pregex_ptn_to_dfa( dfa, ptn ) )
	{
		pregex_dfa_free( dfa );
		RETURN( FALSE );
	}

	if( ( states = pregex_dfa_to_dfatab( dfatab, dfa ) ) < 0 )
	{
		pregex_dfa_free( dfa );
		RETURN( FALSE );
	}

	pregex_dfa_free( dfa );

	VARS( "states", "%d", states );
	RETURN( states );
}

/** Parse a regular expression pattern string into a pregex_ptn structure.

//ptn// is the return pointer receiving the root node of the generated pattern.

//str// is the pointer to the string which contains the pattern to be parsed. If
PREGEX_COMP_WCHAR is assigned in //flags//, this pointer must be set to a
wchar_t-array holding a wide-character string.

//flags// provides compile-time modifier flags (PREGEX_COMP_...).

Returns TRUE on success.
*/
pboolean pregex_ptn_parse( pregex_ptn** ptn, char* str, int flags )
{
	int				ret;
	char*			ptr;
	pregex_accept	accept;

	PROC( "pregex_ptn_parse" );
	PARMS( "ptn", "%p", ptn );
	PARMS( "str", "%s", str );
	PARMS( "flags", "%d", flags );

	if( !( ptn && str ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	/* Set default values into accept structure, except accept member! */
	memset( &accept, 0, sizeof( pregex_accept ) );

	/* If PREGEX_COMP_STATIC is set, parsing is not required! */
	if( flags & PREGEX_COMP_STATIC )
	{
		if( !( *ptn = pregex_ptn_create_string( str, flags ) ) )
			RETURN( FALSE );

		(*ptn)->accept = pmemdup( &accept, sizeof( pregex_accept ) );
		RETURN( TRUE );
	}

	/* Copy input string - this is required,
		because of memory modification during the parse */
	if( flags & PREGEX_COMP_WCHAR )
	{
		if( !( ptr = str = pwcs_to_str( (wchar_t*)str, FALSE ) ) )
			RETURN( FALSE );
	}
	else
	{
		if( !( ptr = str = pstrdup( str ) ) )
			RETURN( FALSE );
	}

	VARS( "ptr", "%s", ptr );

	/* Parse anchor at begin of regular expression */
	if( !( flags & PREGEX_COMP_NOANCHORS ) )
	{
		MSG( "Anchors at begin" );

		if( *ptr == '^' )
		{
			accept.flags |= PREGEX_FLAG_BOL;
			ptr++;
		}
		else if( !strncmp( ptr, "\\<", 2 ) )
			/* This is a GNU-like extension */
		{
			accept.flags |= PREGEX_FLAG_BOW;
			ptr += 2;
		}
	}

	/* Run the recursive descent parser */
	MSG( "Starting the parser" );
	VARS( "ptr", "%s", ptr );

	if( !parse_alter( ptn, &ptr, &accept, flags ) )
		RETURN( FALSE );

	VARS( "ptr", "%s", ptr );

	/* Parse anchor at end of regular expression */
	if( !( flags & PREGEX_COMP_NOANCHORS ) )
	{
		MSG( "Anchors at end" );
		if( !strcmp( ptr, "$" ) )
			accept.flags |= PREGEX_FLAG_EOL;
		else if( !strcmp( ptr, "\\>" ) )
			/* This is a GNU-style extension */
			accept.flags |= PREGEX_FLAG_EOW;
	}

	/* Force nongreedy matching */
	if( flags & PREGEX_COMP_NONGREEDY )
		accept.flags |= PREGEX_FLAG_NONGREEDY;

	/* Free duplicated string */
	pfree( str );

	/* Copy accept structure */
	(*ptn)->accept = pmemdup( &accept, sizeof( pregex_accept ) );

	RETURN( TRUE );
}

/******************************************************************************
 *      RECURSIVE DESCENT PARSER FOR REGULAR EXPRESSIONS FOLLOWS HERE...      *
 ******************************************************************************/

static pboolean parse_char( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags )
{
	pccl*		ccl;
	int			ret;
	pregex_ptn*	alter;
	wchar_t		single;
	char		restore;
	char*		zero;
	pboolean	neg		= FALSE;

	switch( **pstr )
	{
		case '(':
			(*pstr)++;

			if( !parse_alter( &alter, pstr, accept, flags ) )
				return FALSE;

			if( flags & PREGEX_COMP_NOREF &&
					!( *ptn = pregex_ptn_create_sub( alter ) ) )
				return FALSE;
			else if( !( *ptn = pregex_ptn_create_refsub( alter ) ) )
				return FALSE;

			/* Report error? */
			if( **pstr != ')' && !( flags & PREGEX_COMP_NOERRORS ) )
			{
				fprintf( stderr, "Missing closing bracket in regex\n" );
				return FALSE;
			}

			(*pstr)++;
			break;

		case '.':
			ccl = p_ccl_create( -1, -1, (char*)NULL );

			if( !( ccl && p_ccl_addrange( ccl,
								PCCL_MIN, PCCL_MAX ) ) )
			{
				p_ccl_free( ccl );
				return FALSE;
			}

			if( !( *ptn = pregex_ptn_create_char( ccl ) ) )
			{
				p_ccl_free( ccl );
				return FALSE;
			}

			(*pstr)++;
			break;

		case '[':
			if( ( zero = strchr( (*pstr)+1, ']' ) ) )
			{
				restore = *zero;
				*zero = '\0';

				if( (*pstr) + 1 < zero && *((*pstr)+1) == '^' )
				{
					neg = TRUE;
					(*pstr)++;
				}

				if( !( ccl = p_ccl_create( -1, -1, (*pstr) + 1 ) ) )
					return FALSE;

				if( neg )
					p_ccl_negate( ccl );

				if( !( *ptn = pregex_ptn_create_char( ccl ) ) )
				{
					p_ccl_free( ccl );
					return FALSE;
				}

				*zero = restore;
				*pstr = zero + 1;
				break;
			}
			/* No break here! */

		default:
			*pstr += pstrparsechar( &single, *pstr, TRUE );

			ccl = p_ccl_create( -1, -1, (char*)NULL );

			if( !( ccl && p_ccl_add( ccl, single ) ) )
			{
				p_ccl_free( ccl );
				return FALSE;
			}

			if( !( *ptn = pregex_ptn_create_char( ccl ) ) )
			{
				p_ccl_free( ccl );
				return FALSE;
			}

			break;
	}

	return TRUE;
}

static pboolean parse_factor( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags )
{
	if( !parse_char( ptn, pstr, accept, flags ) )
		return FALSE;

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
				return FALSE;

			(*pstr)++;
			break;

		default:
			break;
	}

	return TRUE;
}

static pboolean parse_sequence( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags )
{
	pregex_ptn*	next;

	if( !parse_factor( ptn, pstr, accept, flags ) )
		return FALSE;

	while( !( **pstr == '|' || **pstr == ')' || **pstr == '\0' ) )
	{
		if( !( flags & PREGEX_COMP_NOANCHORS ) )
		{
			if( !strcmp( *pstr, "$" ) || !strcmp( *pstr, "\\>" ) )
				break;
		}

		if( !parse_factor( &next, pstr, accept, flags ) )
			return FALSE;

		*ptn = pregex_ptn_create_seq( *ptn, next, (pregex_ptn*)NULL );
	}

	return TRUE;
}

static pboolean parse_alter( pregex_ptn** ptn, char** pstr,
										pregex_accept* accept, int flags )
{
	pregex_ptn*	seq;

	if( !parse_sequence( ptn, pstr, accept, flags ) )
		return FALSE;

	while( **pstr == '|' )
	{
		(*pstr)++;

		if( !parse_sequence( &seq, pstr, accept, flags ) )
			return FALSE;

		if( !( *ptn = pregex_ptn_create_alt( *ptn, seq, (pregex_ptn*)NULL ) ) )
			return FALSE;
	}

	return TRUE;
}

/*COD_ON*/
