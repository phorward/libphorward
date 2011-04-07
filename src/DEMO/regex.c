/*
	This program is a short demonstration of the Regular Expression capabilities
	the Phorward Foundation Libraries provide for C programs.
	
	By simply including pbasis.h and pregex.h into your sources and linking with
	pbasis.a and pregex.a, your C programs will be enabled to use an entire
	bunch of useful functions for regular expression matching, replacing and
	splitting, both for ANSI-C and wide-character strings.
	
	The regular expression functions are provided in two ways:
	
	1) The compiler-functions are working with regular expression objects, which
	   are compiled once into an NFA, and then can be used for various applica-
	   tions. They begin with "pregex_comp_", the regex-objects are represented
	   by the data-type "pregex". Using the compiler-functions lets you use the
	   full power of the Phorward Regular Expression impelementation.
	2) The direct-functions only require a regular expression, which is interna-
	   lly be compiled, executed and freed for the time of the regular express-
	   ion run. They are used to quickly run a regular-expression from a C pro-
	   gram without creating an object first. A loss of various features is the
	   price to pay when using the direct functions.
	   
	Please take a look now, at the examples below!
*/

#include <pbasis.h>
#include <pregex.h>


/*
	Functions for direct regular expression use:
	
*/
void direct_regex_demo( void )
{
	printf( "\n*** direct_regex_demo ***\n\n" );
}

int main( int argc, char** argv )
{
	setlocale( LC_ALL, "" );


	return EXIT_SUCCESS;
}

