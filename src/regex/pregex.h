/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2009-2013 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	pregex.h
Author:	Jan Max Meyer
Usage:	Header for the pregex object and functions
----------------------------------------------------------------------------- */

/* Defines */
#define PREGEX_ACCEPT_NONE		-1

#define PREGEX_ALLOC_STEP		16

/* Regular expression compile states */
#define PREGEX_STAT_NONE		0
#define PREGEX_STAT_NFA			1
#define PREGEX_STAT_DFA			2

/* Regex Modifiers */
#define PREGEX_MOD_NONE			0		/* 	No modification (for the sake
											of completeness) */
#define PREGEX_MOD_WCHAR		1		/* 	Regular expression and/or
											search string for direct
											pattern executions are
											of type pchar (wide character,
											if UNICODE is flagged!) */
#define PREGEX_MOD_INSENSITIVE	2		/* 	Regular expression is case
											insensitive */
#define PREGEX_MOD_GLOBAL		4		/* 	Regular expression is run
											globally, not only for the first
											match */
#define PREGEX_MOD_STATIC		8		/*	The regular expression passed
											to the compiler should be converted
											1:1 as it where a string-constant.
											Any regex-specific symbols will
											be ignored. */
#define PREGEX_MOD_NO_REF		16		/*	Don't create references */
#define PREGEX_MOD_NO_ERRORS	32		/*	Don't report errors, and try to
											compile as much as possible */
#define PREGEX_MOD_NO_ANCHORS	64		/* 	Ignore anchor tokens, handle them
											as normal characters */
#define PREGEX_MOD_GREEDY		128		/*	Run regular expression greedy */
#define PREGEX_MOD_NONGREEDY	256		/*	Run regular expression nongreedy */
#define PREGEX_MOD_DEBUG		1024 	/*	Debug mode; output some debug to
											stderr */

/* Regular Expression anchors */
#define PREGEX_ANCHOR_NONE		0	/* No anchor defined */
#define PREGEX_ANCHOR_BOL		1	/* Begin of line */
#define PREGEX_ANCHOR_EOL		2	/* End of line */
#define PREGEX_ANCHOR_BOW		4	/* Begin of word */
#define PREGEX_ANCHOR_EOW		8	/* End of word */

/* Regular Expression pattern types */
enum _regex_ptntype
{
	PREGEX_PTN_NULL,
	PREGEX_PTN_CHAR,
	PREGEX_PTN_SUB,
	PREGEX_PTN_ALT,
	PREGEX_PTN_KLE,
	PREGEX_PTN_POS,
	PREGEX_PTN_OPT
};

/* Typedefs */
typedef struct	_regex_cr		pregex_cr;
typedef struct	_regex_ccl		pregex_ccl;

typedef struct	_regex_accept	pregex_accept;

typedef struct	_regex_nfa_st	pregex_nfa_st;
typedef struct	_regex_nfa		pregex_nfa;

typedef struct	_regex_dfa_tr	pregex_dfa_tr;
typedef struct	_regex_dfa_st	pregex_dfa_st;
typedef struct	_regex_dfa		pregex_dfa;

typedef enum 	_regex_ptntype	pregex_ptntype;
typedef struct	_regex_ptn		pregex_ptn;

typedef struct	_regex			pregex;
typedef	struct	_regex_range	pregex_range;

typedef struct 	_regex_in		pregex_in;

/* Callback function */
typedef	int 					(*pregex_fn)( pregex*, pregex_range* );
#define PREGEX_FN_NULL			( (pregex_fn)NULL )


/* Character class */
#define PREGEX_CCL_MIN			0x0
#if UNICODE
#define PREGEX_CCL_MAX			0xffff
#else
#define PREGEX_CCL_MAX			0xff
#endif

struct _regex_cr
{
	pchar			begin;
	pchar			end;
};

struct _regex_ccl
{
	int				min;
	int				max;

	plist*			ranges;

	char*			str;
};

/* Accepting state definition */
struct _regex_accept
{
	int				accept;		/* Accepting state ID */
	pboolean		greedy;		/* Greedyness */
	int				anchors;	/* State anchors */
};

/* NFA state */
struct _regex_nfa_st
{
	pregex_ccl*		ccl;		/* Char-class; if ccl == (pregex_ccl*)NULL,
									then this is an epsilon edge */
	pregex_nfa_st*	next;		/* First following NFA-state */
	pregex_nfa_st*	next2;		/* Second following NFA-state */

	int				ref;		/* Reference level depth */

	pregex_accept	accept;		/* Match parameters */
};

/* NFA state machine */
struct _regex_nfa
{
	LIST*			states;		/* List of nfa-states */
	LIST*			empty;		/* List to pointers of states to be used */
	int				modifiers;	/* Regex-modifiers */

	int				ref_count;	/* Number of references */
	int				ref_cur;	/* Current reference */
};

/* DFA transition */
struct _regex_dfa_tr
{
	pregex_ccl*		ccl;		/* Matching character range */
	unsigned int	go_to;		/* Go-To state */
};

/* DFA state */
struct _regex_dfa_st
{
	LIST*			trans;		/* Transition table row for this DFA state */
	pregex_dfa_tr*	def_trans;	/* If != (pregex_dfa_tr*)NULL, this points
									to a default dfa transition, which covers
									the most character of the entire character
									range */
	int*			ref;		/* Reference level depths */
	int				ref_cnt;	/* Number of reference level depths */

	pregex_accept	accept;		/* Match parameters */

	pboolean		done;		/* Done-Flag */
	LIST*			nfa_set;	/* List of closed sets of NFA-states */
};

/* DFA state machine */
struct _regex_dfa
{
	LIST*			states;		/* List of dfa-states */
	int				ref_count;	/* Number of references */
};

/*
 * Patterns
 */

struct _regex_ptn
{
	pregex_ptntype	type;		/* Pattern state element type */
	pregex_ccl*		ccl;		/* Character-class for PREGEX_PTN_CHAR */

	pregex_ptn*		child[ 2 ];	/* Links to child */
	pregex_ptn*		next;		/* Next sequence element */

	pregex_accept*	accept;		/* Optional accepting information */
};

struct _regex_range
{
	char*			begin;		/* Begin pointer */
	pchar*			pbegin;		/* Wide-character begin pointer */
	char*			end;		/* End pointer */
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

/* The pregex object structure */
struct _regex
{
	/* Definition elements */

	short			stat;		/* Current regex status */
	int				flags;		/* Compile- and runtime flags */

	LIST*			defs;		/* List of pattern definitions
									holding the patterns */

	union
	{
		pregex_nfa*	nfa;		/* NFA state machine */
		pregex_dfa*	dfa;		/* DFA state machine */
	} machine;

	/* Runtime elements */

	int				age;		/* The objects age; This value is incremented
									each time a change to the object that
									affects its runtime validity. */

	pregex_fn		match_fn;	/* A match function to be invoked
									at every match */

	/* Ephemerial elements */
	int				last_err;	/* Last error code (one of the Phorward
									standard error level defines */
	int				match_count;/* Number of total matches since last
									match function restart */
	int				last_age;	/* Age of the pregex object at the last
									match function restart */

	char*			last_str;	/* Begin pointer of last string that was
									analyzed */
	char*			last_pos;	/* Holds last string position within
									multiple matches */

	pregex_range	range;		/* Holds the last pattern match range */
	pregex_range	split;		/* Holds the last pattern split range,
									if the split function has been invoked */

	pregex_range*	refs;		/* Holds all references within the last
									pattern match range */
	int				refs_cnt;	/* The number of references; This remains
									constant, unless other expressions are
									added to the pregex-object */

	char*			tmp_str;	/* Temporary string pointer that is bound
									to the object */
};

