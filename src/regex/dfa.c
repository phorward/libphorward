/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	dfa.c
Author:	Jan Max Meyer
Usage:	DFA creation and transformation functions
----------------------------------------------------------------------------- */

#define PREGEX_LOCAL
#include <phorward.h>

/*NO_DOC*/

/* Printing a DFA-state machine to a stream, for debug */
void pregex_dfa_print( FILE* stream, pregex_dfa* dfa )
{
	LIST*			l;
	pregex_dfa_st*	s;
	pregex_dfa_tr*	e;
	LIST*			m;
	int				i;

	for( l = dfa->states; l; l = list_next( l ) )
	{
		s = (pregex_dfa_st*)list_access( l );
		fprintf( stream, "*** STATE %d (accepts %d, ref_cnt %d, anchors %d)\n",
			list_find( dfa->states, (void*)s ), s->accept.accept,
				s->ref_cnt, s->accept.anchors );

		if( s->ref_cnt )
		{
			for( i = 0; i < s->ref_cnt; i++ )
				fprintf( stream, "ref[%d]=%d ", i, s->ref[i] );

			fprintf( stream, "\n" );
		}

		for( m = s->trans; m; m = list_next( m ) )
		{
			e = (pregex_dfa_tr*)list_access( m );

			ccl_print( stream, e->ccl, 0 );
			fprintf( stream, "-> %d\n", e->go_to );
		}

		fprintf( stream, "\n" );
	}
}

/* Creating a new DFA state */
static pregex_dfa_st* pregex_dfa_create_state( pregex_dfa* dfa )
{
	pregex_dfa_st* 	ptr;

	if( !( ptr = pmalloc( sizeof( pregex_dfa_st ) ) ) )
		return (pregex_dfa_st*)NULL;

	memset( ptr, 0, sizeof( pregex_dfa_st ) );

	pregex_accept_init( &( ptr->accept ) );
	ptr->done = FALSE;

	if( !( dfa->states = list_push( dfa->states, (void*)ptr ) ) )
	{
		pfree( ptr );
		return (pregex_dfa_st*)NULL;
	}

	return ptr;
}

/* Freeing a DFA-state */
static void pregex_dfa_delete_state( pregex_dfa_st* st )
{
	LIST*			l;
	pregex_dfa_tr*	tr;

	for( l = st->trans; l; l = list_next( l ) )
	{
		tr = (pregex_dfa_tr*)list_access( l );
		pfree( tr->ccl );
		pfree( tr );
	}

	list_free( st->trans );

	pfree( st->ref );
	pfree( st );
}

/* Checks the unfinished DFA state machine for states having the done-flag
	set to FALSE and returns the first occurence. If no more undone states
	are found, (regex_dfa*)NULL will be returned. */
static pregex_dfa_st* pregex_dfa_get_undone_state( pregex_dfa* dfa )
{
	LIST*			l;
	pregex_dfa_st*	ptr;

	for( l = dfa->states; l; l = list_next( l ) )
	{
		ptr = (pregex_dfa_st*)list_access( l );
		if( !ptr->done )
			return ptr;
	}

	return (pregex_dfa_st*)NULL;
}

/* Checks for DFA-states with same NFA-epsilon transitions than the specified
	one within the DFA-state machine. If an equal item is found, the offset of
		that DFA-state is returned, else -1. */
static int pregex_dfa_same_transitions( pregex_dfa* dfa, LIST* trans )
{
	LIST*			l;
	pregex_dfa_st*	ptr;

	for( l = dfa->states; l; l = list_next( l ) )
	{
		ptr = (pregex_dfa_st*)list_access( l );

		if( list_diff( ptr->nfa_set, trans ) )
			return list_find( dfa->states, (void*)ptr );
	}

	return -1;
}

/** Frees and resets a DFA state machine.

//dfa// is the pointer to a DFA-machine to be reset.

Always returns (pregex_dfa*)NULL.
*/
pregex_dfa* pregex_dfa_free( pregex_dfa* dfa )
{
	LIST*			l;
	LIST*			m;
	pregex_dfa_st*	dfa_st;
	pregex_dfa_tr*	tr;

	PROC( "pregex_dfa_free" );
	PARMS( "dfa", "%p", dfa );

	if( !( dfa ) )
		RETURN( (pregex_dfa*)NULL );

	LISTFOR( dfa->states, l )
	{
		dfa_st = (pregex_dfa_st*)list_access( l );

		LISTFOR( dfa_st->trans, m )
		{
			tr = (pregex_dfa_tr*)list_access( m );

			ccl_free( tr->ccl );
			pfree( tr );
		}

		list_free( dfa_st->trans );
		list_free( dfa_st->nfa_set );

		pfree( dfa_st->ref );
		pfree( dfa_st );
	}

	list_free( dfa->states );

	memset( dfa, 0, sizeof( pregex_dfa ) );

	RETURN( (pregex_dfa*)NULL );
}

/** Performs a check on all DFA state transitions to figure out the default
transition for every dfa state. The default-transition is only filled, when any
character of the entire character-range results in a transition, and is set to
the transition with the most characters in its class.

For example, the regex "[^\"]*" causes a dfa-state with two transitions:
On '"', go to state x, on every character other than '"', go to state y.
y will be selected as default state.
*/
static void pregex_dfa_default_trans( pregex_dfa* dfa )
{
	LIST*			l;
	LIST*			m;
	pregex_dfa_st*	st;
	pregex_dfa_tr*	tr;
	int				max;
	int				all;
	int				cnt;

	PROC( "pregex_dfa_default_trans" );
	PARMS( "dfa", "%p", dfa );

	LISTFOR( dfa->states, l )
	{
		st = (pregex_dfa_st*)list_access( l );

		max = all = 0;
		LISTFOR( st->trans, m )
		{
			tr = (pregex_dfa_tr*)list_access( m );

			if( max < ( cnt = ccl_count( tr->ccl ) ) )
			{
				max = cnt;
				st->def_trans = tr;
			}

			all += cnt;
		}

		if( all != CCL_MAX )
			st->def_trans = (pregex_dfa_tr*)NULL;
	}

	VOIDRET;
}

/** Collects all references by the NFA-states forming a DFA-state, and puts them
into an dynamically allocated array for later re-use.

//st// is the DFA-state, for which references shall be collected.

Returns ERR_OK on success, and an ERR_-define on error.
*/
static int pregex_dfa_collect_ref( pregex_dfa_st* st )
{
	LIST*			l;
	pregex_nfa_st*	nfa_st;
	int				i;

	PROC( "pregex_dfa_collect_ref" );
	PARMS( "st", "%p", st );

	/* References? */
	if( !( st->ref ) )
	{
		/* Find out number of references, check for anchors */
		MSG( "Searching for references in the NFA transitions" );
		LISTFOR( st->nfa_set, l )
		{
			nfa_st = (pregex_nfa_st*)list_access( l );

			if( nfa_st->ref > -1 )
			{
				for( i = 0; i < st->ref_cnt; i++ )
					if( st->ref[i] == nfa_st->ref )
						break;

				if( i < st->ref_cnt )
					continue;

				if( !st->ref )
					st->ref = (int*)pmalloc(
								PREGEX_ALLOC_STEP * sizeof( int ) );
				else if( st->ref_cnt % PREGEX_ALLOC_STEP )
					st->ref = (int*)prealloc( (void*)st->ref,
								( st->ref_cnt + PREGEX_ALLOC_STEP )
									* sizeof( int ) );

				if( !st->ref )
					RETURN( ERR_MEM );

				st->ref[ st->ref_cnt++ ] = nfa_st->ref;
			}
		}
	}

	RETURN( ERR_OK );
}

/** Turns a NFA-state machine into a DFA-state machine using the
subset-construction algorithm.

//dfa// is the pointer to the DFA-machine that will be constructed by this
function. The pointer is set to zero before it is used.
//nfa// is the pointer to the NFA-Machine where the DFA-machine should be
constructed from.

Returns the number of DFA states that where constructed.
In case of an error, an adequate ERR_-define will be returned.
*/
int pregex_dfa_from_nfa( pregex_dfa* dfa, pregex_nfa* nfa )
{
	LIST*			set;
	LIST*			transitions;
	LIST*			item;
	LIST*			tr;
	LIST*			classes;
	LIST*			l;
	LIST*			m;
	pregex_dfa_tr*	trans;
	pregex_dfa_st*	current;
	pregex_dfa_st*	tmp;
	pregex_nfa_st*	nfa_st;
	int				state_next	= 0;
	pboolean		changed;
	CCL				i;
	CCL				ccl;
	CCL				test;
	CCL				del;
	CCL				subset;

	PROC( "pregex_dfa_from_nfa" );
	PARMS( "dfa", "%p", dfa );
	PARMS( "nfa", "%p", nfa );

	/* Initialize */
	memset( dfa, 0, sizeof( pregex_dfa ) );

	if( !( current = pregex_dfa_create_state( dfa ) ) )
		RETURN( ERR_MEM );

	/* Set starting seed */
	if( !( set = list_push( (LIST*)NULL, list_access( nfa->states ) ) ) )
		RETURN( ERR_MEM );

	current->nfa_set = pregex_nfa_epsilon_closure( nfa, set,
							(pregex_accept*)NULL );
	pregex_dfa_collect_ref( current );

	/* Perform algorithm until all states are done */
	while( ( current = pregex_dfa_get_undone_state( dfa ) ) )
	{
		current->done = TRUE;
		current->accept.accept = PREGEX_ACCEPT_NONE;

		/* Assemble all character sets in the alphabet list */
		classes = (LIST*)NULL;

		for( item = current->nfa_set; item; item = list_next( item ) )
		{
			nfa_st = (pregex_nfa_st*)list_access( item );

			if( nfa_st->accept.accept > PREGEX_ACCEPT_NONE )
			{
				MSG( "NFA is an accepting state" );
				if( current->accept.accept == PREGEX_ACCEPT_NONE
					|| current->accept.accept >= nfa_st->accept.accept )
				{
					MSG( "Copying accept information" );
					memcpy( &( current->accept ), &( nfa_st->accept ),
								sizeof( pregex_accept ) );
				}
			}

			/* Generate list of character classes */
			VARS( "nfa_st->ccl", "%p", nfa_st->ccl );
			VARS( "classes", "%p", classes );

			if( nfa_st->ccl )
			{
				MSG( "Adding character class to list" );
				if( !( ccl = ccl_dup( nfa_st->ccl ) ) )
					RETURN( ERR_MEM );

				if( !( classes = list_push( classes, (void*)ccl ) ) )
					RETURN( ERR_MEM );

				VARS( "count (classes)", "%d", list_count( classes ) );
			}
		}

		VARS( "current->accept", "%d", current->accept );

		MSG( "Removing intersections within character classes" );
		do
		{
			changed = FALSE;

			LISTFOR( classes, l )
			{
				ccl = (CCL)list_access( l );

				LISTFOR( classes, m )
				{
					if( l == m )
						continue;

					test = (CCL)list_access( m );

					if( ccl_count( ccl ) > ccl_count( test ) )
						continue;

					if( ccl_size( ( subset = ccl_intersect( ccl, test ) ) ) )
					{
						test = ccl_diff( test, subset );

						del = (CCL)list_access( m );
						ccl_free( del );

						list_replace( m, test );

						changed = TRUE;
					}

					ccl_free( subset );
				}
			}

			VARS( "changed", "%s", BOOLEAN_STR( changed ) );
		}
		while( changed );

		MSG( "Make transitions on constructed alphabet" );
		/* Make transitions on constructed alphabet */
		LISTFOR( classes, l )
		{
			ccl = (CCL)list_access( l );

			MSG( "Check char class" );
			for( i = ccl; i && i->begin != CCL_MAX; i++ )
			{
				VARS( "i->begin", "%d", i->begin );
				VARS( "i->end", "%d", i->end );

				if( !( transitions = list_dup( current->nfa_set ) ) )
					RETURN( ERR_MEM );

				if( ( transitions = pregex_nfa_move(
						nfa, transitions, i->begin, i->end ) ) )
				{
					transitions = pregex_nfa_epsilon_closure(
						nfa, transitions, (pregex_accept*)NULL );
				}

				if( !transitions )
				{
					/* There is no move on this character! */
					MSG( "transition set is empty, will continue" );

					ccl_free( ccl );
					continue;
				}
				else if( ( state_next = pregex_dfa_same_transitions(
					dfa, transitions ) ) >= 0 )
				{
					/* This transition is already existing in the DFA
						- discard the transition table! */
					transitions = list_free( transitions );
				}
				else
				{
					MSG( "Creating new DFA state" );
					/* Create a new DFA as undone with this transition! */
					if( !( tmp = pregex_dfa_create_state( dfa ) ) )
						RETURN( ERR_MEM );

					tmp->nfa_set = transitions;
					transitions = (LIST*)NULL;

					pregex_dfa_collect_ref( tmp );

					state_next = list_count( dfa->states ) - 1;
				}

				VARS( "state_next", "%d", state_next );

				/* Find transition entry with same follow state */
				for( tr = current->trans; tr; tr = list_next( tr ) )
				{
					trans = (pregex_dfa_tr*)list_access( tr );

					if( trans->go_to == state_next )
						break;
				}

				VARS( "tr", "%p", tr );

				if( !tr )
				{
					MSG( "Need to create new transition entry" );

					/* Set the transition into the transition state matrix... */
					if( !( trans = (pregex_dfa_tr*)pmalloc(
								sizeof( pregex_dfa_tr ) ) ) )
						RETURN( ERR_MEM );

					memset( trans, 0, sizeof( pregex_dfa_tr ) );

					if( !( trans->ccl = ccl_addrange(
							trans->ccl, i->begin, i->end ) ) )
						RETURN( ERR_MEM );

					trans->go_to = state_next;

					if( !( current->trans = list_push( current->trans,
												(void*)trans ) ) )
						RETURN( ERR_MEM );
				}
				else
				{
					MSG( "Will extend existing transition entry" );

					if( !( trans->ccl = ccl_addrange(
							trans->ccl, i->begin, i->end ) ) )
						RETURN( ERR_MEM );
				}
			}

			ccl_free( ccl );
		}

		list_free( classes );
	}

	/* Remove NFA structs */
	for( item = dfa->states; item; item = list_next( item ) )
	{
		tmp = (pregex_dfa_st*)list_access( item );
		tmp->nfa_set = list_free( tmp->nfa_set );
	}

	/* Set default transitions */
	pregex_dfa_default_trans( dfa );

	/* Maximum number of references does not change */
	dfa->ref_count = nfa->ref_count;

	RETURN( list_count( dfa->states ) );
}

/** Checks for transition equality within two dfa states.

//dfa// is the pointer to DFA state machine.
//groups// is the list of DFA groups
//first// is the first state to check.
//second// is the Second state to check.

Returns TRUE if both states are totally equal, FALSE else.
*/
static pboolean pregex_dfa_equal_states( pregex_dfa* dfa, LIST* groups,
	pregex_dfa_st* first, pregex_dfa_st* second )
{
	LIST*			l;
	LIST*			m;
	LIST*			n;
	pregex_dfa_tr*	f;
	pregex_dfa_tr*	s;

	PROC( "pregex_dfa_equal_states" );
	PARMS( "first", "%p", first );
	PARMS( "second", "%p", second );

	if( list_count( first->trans ) != list_count( second->trans ) )
	{
		MSG( "Number of transitions is different" );
		RETURN( FALSE );
	}

	for( l = first->trans, m = second->trans; l && m;
			l = list_next( l ), m = list_next( m ) )
	{
		f = (pregex_dfa_tr*)list_access( l );
		s = (pregex_dfa_tr*)list_access( m );

		/* Equal goto state? */
		/*
		if( f->go_to != s->go_to )
			RETURN( FALSE );
		*/

		/* Equal Character class selection? */
		if( ccl_compare( f->ccl, s->ccl ) )
		{
			MSG( "Character classes are not equal" );
			RETURN( FALSE );
		}

		/* Search for goto-state group equality */
		first = (pregex_dfa_st*)list_getptr( dfa->states, f->go_to );
		second = (pregex_dfa_st*)list_getptr( dfa->states, s->go_to );

		for( n = groups; n; n = list_next( n ) )
		{
			if( list_find( (LIST*)list_access( n ), first ) >= 0
				&& list_find( (LIST*)list_access( n ), second ) < 0 )
			{
				MSG( "Transition state are in different groups\n" );
				RETURN( FALSE );
			}
		}
	}

	RETURN( TRUE );
}

/** Minimizes a DFA to lesser states by grouping equivalent states to new
states, and transforming transitions to them.

//dfa// is the pointer to the DFA-machine that will be minimized. The content of
//dfa// will be replaced with the reduced machine.

Returns the number of DFA states that where constructed in the minimized version
of //dfa//. Returns an ERR_-define on error.
*/
int pregex_dfa_minimize( pregex_dfa* dfa )
{
	pregex_dfa		min_dfa;
	pregex_dfa_st*	dfa_st;
	pregex_dfa_st*	grp_dfa_st;
	pregex_dfa_tr*	ent;
	LIST*			l;
	LIST*			m;
	LIST*			n;
	LIST*			groups		= (LIST*)NULL;
	LIST*			group;
	LIST*			newgroup;
	LIST*			first;
	LIST*			next;
	LIST*			next_next;
	int				i;
	pboolean		changes		= TRUE;

	PROC( "pregex_dfa_minimize" );
	PARMS( "min_dfa", "%p", min_dfa );
	PARMS( "dfa", "%p", dfa );

	memset( &min_dfa, 0, sizeof( pregex_dfa ) );

	/* First, all states are grouped by accepting id */
	for( l = dfa->states; l; l = list_next( l ) )
	{
		dfa_st = (pregex_dfa_st*)list_access( l );

		for( m = groups; m; m = list_next( m ) )
		{
			group = (LIST*)list_access( m );
			grp_dfa_st = (pregex_dfa_st*)list_access( group );

			if( grp_dfa_st->accept.accept == dfa_st->accept.accept )
				break;
		}

		if( !m )
		{
			if( !( group = list_push( (LIST*)NULL, (void*)dfa_st ) ) )
				RETURN( ERR_MEM );

			if( !( groups = list_push( groups, (void*)group ) ) )
				RETURN( ERR_MEM );
		}
		else if( !( list_push( group, (void*)dfa_st ) ) )
			RETURN( ERR_MEM );
	}

	/* Perform the algorithm */
	while( changes )
	{
		changes = FALSE;

		for( l = groups; l; l = list_next( l ) )
		{
			first = (LIST*)list_access( l );
			dfa_st = (pregex_dfa_st*)list_access( first );

			newgroup = (LIST*)NULL;
			next_next = list_next( first );

			while( ( next = next_next ) )
			{
				next_next = list_next( next );
				grp_dfa_st = (pregex_dfa_st*)list_access( next );

				VARS( "dfa_st", "%p", dfa_st );
				VARS( "grp_dfa_st", "%p", grp_dfa_st );
				VARS( "dfa_st->trans", "%p", dfa_st->trans );
				VARS( "grp_dfa_st->trans", "%p", grp_dfa_st->trans );

				/* Perform first test: Same transition count? */
				if( !pregex_dfa_equal_states( dfa, groups,
							dfa_st, grp_dfa_st ) )
				{
					MSG( "States aren't equal" );
					MSG( "Removing state from current group" );
					list_replace( l, (void*)list_remove( first,
										(void*)grp_dfa_st ) );

					if( !list_access( l ) )
						groups = list_remove( groups, (void*)first );

					MSG( "Appending state into new group" );
					if( !( newgroup = list_push( newgroup,
								(void*)grp_dfa_st ) ) )
						RETURN( ERR_MEM );
				}
			}

			VARS( "newgroup", "%p", newgroup );
			if( newgroup )
			{
				MSG( "Engaging new group into list of groups" );
				if( !( groups = list_push( groups, (void*)newgroup ) ) )
					return ERR_MEM;

				changes = TRUE;
			}
		}

		VARS( "changes", "%d", changes );
	}

	/* Now that we have all groups, reduce each group to one state */
	for( l = groups; l; l = list_next( l ) )
	{
		group = (LIST*)list_access( l );
		grp_dfa_st = (pregex_dfa_st*)list_access( group );

		for( m = grp_dfa_st->trans; m; m = list_next( m ) )
		{
			ent = (pregex_dfa_tr*)list_access( m );

			dfa_st = (pregex_dfa_st*)list_getptr( dfa->states, ent->go_to );
			for( n = groups, i = 0; n; n = list_next( n ), i++ )
			{
				if( list_find( (LIST*)list_access( n ), (void*)dfa_st ) >= 0 )
				{
					ent->go_to = i;
					break;
				}
			}
		}
	}

	/* Put leading group states into new, minimized dfa state machine */
	for( l = groups; l; l = list_next( l ) )
	{
		group = (LIST*)list_access( l );

		for( m = list_next( group ); m; m = list_next( m ) )
		{
			dfa_st = (pregex_dfa_st*)list_access( m );
			pregex_dfa_delete_state( dfa_st );
		}

		min_dfa.states = list_push( min_dfa.states, list_access( group ) );
		list_free( group );
	}

	list_free( groups );
	list_free( dfa->states );

	memcpy( dfa, &min_dfa, sizeof( pregex_dfa ) );

	/* Set default transitions */
	pregex_dfa_default_trans( dfa );

	RETURN( list_count( dfa->states ) );
}

/** Tries to match a pattern using a DFA-machine.

//dfa// is the DFA state machine to be executed.
//str// is thetest string where the DFA should work on.
//len// is the Length of the match, -1 on error or no match.

Returns PREGEX_ACCEPT_NONE, if no match was found, else the number of the
bestmost (=longes) match.
*/
int pregex_dfa_match( pregex_dfa* dfa, char* str, size_t* len,
		int* anchors, pregex_range** ref, int* ref_count, int flags )
{
	pregex_dfa_st*	dfa_st;
	pregex_dfa_st*	next_dfa_st;
	pregex_dfa_st*	last_accept = (pregex_dfa_st*)NULL;
	pregex_dfa_tr*	ent;
	LIST*			l;
	char*			pstr		= str;
	size_t			plen		= 0;
	pchar			ch;
	int				i;

	PROC( "pregex_dfa_match" );
	PARMS( "dfa", "%p", dfa );
	if( flags & PREGEX_MOD_WCHAR )
		PARMS( "str", "%ls", str );
	else
		PARMS( "str", "%s", str );
	PARMS( "len", "%p", len );
	PARMS( "anchors", "%p", anchors );
	PARMS( "ref", "%p", ref );
	PARMS( "ref_count", "%p", ref_count );
	PARMS( "flags", "%d", flags );

	/* Initialize! */
	if( anchors )
		*anchors = PREGEX_ANCHOR_NONE;

	if( ( i = pregex_ref_init( ref, ref_count, dfa->ref_count, flags ) )
			!= ERR_OK )
		RETURN( i );

	*len = 0;
	dfa_st = (pregex_dfa_st*)list_access( dfa->states );

	while( dfa_st )
	{
		MSG( "At begin of loop" );

		VARS( "dfa_st->accept.accept", "%d", dfa_st->accept.accept );
		if( dfa_st->accept.accept > PREGEX_ACCEPT_NONE )
		{
			MSG( "This state has an accept" );
			last_accept = dfa_st;
			*len = plen;

			if( !( flags & PREGEX_MOD_GREEDY ) )
			{
				VARS( "last_accept->accept.greedy", "%s",
					BOOLEAN_STR( last_accept->accept.greedy ) );
				if(	!last_accept->accept.greedy || ( flags & PREGEX_MOD_NONGREEDY ) )
				{
					MSG( "This match is not greedy, "
							"so matching will stop now" );
					break;
				}
			}
		}

		MSG( "Handling References" );
		if( ref && dfa->ref_count && dfa_st->ref_cnt )
		{
			VARS( "Having ref_cnt", "%d", dfa_st->ref_cnt );

			for( i = 0; i < dfa_st->ref_cnt; i++ )
			{
				VARS( "i", "%d", i );
				VARS( "State", "%d", list_find(
								dfa->states, (void*)dfa_st ) );
				VARS( "dfa_st->ref[i]", "%d", dfa_st->ref[ i ] );

				pregex_ref_update( &( ( *ref )[ dfa_st->ref[ i ] ] ),
									pstr, plen );
			}
		}

		/* Get next character */
		if( flags & PREGEX_MOD_WCHAR )
		{
			VARS( "pstr", "%ls", (pchar*)pstr );
			ch = *((pchar*)pstr);
		}
		else
		{
			VARS( "pstr", "%s", pstr );
#ifdef UTF8
			ch = u8_char( pstr );
#else
			ch = *pstr;
#endif
		}

		VARS( "ch", "%d", ch );

		next_dfa_st = (pregex_dfa_st*)NULL;
		LISTFOR( dfa_st->trans, l )
		{
			ent = (pregex_dfa_tr*)list_access( l );

			if( ccl_test( ent->ccl, ch ) )
			{
				MSG( "Having a character match!" );
				next_dfa_st = (pregex_dfa_st*)list_getptr(
								dfa->states, ent->go_to );
				break;
			}
		}

		if( !next_dfa_st )
		{
			MSG( "No transitions match!" );
			break;
		}

		/* Move to next char */
		if( flags & PREGEX_MOD_WCHAR )
			pstr += sizeof( pchar );
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}

		plen++;
		VARS( "plen", "%ld", plen );

		dfa_st = next_dfa_st;
	}

	MSG( "Done scanning" );

	if( anchors && last_accept )
	{
		*anchors = last_accept->accept.anchors;
		VARS( "*anchors", "%d", *anchors );
	}

	VARS( "*len", "%d", *len );
	VARS( "last_accept->accept.accept", "%d", ( last_accept ?
										last_accept->accept.accept :
											PREGEX_ACCEPT_NONE ) );
	RETURN( ( last_accept ? last_accept->accept.accept : PREGEX_ACCEPT_NONE ) );
}

/*COD_ON*/

