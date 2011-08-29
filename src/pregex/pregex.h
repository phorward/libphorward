/* -HEADER----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	pregex.h
Author:	Jan Max Meyer
Usage:	Header for regex lib
----------------------------------------------------------------------------- */

#ifndef PREGEX_H
#define PREGEX_H

#include <pbasis.h>

/* Defines */
#define REGEX_ACCEPT_NONE		-1

#define REGEX_ALLOC_STEP		16

/* Regular expression compile states */
#define REGEX_STAT_NONE			0
#define REGEX_STAT_NFA			1
#define REGEX_STAT_DFA			2

/* Regex Modifiers */
#define REGEX_MOD_NONE			0	/* No modification (for the sake
										of completeness) */
#define REGEX_MOD_WCHAR			1	/* Regular expression and/or search string
										for direct pattern executions are
											of type pchar (wide character,
												if UNICODE is flagged!) */
#define REGEX_MOD_INSENSITIVE	2	/* Regular expression is case insensitive */
#define REGEX_MOD_GLOBAL		4	/* Regular expression is run globally, not
										only for the first match */
#define REGEX_MOD_STATIC_STRING	8	/* The regular expression passed for to the
										compiler should be converted 1:1 as
											it where a string-constant.
												Any regex-specific symbols will
												be ignored! */
#define REGEX_MOD_NO_REFERENCES	16	/* Don't create references */
#define REGEX_MOD_NO_ERRORS		32	/* Don't report errors, try to compile as
										much as possible */
#define REGEX_MOD_NO_ANCHORS	64	/* Ignore anchor tokens, handle them as
										normal characters */
#define REGEX_MOD_GREEDY		128	/* Parse or run regular expression
										greedy */
#define REGEX_MOD_NONGREEDY		256	/* Parse or run regular expression
										nongreedy */

/* Regular Expression anchors */
#define REGEX_ANCHOR_NONE		0	/* No anchor defined */
#define REGEX_ANCHOR_BOL		1	/* Begin of line */
#define REGEX_ANCHOR_EOL		2	/* End of line */
#define REGEX_ANCHOR_BOW		4	/* Begin of word */
#define REGEX_ANCHOR_EOW		8	/* End of word */

#ifndef PRIVATE
#define PRIVATE static
#endif

/* Typedefs */
typedef CCL						pregex_ccl;
typedef struct _regex_nfa_st	pregex_nfa_st;
typedef struct _regex_dfa_tr	pregex_dfa_tr;
typedef struct _regex_dfa_st	pregex_dfa_st;
typedef struct _regex_nfa		pregex_nfa;
typedef struct _regex_dfa		pregex_dfa;
typedef struct _regex			pregex;
typedef	struct _regex_result	pregex_result;

/* Callback-Functions */
typedef	int 					(*pregex_callback)( pregex_result* );
#define REGEX_NO_CALLBACK	( (pregex_callback)NULL )


/* Structs */
struct _regex_nfa_st
{
	pregex_ccl		ccl;		/* Char-class; if ccl == (pregex_ccl*)NULL,
									then this is an epsilon edge */
	pregex_nfa_st*	next;		/* First following NFA-state */
	pregex_nfa_st*	next2;		/* Second following NFA-state */

	int				accept;		/* Accepting state ID */
	BOOLEAN			greedy;		/* Greedyness */

	int				ref;		/* Reference level depth */
	int				anchor;		/* State anchor */
};

struct _regex_nfa
{	
	LIST*			states;		/* List of nfa-states */
	LIST*			empty;		/* List to pointers of states to be used */
	pbyte			modifiers;	/* Regex-modifiers */
	
	int				ref_count;	/* Number of references */
	int				ref_cur;	/* Current reference */
};

struct _regex_dfa_tr
{
	pregex_ccl		ccl;		/* Matching character range */
	unsigned int	go_to;		/* Go-To state */
};

struct _regex_dfa_st
{
	LIST*			trans;		/* Transition table row for this DFA state */
	pregex_dfa_tr*	def_trans;	/* If != (pregex_dfa_tr*)NULL, this points
									to a default dfa transition, which covers
									the most character of the entire character
									range */
	int				accept;		/* Accepting state and ID */
	int*			ref;		/* Reference level depths */
	int				ref_cnt;	/* Number of reference level depths */

	int				anchor;		/* State anchoring */
	BOOLEAN			greedy;		/* Greedyness */

	BOOLEAN			done;		/* Done-Flag */
	LIST*			nfa_set;	/* List of closed sets of NFA-states */
};

struct _regex_dfa
{
	LIST*			states;		/* List of dfa-states */
	int				ref_count;	/* Number of references */
};

struct _regex
{
	byte			stat;		/* Current regex status */

	union
	{
		pregex_nfa	nfa;

		/* For later extensions... */
		pregex_dfa	dfa;
	} machine;

	int				flags;		/* Several flags */
};

struct _regex_result
{
	uchar*			begin;		/* Begin pointer */
	pchar*			pbegin;		/* Wide-character begin pointer */
	uchar*			end;		/* End pointer */
	pchar*			pend;		/* Wide-character end pointer */
	psize			pos;		/* Position from string begin in bytes */
	psize			len;		/* Length of result in bytes */
	int				accept;		/* The ID of the accepting state;	
									This is only filled in a pattern match */
	pbyte*			user;		/* User data pointer; This can be set from
									within callback-functions and is copied
									into elements of result-descriptors;
									It is also used in replace-function-call-
									backs to point to a replacement string! */
};

/* Prototypes */
#ifndef MAKE_PROTOTYPES
#include "pregex.proto.h"

#ifdef PREGEX_LOCAL
#include "pregex.sproto.h"
#endif

#endif

#endif
