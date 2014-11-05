/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	regex.h
Author:	Jan Max Meyer
Usage:	Header for the pregex object and functions.
----------------------------------------------------------------------------- */

/* Defines */
#define PREGEX_ALLOC_STEP		16
#define PREGEX_MAXREF			32

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
											of type wchar_t (wide character,
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

/* Matching flags */

#define PREGEX_FLAG_NONE		0		/* No flags defined */
#define PREGEX_FLAG_BOL			1		/* Match at begin of line only */
#define PREGEX_FLAG_EOL			2		/* Match at end of line only */
#define PREGEX_FLAG_BOW			4		/* Match at begin of word only */
#define PREGEX_FLAG_EOW			8		/* Match at end of word only */
#define PREGEX_FLAG_GREEDY		16		/* Greedy match, overwrite mode. */
#define PREGEX_FLAG_NONGREEDY	32		/* Nongreedy match, overwrite mode. */

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

/* Callback function */
typedef	int 					(*pregex_fn)( pregex*, pregex_range* );
#define PREGEX_FN_NULL			( (pregex_fn)NULL )

/*
 * Internal Structures
 */

/* Accepting state definition */
struct _regex_accept
{
	int				accept;		/* Accepting ID */
	int				flags;		/* Flags */
};

/* NFA state */
struct _regex_nfa_st
{
	pccl*			ccl;		/* Char-class; if ccl == (pccl*)NULL,
									then this is an epsilon edge */
	pregex_nfa_st*	next;		/* First following NFA-state */
	pregex_nfa_st*	next2;		/* Second following NFA-state */

	pregex_accept	accept;		/* Match parameters */
	int				refs;		/* References flags */
};

/* NFA state machine */
struct _regex_nfa
{
	plist*			states;		/* NFA states */
	int				modifiers;	/* Regex-modifiers */
};

/* DFA transition */
struct _regex_dfa_tr
{
	pccl*			ccl;		/* Matching character range */
	unsigned int	go_to;		/* Go-To state */
};

/* DFA state */
struct _regex_dfa_st
{
	plist*			trans;		/* Transition table row for this DFA state */
	pregex_dfa_tr*	def_trans;	/* If != (pregex_dfa_tr*)NULL, this points
									to a default dfa transition, which covers
									the most character of the entire character
									range */

	pregex_accept	accept;		/* Match parameters */
	int				refs;		/* References flags */

	pboolean		done;		/* Done-Flag */
	plist*			nfa_set;	/* List of closed sets of NFA-states */
};

/* DFA state machine */
struct _regex_dfa
{
	plist*			states;		/* List of dfa-states */
};

/*
 * Patterns
 */

struct _regex_ptn
{
	pregex_ptntype	type;		/* Pattern state element type */
	pccl*			ccl;		/* Character-class for PREGEX_PTN_CHAR */

	pregex_ptn*		child[ 2 ];	/* Links to child */
	pregex_ptn*		next;		/* Next sequence element */

	pregex_accept*	accept;		/* Optional accepting information */
};

struct _regex_range
{
	char*			begin;		/* Begin pointer */
	wchar_t*		pbegin;		/* Wide-character begin pointer */
	char*			end;		/* End pointer */
	wchar_t*		pend;		/* Wide-character end pointer */
	size_t			pos;		/* Position from string begin in bytes */
	size_t			len;		/* Length of result in bytes */
	int				accept;		/* The ID of the accepting state;
									This is only filled in a pattern match */
	void*			user;		/* User data pointer; This can be set from
									within callback-functions and is copied
									into elements of result-descriptors;
									It is also used in replace-function-call-
									backs to point to a replacement string! */
};

/*
 * Userspace Objects
 */

/* The pregex object structure */
struct _regex
{
	int				flags;		/* Flags */
	pregex_ptn*		ptn;		/* Regex pattern */

	int				trans_cnt;	/* Counts of DFA states */
	wchar_t**		trans;		/* DFA transitions */

	pregex_range	ref			[ PREGEX_MAXREF ];
};


/* The pregex object structure */
struct _lex
{
	/* Definition elements */

	short			stat;		/* Current regex status */
	int				flags;		/* Compile- and runtime flags */

	plist*			patterns;	/* List of pattern definitions
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
	pboolean		completed;	/* Flag, if the regex request was completed. */
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

