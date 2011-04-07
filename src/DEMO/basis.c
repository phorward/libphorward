/*
	This program is a short demostration of the functions
	provided by the Phorward Basis Libray. It covers not all
	available functions, but the really useful ones! ;)
*/

#include <pbasis.h>

/* Required for stack functions */
struct person
{
	uchar	first_name	[ 30 + 1 ];
	uchar	last_name	[ 30 + 1 ];
};

/* Extended String Functions */
void string_demo( void )
{
	uchar	str[ 1024 ];
	int	all, i;
	uchar**	tokens;
	uchar*	str1;
	uchar*	str2;

	printf( "\n*** string_demo ***\n\n" );

	/* Dynamic string functions */
	str1 = pstrdup( "Hello " );
	str1 = pstr_append_str( str1, "World", FALSE );
	str1 = pstr_append_char( str1, '!' );

	str2 = pstr_replace( str1, "World", "out there" );

	printf( "str1 = >%s<\n", str1 );
	printf( "str2 = >%s<\n", str2 );
	pfree( str1 ); /* str2 will be freed within next function call! */

	/* String rendering from templates */
	str1 = pstr_render( "<a href=\"$link\" alt=\"$title\">$title</a>",
			"$link", "http://www.phorward-software.com", FALSE,
			"$title", str2, TRUE,
			(uchar*)NULL );

	printf( "str1 = >%s<\n", str1 );
	pfree( str1 );
	
	/* String Tokenizer */
	pstrcpy( str, "Hello World, this is a simple test" );
	all = pstr_tok( &tokens, str, " ", 3 );
	printf( "%d\n", all );

	for( i = 0; i < all; i++ )
		printf( "%d: >%s<\n", i, tokens[i] );

	pfree( tokens );
}

/* UTF-8 Functions */
void utf8_demo( void )
{
	uchar	str		[ 1024 ];
	uchar*	ptr;

	printf( "\n*** utf8_demo ***\n\n" );

	pstrcpy( str, "Hällö ich bün ein StrÜng€!" );
	/*             123456789012345678901234567890
	               0        1         2         3
	*/
	printf( "%ld %d\n", pstrlen( str ), u8_strlen( str ) );
	for( ptr = str; *ptr; ptr += u8_seqlen( ptr ) )
		printf( ">%s< %d\n", ptr, u8_char( ptr ) );

	for( ptr = str; *ptr; ptr++ )
		printf( ">%s< >%c< %x %d\n", ptr, *ptr, *ptr, *ptr );

	u8_unescape( str, sizeof( str ), "\\u20AC" );
	printf( ">%s< %d\n", str, u8_char( str ) );
}

/* Unicode-Function replacements */
void unicode_demo( void )
{
	pchar	mystr		[ 255 ];
	pchar*	mydynamicstr;

	printf( "\n*** unicode_demo ***\n\n" );

	Pstrcpy( mystr, L"Yes, w€ cän üse standard C function "
			L"names for Unicode-strings!" );

	
	printf( "mystr = >%ls<\n", mystr );
	Psprintf( mystr, L"This string was %d characters long!",
			Pstrlen( mystr ) );
	printf( "mystr = >%ls<\n", mystr );

	mydynamicstr = Pstrdup( mystr );
	mydynamicstr = Pstr_append_str( mydynamicstr,
			L" You can see: The functions are"
			L" used the same way as the standard"
			L" uchar-functions!", FALSE );

	printf( "mydynamicstr = >%ls<\n", mydynamicstr );
	pfree( mydynamicstr );
}

void ccl_demo( void )
{
	CCL c;
	CCL d;
	uchar* x;

	printf( "\n*** ccl_demo ***\n\n" );

	/* c = ccl_create( "\\1-\\n€A-ZX-dÄÜÖ" ); */
	c = ccl_create( "^ €A-Z\n" );
	d = ccl_create( "A-NXYZ\n" );
	
	d = ccl_create( "^alles richtig! :)" );
	ccl_print( stderr, c, -1 );
	ccl_print( stderr, d, -1 );

	c = ccl_union( c, d );
	ccl_print( stderr, c, -1 );

	ccl_negate( c );
	ccl_print( stderr, c, -1 );

	ccl_negate( c );
	ccl_print( stderr, c, -1 );
	
	/* e = ccl_intersect( c, d ); */
	d = ccl_delrange( d, '\0', CCL_MAX );

	x = ccl_to_str( d, TRUE );
	printf( "e = >%s<\n", x );

	pfree( x );
	ccl_free( c );
	ccl_free( d );
}

void list_demo( void )
{
	LIST*	mylist = (LIST*)NULL; /* This is our main list */
	LIST*	l; /* l, for list iteration */
	uchar*	values[] = { "Hello", "World", "out there!" };
	uchar*	tmp;

	printf( "\n*** list_demo ***\n\n" );
	
	/* Create the list. */
	mylist = list_push( mylist, (void*)values[0] );
	mylist = list_push( mylist, (void*)values[1] );
	mylist = list_push( mylist, (void*)values[2] );
	
	/* Let's iterate it. */
	printf( "mylist contains %d items\n", list_count( mylist ) );
	for( l = mylist; l; l = list_next( l ) )
	{
		tmp = (uchar*)list_access( l );
		printf( "%s ", tmp );
	}
	
	/* Now, we remove one element (identified by its pointer) and iterate
		the list again */
	mylist = list_remove( mylist, (void*)values[1] );
	printf( "\nmylist contains now %d items\n", list_count( mylist ) );

	/* LISTFOR expands in a for-loop as above, but is shorter! ;) */
	LISTFOR( mylist, l )
	{
		tmp = (uchar*)list_access( l );
		printf( "%s ", tmp );
	}
	
	printf( "\n" );
	
	/* Free the entire list */
	mylist = list_free( mylist );
}


void hashtab_demo( void )
{
	HASHTAB		ht;
	HASHELEM*	e;
	LIST*		l;

	printf( "\n*** hashtab_demo ***\n\n" );
	
	/* We configure a hash-table with 6 buckets, wide-character enabled and
		list-feature enabled */
	hashtab_init( &ht, 6, HASHTAB_MOD_WCHAR | HASHTAB_MOD_LIST );
	
	/* Let's insert some data */
	hashtab_insert( &ht, (uchar*)L"Hello", "This is my first string" );
	hashtab_insert( &ht, (uchar*)L"World", "And this is my second one" );
	hashtab_insert( &ht, (uchar*)L"Test", "Last but no least, the third :)" );
	
	/* Get one entry */
	e = hashtab_get( &ht, (uchar*)L"World" );
	printf( ">%s<\n", (uchar*)hashelem_access( e ) );
	printf( "%d items in table\n", hashtab_count( &ht ) );
	
	/* Loop trough the entries, without any order */
	for( e = hashtab_fetch( &ht, (HASHELEM*)NULL );
			e; e = hashtab_fetch( &ht, e ) )
		printf( ">%ls<: >%s<\n",
			(pchar*)hashelem_key( e ),
				(uchar*)hashelem_access( e ) );
		
	/* Loop trough the entries using the list */
	LISTFOR( hashtab_list( &ht ), l )
	{
		e = (HASHELEM*)list_access( l );
		printf( ">%ls<: >%s<\n",
			(pchar*)hashelem_key( e ),
				(uchar*)hashelem_access( e ) );
	}
	
	/* Dump the table to stderr */
	hashtab_print( &ht, stderr );
	
	/* Discard the element with the key "World" */
	hashtab_discard( &ht, (uchar*)L"World", HASHTAB_NO_CALLBACK );
	
	/* Print again! */
	hashtab_print( &ht, stderr );
	
	/* Free entire table, reset all */
	hashtab_free( &ht, HASHTAB_NO_CALLBACK );
}

static void stack_demo_callback( struct person* p )
{
	fprintf( stderr, "--- %s, %s\n", p->last_name, p->first_name );
}

void stack_demo( void )
{
	struct person x;
	struct person* p;

	STACK	s;

	printf( "\n*** stack_demo ***\n\n" );

	stack_init( &s, sizeof( struct person ), 3 );
	
	pstrcpy( x.last_name, "Zufall" );
	pstrcpy( x.first_name, "Reiner" );
	stack_push( &s, (pbyte*)&x );
	
	pstrcpy( x.last_name, "Pfahl" );
	pstrcpy( x.first_name, "Martha" );
	stack_push( &s, (pbyte*)&x );
	
	pstrcpy( x.last_name, "Racho" );
	pstrcpy( x.first_name, "Volker" );
	stack_push( &s, (pbyte*)&x );
	
	pstrcpy( x.last_name, "Pete" );
	pstrcpy( x.first_name, "Dieter" );
	stack_push( &s, (pbyte*)&x );

	stack_dump( __FILE__, __LINE__, "s", &s,
		(STACK_CALLBACK)stack_demo_callback );
	
	while( ( p = (struct person*)stack_pop( &s ) ) )
		fprintf( stderr, "Pop: %s %s\n", p->first_name, p->last_name );

	stack_free( &s, (STACK_CALLBACK)stack_demo_callback );
}

int dbg_demo( int x )
{
	int ret;
	
	PROC( "dbg_demo" );
	PARMS( "x", "%d", x );

	if( x < 0 )
	{
		MSG( "Not allowed!" );
		RETURN( ERR_PARMS );
	}
	else if( x == 0 )
	{
		MSG( "x is 0, so faculty is 1" );
		RETURN( 1 );
	}
	else
	{
		MSG( "Calling dbg_demo recurisvely with:" );
		VARS( "x - 1", "%d", x - 1 );
		ret = x * dbg_demo( x - 1 );
	}

	VARS( "ret", "%d", ret );
	RETURN( ret );
}

int main( int argc, char** argv )
{
	setlocale( LC_ALL, "" );

	string_demo();
	utf8_demo();
	unicode_demo();
	list_demo();
	hashtab_demo();
	stack_demo();
	printf( "faculty of 3 is %d\n", dbg_demo( 3 ) );

	return EXIT_SUCCESS;
}

