/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	pattern.c
Author:	Jan Max Meyer
Usage:	Regular expression pattern management functions
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

/*
 * Global variables
 */
#define INC( i )			(i)++
#define VALID_CHAR( ch )	!pstrchr( "|()[]*+?", (ch) )

/*
 * Local prototypes
 */
static int parse_char( pregex_ptn** ptn, uchar** pstr, pregex_accept* accept, int flags );
static int parse_factor( pregex_ptn** ptn, uchar** pstr, pregex_accept* accept, int flags );
static int parse_sequence( pregex_ptn** ptn, uchar** pstr, pregex_accept* accept, int flags );
static int parse_alter( pregex_ptn** ptn, uchar** pstr, pregex_accept* accept, int flags );

/*
 * Functions
 */

static pregex_ptn* pregex_ptn_create( pregex_ptntype type )
{
	pregex_ptn*		pattern;
	
	pattern = pmalloc( sizeof( pregex_ptn ) );
	pattern->type = type;
	
	return pattern;	
}

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
							(char*)NULL, REGEX_MOD_NONE );
				n_end = pregex_nfa_create_state( nfa,
							(uchar*)NULL, REGEX_MOD_NONE );
				
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
							(char*)NULL, REGEX_MOD_NONE );
				n_end = pregex_nfa_create_state( nfa,
							(uchar*)NULL, REGEX_MOD_NONE );
				
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
							(char*)NULL, REGEX_MOD_NONE );
				n_end = pregex_nfa_create_state( nfa,
							(uchar*)NULL, REGEX_MOD_NONE );
				
				if( ( ret = pregex_ptn_to_NFA( nfa, pattern->child[ 0 ],
								&m_start, &m_end ) ) != ERR_OK )
					return ret;

				/* Standard chain linkng */
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
										^           |
										|___________|
						*/
						n_start->next2 = n_end;
						m_end->next2 = m_start;
						break;
					
					case PREGEX_PTN_POS:
						/*
								m_start -> n_start -> n_end -> m_end
											  ^         |
											  |_________|
						*/
						n_end->next2 = n_start;
						break;
						
					case PREGEX_PTN_OPT:
						/*
									_____________________________
								   |                             |
								   |                             v
								m_start -> n_start -> n_end -> m_end
						*/
						n_start->next2 = n_end;
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
	PARMS( "anchors", "%d", anchors );
	PARMS( "accept", "%d", accept );
	PARMS( "greedy", "%s", BOOLEAN_STR( greedy ) );
	
	if( !( nfa && pattern && accept >= 0 ) )
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
			(uchar*)NULL, REGEX_MOD_NONE ) ) )
	{
		RETURN( ERR_MEM );
	}
	
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
	
	return ERR_OK;
}

int pregex_ptn_parse( pregex_ptn** ptn, pregex_accept* accept,
						char* str, int flags )
{
	int			ret;
	char*		ptr;
	
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

	/* Copy input string - this is required,
		because of memory modification during the parse */
	if( flags & REGEX_MOD_WCHAR )
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
		
	/* Initialize parameters */	
	if( accept )
		pregex_accept_init( accept );
	
	/* Parse anchor at begin of regular expression */
	if( accept && !( flags & REGEX_MOD_NO_ANCHORS ) )
	{
		MSG( "Anchors at begin" );

		if( *ptr == '^' )
		{
			accept->anchors |= REGEX_ANCHOR_BOL;
			ptr++;
		}
		else if( !pstrncmp( ptr, "\\<", 2 ) )
				/* This is a GNU-like extension */
		{
			accept->anchors |= REGEX_ANCHOR_BOW;
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
	if( accept && !( flags & REGEX_MOD_NO_ANCHORS ) )
	{
		MSG( "Anchors at end" );
		if( !pstrcmp( ptr, "$" ) )
			accept->anchors |= REGEX_ANCHOR_EOL;
		else if( !pstrcmp( ptr, "\\>" ) )
			/* This is a GNU-style extension */
			accept->anchors |= REGEX_ANCHOR_EOW;
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

			if( **pstr != ')' && !( flags & REGEX_MOD_NO_ERRORS ) )
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
			*pstr += pstr_char( &single, *pstr, TRUE );
			
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
		if( !( flags & REGEX_MOD_NO_ANCHORS ) )
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
