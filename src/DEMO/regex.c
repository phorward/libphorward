/*
	This program is a short demonstration of the Regular Expression capabilities
	that the Phorward Foundation Libraries provides for C programs.
	
	By simply including pbasis.h and pregex.h into your sources and linking with
	pbasis.a and pregex.a, your C programs will be enabled to use an entire
	bunch of useful functions for regular expression matching, replacing and
	splitting, both for ANSI-C and wide-character strings.
	
	The regular expression functions can be used in two ways:
	
	1) The compiler-functions are working with regular expression objects, which
	   are compiled once into an NFA, and then can be used for various subse-
	   quent matching, replacement and splitting operations using these compiled
	   "machines". These functions begin with "pregex_comp_", the regex-objects
	   are represented by the data-type "pregex". Using the compiler-functions
	   lets you use the entire power of the Phorward Regular Expression
	   implementation.
	2) The direct-functions only require a regular expression, which is interna-
	   lly be compiled, executed and freed for the time of the regular express-
	   ion run. They are used to quickly perform a regular-expression-based
	   operating from a C program without creating an object first. A loss of
	   various features is the price to pay when using the direct functions.
	   
	The regular expressions implementation of the Phorward Foundation Libraries
	comes with a build-in parser.

	This build-in parser will parse and recogize the following symbols for
	expressing regular languages:
	
		[ ... ]					Character-class
		( ... )					Sub-expression (in replacements, every
								sub-expression can be accessed in back-
								references)
		|						Or-Operator
		?						Optional-closure
		+						Positive-closure
		*						Kleene-closure
		
	Ordinary Characters are simply taken from the string.
	For special characters, use C-styled character definitions, e.g.

		"\n" 					pre-defined escape sequence,
		"\x40"					hexadecimal escape sequence,
		"\100"					octal escape sequence.

	If the Foundation Libraries are compiled with the UNICODE-flag,
	Unicode-characters can be defined with

		"\u20ac"				for a four-digit hexadecimal code
		"\U0001D49C"			for an eight-digit hexadecimal code
		
	Note, that the "\" must be double-escaped in static C-strings, as "\\".
		
	Anchors are supported. You may use
	
		"^"						Match begin of line (only recognized if speci-
								fied in front of the regular expression).
		"$"						Match end of line (only recognized if speci-
								fied at the end of the regular expression).
		"\<"					Match entire word only (no characters in front
								of match, only recognized if specified in front
								of the regular expression)
		"\>"					Match entire word only (no characters at end
								of match, only recognized if specified in front
								of the regular expression)
		
	The regular expression functions behavior can be influenced by various
	flags, which can be combined using the or-operator. Some of them are:
	
		REGEX_MOD_WCHAR				The regular-expressions and input strings
									shall be handled as wide-character strings.
		REGEX_MOD_INSENSITIVE		Case-insensitive matching. If your
									expression does only work for lower-case
									characters, this will even recognize
									upper-case ones. It also works for special
									characters like Umlauts, depending on your
									locale settings.
		REGEX_MOD_GLOBAL			This causes a regular expression to match on
									an entire string. If not given, the
									functions will stop working after the first
									match.
		REGEX_MOD_STATIC_STRING		Defines, that a regular expression
									definition should be matched as it is. The
									parser is not used, the string will be
									directly turned as it is (including all
									characters for expressing regular languages)
									into a state machine.
		REGEX_MOD_NO_REFERENCES		Don't hold and provide references in
									replacement operations.
		REGEX_MOD_NO_ERRORS			Ignore parse errors in regular expressions.
		REGEX_MOD_NO_ANCHORS		Ignore anchors. Anchor-characters are then
									handled as ordinary characters.
	
	To provide no flag to a regex-function, use the define REGEX_MOD_NONE.
*/

#include <pbasis.h>
#include <pregex.h>

/* =============================================================================
	QUICK REGULAR EXPRESSION USAGE
============================================================================= */
void direct_regex_demo( void )
{
	uchar*			str		=	"Gliding is a recreational activity and "
								"competitive air sport in which pilots fly "
								"unpowered aircraft known as gliders or "
								"sailplanes using naturally occurring currents "
								"of rising air in the atmosphere to remain "
								"airborne. The word soaring is also used for "
								"the sport. When soaring conditions are good "
								"enough, experienced pilots can fly hundreds "
								"of kilometres before returning to their home "
								"airfields; occasionally flights of more than "
								"1,000 kilometres are achieved. However, if "
								"the weather deteriorates, they may need to "
								"land elsewhere, but motorglider pilots can "
								"avoid this by starting an engine. Some "
								"competitive pilots fly in races around "
								"pre-defined courses. These gliding "
								"competitions test pilots' abilities to make "
								"best use of local weather conditions as well "
								"as their flying skills. Local and national "
								"competitions are organized in many countries "
								"and there are biennial World Gliding "
								"Championships. Powered aircraft and winches "
								"are the two most common means of launching "
								"gliders. These and other methods (apart from "
								"self-launching motor-gliders) require"
								"assistance from other participants. Gliding "
								"clubs have thus been established to share "
								"airfields and equipment, train new pilots and "
								"maintain high safety standards.";
								/* Above text is from Wikipedia, and licensed
									under the Creative Commons
										Attribution-ShareAlike License */
								
	pchar*			lstr	=	L"This is the wide-character version of a test "
								L"string to show even the wide-character "
								L"support of our regular expression library! "
								L"Yeahhh! ÖÜÄ€€€öäü€€ß and the €uro-Symbol";
								
								
	pregex_result*	res;
	int				matches;
	int				i;
	uchar*			result;
	pchar*			lresult;

	printf( "\n*** direct_regex_demo ***\n\n" );
	
	/* First, a simple match. We want to match all words that consist of one
		or two characters! */
	matches = pregex_match(	"\\<[A-Za-z_][A-Za-z_]?\\>", str,
								REGEX_MOD_GLOBAL, &res );

	printf( "we got %d matches:\n", matches );
	for( i = 0; i < matches; i++ )
		printf( "  match %d: >%.*s<\n", i, (int)res[i].len, res[i].begin );
		
	/*
		Various functions make use of the pregex_result-structure, as you can
		see above. This structure contains various pointers that describe the
		match of the regular expression. Member variables are:
		
		int	accept			The accepting regular expression that matches a
							value. Using the compiler-functions, multiple
							regular expressions can be compiled with different
							identifying numbers into one state machine. Which
							regular expression matches will be written to this
							member variable.
		uchar* begin		uchar*-Pointer (byte-Pointer) to the begin of the
							match.
		pchar* pbegin		If compiled with UNICODE-flag and the function
							was called with REGEX_MOD_WCHAR, this contains the
							wide-character begin pointer of the match.
		uchar* end			uchar*-Pointer (byte-Pointer) to the end of the
							match.
		pchar* pend			If compiled with UNICODE-flag and the function
							was called with REGEX_MOD_WCHAR, this contains the
							wide-character end pointer of the match.
		psize len			The length of the match. If the string is UTF-8,
							this represents the number of bytes, for
							wide-characters the true number of characters.
		psize pos			The position of the match from the string beginning.
							If the string is UTF-8, this represents the number
							of bytes, for wide-characters the true number of
							characters.
	*/
		
	pfree( res );
	
	/* Now, let's split at each comma and dot, and remove optionally following
		space characters. */
	matches = pregex_split(	"[.,;] ?", str, REGEX_MOD_GLOBAL, &res );

	printf( "we got %d split results:\n", matches );
	for( i = 0; i < matches; i++ )
		printf( "  result %d: >%.*s<\n", i, (int)res[i].len, res[i].begin );
		
	pfree( res );
	
	/* Now, let's replace 'gliding' with an HTML-bold-tag! */
	pregex_replace( "(gliding)", str, "<b>$1</b>",
						REGEX_MOD_GLOBAL | REGEX_MOD_INSENSITIVE, &result );
	printf( "This is the modified string:\n%s\n\n", result );
	pfree( result );
	
	/* Matching with wide-character strings and regular expressions... */
	/* First, a simple match. We want to match all words that consist of one
		or two characters! */
	matches = pregex_replace( (uchar*)L"(wide-character)", (uchar*)lstr,
								(uchar*)L"$1 (also known as 'pchar')",
								REGEX_MOD_GLOBAL | REGEX_MOD_WCHAR,
									(uchar**)&lresult );

	printf( "1) This is the modified wide-character string:"
					"\n%ls\n\n", lresult );
	pfree( lresult );
	
	matches = pregex_replace( (uchar*)L"€+", (uchar*)lstr, (uchar*)L"EUR",
								REGEX_MOD_GLOBAL | REGEX_MOD_WCHAR,
									(uchar**)&lresult );

	printf( "2) This is the modified wide-character string:"
					"\n%ls\n\n", lresult );
	pfree( lresult );
}

/* =============================================================================
	USING COMPILED REGULAR EXPRESSIONS
============================================================================= */

/*
	Using callback functions allows to run individual code on each matched
	token within a match, replace or split function. It allows to modify
	a match, return an entirely different match result or simply declare a 
	match to be invalid, by returning -1. 
*/
static int regex_callback1( pregex_result* res )
{
	/* 
		This is a special case usage: By writing something into the 
		'user'-pointer of the pregex_result-structure, this pointer will
		be used for the replacement instead of the standard replacement
		string.
	*/
	switch( res->accept )
	{
		case 0:
			res->user = (pbyte*)"IDENT";
			break;
		case 1:
			res->user = (pbyte*)"INTEGER";
			break;
		case 2:
			res->user = (pbyte*)"FLOAT";
			break;
			
		default:
			break;
	}
	
	/*
		To modify a match without declaring it invalid or as another identified
		object, always return the original matching identifier which is 
		pre-written to the accept-member of the provided
		pregex_result-structure.
	*/
	return res->accept;
}

static int regex_callback2( pregex_result* res )
{
	if( res->accept < 2 )
		return -1;

	return res->accept;
}

void compiled_regex_demo( void )
{
	pregex			rx;
	uchar*			simple = "Hello WORLD this is 876 a funny 123.3 test .4 :)";
	uchar*			newstr;
	pregex_result*	res;
	int				matches;
	int				i;

	printf( "\n*** compiled_regex_demo ***\n\n" );

	pregex_comp_init( &rx, REGEX_MOD_GLOBAL );
	pregex_comp_compile( &rx, "\\<[A-Z]+\\>", 0 );
	pregex_comp_compile( &rx, "[0-9]+", 1 );
	pregex_comp_compile( &rx, "[0-9]+\\.[0-9]*|[0-9]*\\.[0-9]+", 2 );	
	
	/* First, we're extracting tokens from a string */
	matches = pregex_comp_match( &rx, simple, REGEX_NO_CALLBACK, &res );
	printf( "compiled regex returned %d matches\n", matches );

	for( i = 0; i < matches; i++ )
		printf( "%d: id %d >%.*s<\n", i, res[i].accept,
			(int)res[i].len, res[i].begin );
	
	/* Now, we replace these tokens on the fly, using a callback-function! */
	matches = pregex_comp_replace( &rx, simple,
				(uchar*)NULL, regex_callback1, &newstr );
	printf( "compiled regex returned %d matches, and is: >%s<\n",
				matches, newstr );
	pfree( newstr ); /* the returned string must be freed after usage! */
	
	/* We only want to accept floats in this callback-example run: */
	matches = pregex_comp_match( &rx, simple, regex_callback2, &res );
	printf( "compiled regex returned %d matches\n", matches );

	for( i = 0; i < matches; i++ )
		printf( "%d: id %d >%.*s<\n", i, res[i].accept,
			(int)res[i].len, res[i].begin );
	
	pregex_comp_free( &rx );
}

int main( int argc, char** argv )
{
	setlocale( LC_ALL, "" );
	
	direct_regex_demo();
	compiled_regex_demo();

	return EXIT_SUCCESS;
}

