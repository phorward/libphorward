/* -HEADER----------------------------------------------------------------------
Phorward Regular Expression Library, Version 2
Copyright (C) 2009 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ mail<at>phorward<dash>software<dot>com

File:	pregex.h
Author:	Jan Max Meyer
Usage:	Header for regex lib
----------------------------------------------------------------------------- */

#ifndef PREGEX_H
#define PREGEX_H

#include <pbasis.h>
#include <pstring.h>

/* Defines */
#define REGEX_ACCEPT_NONE		-1

#define REGEX_ALLOC_STEP		16

/* Regular expression compile states */
#define REGEX_STAT_UNCOMPILED	0
#define REGEX_STAT_COMPILED		1
#define REGEX_STAT_FINALIZED	2

/* Regex Modifiers */
#define REGEX_MOD_NONE			0
#define REGEX_MOD_INSENSITIVE	1
#define REGEX_MOD_MULTILINE		2
#define REGEX_MOD_GLOBAL		4

#ifndef PRIVATE
#define PRIVATE static
#endif


/* Typedefs */
typedef CCL						pregex_ccl;
typedef struct _regex_nfa_st	pregex_nfa_st;
typedef struct _regex_dfa_ent	pregex_dfa_ent;
typedef struct _regex_dfa_st	pregex_dfa_st;
typedef struct _regex_nfa		pregex_nfa;
typedef struct _regex_dfa		pregex_dfa;
typedef struct _regex			pregex;
typedef	struct _regex_result	pregex_result;

/* Structs */
struct _regex_nfa_st
{
	pregex_ccl		ccl;		/* Char-class; if ccl == (pregex_ccl*)NULL,
									then this is an epsilon edge */
	pregex_nfa_st*	next;		/* First following NFA-state */
	pregex_nfa_st*	next2;		/* Second following NFA-state */
	int				accept;		/* Accepting state ID */
	int				ref;
};

struct _regex_nfa
{	
	LIST*			states;		/* List of nfa-states */
	LIST*			empty;		/* List to pointers of states to be used */
	byte			modifiers;	/* Regex-modifiers */
	
	int				ref_count;	/* Number of last reference */
};

struct _regex_dfa_ent
{
	pregex_ccl		ccl;		/* Matching character range */
	unsigned int	go_to;		/* Go-To state */
};

struct _regex_dfa_st
{
	LIST*			trans;		/* Transition table row for this DFA state */
	int				accept;		/* Accepting state and ID */
	BOOLEAN			done;		/* Done-Flag */
	
	LIST*			nfa_set;	/* List of closed sets of NFA-states */
};

struct _regex_dfa
{
	LIST*			states;		/* List of dfa-states */
};

struct _regex
{
	byte			stat;		/* Current regex status */

	union
	{
		pregex_nfa	nfa;
		pregex_dfa	dfa;
	} machine;

	int				flags;		/* Several flags */
};

struct _regex_result
{
	uchar*			begin;
	uchar*			end;
};

/* Prototypes */
#ifndef MAKE_PROTOTYPES
#include "pregex.proto.h"

#ifdef PREGEX_LOCAL
#include "pregex.sproto.h"
#endif

#endif

#endif
