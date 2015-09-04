/*
	This program is a short demostration of the functions
	provided by the Phorward Base Libray. It covers not all
	available functions, but the really useful ones! ;)
*/

#include <phorward.h>

#define DEMO( name )	printf( "\n*** %s ***\n\n", (name) )

/* Required for various demonstration functions */
typedef struct
{
	char	first_name	[ 30 + 1 ];
	char	last_name	[ 30 + 1 ];
} person;

/* Extended String Functions */
void string_demo( void )
{
	char	str[ 1024 ];
	int		all;
	int		i;
	char**	tokens;
	char*	str1;
	char*	str2;

	printf( "\n*** string_demo ***\n\n" );

	/* Dynamic string functions */
	str1 = pstrdup( "Hello " );
	str1 = pstrcatstr( str1, "World", FALSE );
	str1 = pstrcatchar( str1, '!' );

	str2 = pstrreplace( str1, "World", "out there" );

	printf( "str1 = >%s<\n", str1 );
	printf( "str2 = >%s<\n", str2 );
	pfree( str1 ); /* str2 will be freed within next function call! */

	/* String rendering from templates */
	str1 = pstrrender( "<a href=\"$link\" alt=\"$title\">$title</a>",
			"$link", "http://www.phorward-software.com", FALSE,
			"$title", str2, TRUE,
			(char*)NULL );

	printf( "str1 = >%s<\n", str1 );
	pfree( str1 );

	/* String Tokenizer */
	strcpy( str, "Hello World, this is a simple test" );
	all = pstrsplit( &tokens, str, " ", 3 );
	printf( "%d\n", all );

	for( i = 0; i < all; i++ )
		printf( "%d: >%s<\n", i, tokens[i] );

	pfree( tokens );

	/* Self-allocating sprintf extension */
	str1 = pasprintf( "current content of str is >%s<, and all has %d",
						str, all );
	printf( "str1 = >%s<\n", str1 );
	pfree( str1 );
}

/* UTF-8 Functions */
void utf8_demo( void )
{
	char	str		[ 1024 ];
	char*	ptr;

	printf( "\n*** utf8_demo ***\n\n" );

	strcpy( str, "Hällö ich bün ein StrÜng€!" );
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
	wchar_t	mystr		[ 255 ];
	wchar_t*	mydynamicstr;

	DEMO( "unicode_demo" );

	wcscpy( mystr, L"Yes, w€ cän üse standard C function "
			L"names for Unicode-strings!" );


	printf( "mystr = >%ls<\n", mystr );
	swprintf( mystr, sizeof( mystr ),
			L"This string was %d characters long!",
			pwcslen( mystr ) );
	printf( "mystr = >%ls<\n", mystr );

	mydynamicstr = pwcsdup( mystr );
	mydynamicstr = pwcscatstr( mydynamicstr,
			L" You can see: The functions are"
			L" used the same way as the standard"
			L" char-functions!", FALSE );

	printf( "mydynamicstr = >%ls<\n", mydynamicstr );
	pfree( mydynamicstr );

	mydynamicstr = pawcsprintf( L"This is €uro symbol %ls of %d",
						mystr, sizeof( mystr ) );
	printf( "mydynamicstr = >%ls<\n", mydynamicstr );
	pfree( mydynamicstr );
}

void p_ccl_demo( void )
{
	pccl*		c;
	pccl*		d;
	pccl*		e;
	char* 		x;

	DEMO( "p_ccl_demo" );

	/* c = p_ccl_create( "\\1-\\n€A-ZX-dÄÜÖ" ); */
	c = p_ccl_create( PCCL_MIN, PCCL_MAX, "^ €A-Z\n" );
	d = p_ccl_create( PCCL_MIN, PCCL_MAX, "A-NXYZ\n" );

	d = p_ccl_create( PCCL_MIN, PCCL_MAX,
							"^alles richtig! :)" );
	p_ccl_print( stderr, c, -1 );
	p_ccl_print( stderr, d, -1 );

	e = p_ccl_union( c, d );
	p_ccl_print( stderr, e, -1 );

	p_ccl_negate( e );
	p_ccl_print( stderr, e, -1 );

	p_ccl_negate( e );
	p_ccl_print( stderr, e, -1 );

	/* e = p_ccl_intersect( c, d ); */
	p_ccl_delrange( d, '\0', PCCL_MAX );
	printf( "e = >%s<\n", p_ccl_to_str( d, TRUE ) );

	p_ccl_free( c );
	p_ccl_free( d );
	p_ccl_free( e );
}

static void plist_demo_print( plist* list )
{
	person*		pp;
	plistel*	e;

	printf( "\n" );
	for( e = plist_first( list ); e; e = plist_next( e ) )
	{
		pp = (person*)plist_access( e );
		printf( "- %s, %s\n", pp->last_name, pp->first_name );
	}
	printf( "~~~\n%d elements\n\n", plist_count( list ) );
}

static void plist_demo_print_by_key( plist* list, char* key )
{
	person*		pp;
	plistel*	e;

	if( !( e = plist_get_by_key( list, key ) ) )
	{
		printf( "<No record found matching '%s'>\n", key );
		return;
	}

	pp = (person*)plist_access( e );
	printf( "%s => %s, %s\n", key, pp->last_name, pp->first_name );
}

static int by_name( plist* list, plistel* a, plistel* b )
{
	person*	ap = plist_access( a );
	person*	bp = plist_access( b );

	return strcmp( ap->last_name, bp->last_name );
}

void plist_demo( void )
{
	person		p;
	plist*		my;
	plistel*	e;

	DEMO( "plist_demo" );

	my = plist_create( sizeof( person ), PLIST_MOD_RECYCLE );

	/* Add some data */
	strcpy( p.first_name, "Melinda" );
	strcpy( p.last_name, "Smith" );
	plist_insert( my, NULL, "Smith", &p );

	strcpy( p.first_name, "Brenda" );
	strcpy( p.last_name, "Brandon" );
	plist_insert( my, NULL, "Brandon", &p );

	strcpy( p.first_name, "Monique" );
	strcpy( p.last_name, "Joli" );
	e = plist_insert( my, NULL, "Joli", &p );

	strcpy( p.first_name, "Susan" );
	strcpy( p.last_name, "Mueller" );
	plist_insert( my, NULL, "Mueller", &p );

	/* Print content */
	plist_demo_print( my );

	/* Sort list by name */
	plist_set_sortfn( my, by_name );
	plist_sort( my );

	/* Print content */
	plist_demo_print( my );

	/* Find by key */
	plist_demo_print_by_key( my, "Joli" );

	/* Remove entry */
	plist_remove( my, e );

	/* Find again by key */
	plist_demo_print_by_key( my, "Joli" );

	/* Add more data - first element will be recycled. */
	strcpy( p.first_name, "Rei" );
	strcpy( p.last_name, "Ayanami" );
	plist_insert( my, NULL, "Ayanami", &p );
	plist_demo_print_by_key( my, "Ayanami" );

	/* Add data with same key, test collision */
	strcpy( p.first_name, "Throttle" );
	strcpy( p.last_name, "Full" );
	plist_insert( my, NULL, "Ayanami", &p );

	/* Sort list by name again */
	plist_sort( my );

	/* Now print and get by key */
	plist_demo_print( my );
	plist_demo_print_by_key( my, "Ayanami" );

	plist_erase( my );
	plist_demo_print( my );

	my = plist_free( my );
}

/*
	plist_demo2() is an example based on the list_demo() example from above,
	but uses the plist-object with PLIST_MOD_PTR-flag for pointer mode enabled,
	to result in the same behavior.
*/
void plist_demo2( void )
{
	plist*		mylist; /* This is list object */
	plistel*	e; 		/* e, for list iteration */
	char*		values[] = { "Hello", "World", "out there!" };
	char*		tmp;

	DEMO( "plist_demo2" );

	mylist = plist_create( sizeof( char* ),
				PLIST_MOD_RECYCLE | PLIST_MOD_PTR );

	/* Create the list. */
	plist_push( mylist, (void*)values[0] );
	plist_push( mylist, (void*)values[1] );
	plist_push( mylist, (void*)values[2] );

	printf( "%p\n", values[0] );
	printf( "%p\n", values[1] );
	printf( "%p\n", values[2] );

	/* Let's iterate it. */
	printf( "mylist contains %d items\n", plist_count( mylist ) );
	for( e = plist_first( mylist ); e; e = plist_next( e ) )
		printf( "%s(%p) ", (char*)plist_access( e ),
								(char*)plist_access( e ) );

	/* Now, we remove one element (identified by its pointer)
		and iterate the list again */
	plist_remove( mylist, plist_get_by_ptr( mylist, (void*)values[1] ) );
	printf( "\nmylist contains now %d items\n", plist_count( mylist ) );

	/* The macro plist_for() expands into a
		for-loop like above, but is shorter! ;) */
	plist_for( mylist, e )
		printf( "%s(%p) ", (char*)plist_access( e ),
								(char*)plist_access( e ) );

	printf( "\n" );

	plist_pop( mylist, (void*)&tmp );
	printf( "tmp = %p >%s<\n", tmp, tmp );

	/* Free the entire list */
	mylist = plist_free( mylist );
}

static void array_print( parray* a )
{
	int		i;
	person*	p;

	printf( "first = %ld last = %ld count = %ld size = %ld\n",
				a->first, a->last, a->count, a->size );

	printf( "-- %d Elements --\n", parray_count( a ) );

	for( i = 0; ( p = (person*)parray_get( a, i ) ); i++ )
		printf( "%02d) %s %s\n", i, p->first_name, p->last_name );

	printf( "-- %d Elements --\n", parray_count( a ) );
}

void parray_demo( void )
{
	person 	x;
	person*	p;
	parray*	a;

	DEMO( "parray_demo" );

	a = parray_create( sizeof( person ), 0 );

	strcpy( x.last_name, "Zufall" );
	strcpy( x.first_name, "Reiner" );
	parray_push( a, (void*)&x );

	array_print( a );

	strcpy( x.last_name, "Pfahl" );
	strcpy( x.first_name, "Martha" );
	p = (person*)parray_insert( a, 5, (void*)&x );

	array_print( a );
	printf( "%ld\n", parray_offset( a, p - 10 ) );

	strcpy( x.last_name, "Racho" );
	strcpy( x.first_name, "Volker" );
	parray_unshift( a, (void*)&x );

	array_print( a );

	strcpy( x.last_name, "Pete" );
	strcpy( x.first_name, "Dieter" );
	parray_unshift( a, (void*)&x );

	array_print( a );

	parray_remove( a, 1, (void**)NULL );

	array_print( a );

	a = parray_free( a );
}

int faculty( int x )
{
	int ret;

	PROC( "faculty" );
	PARMS( "x", "%d", x );

	if( x < 0 )
	{
		WRONGPARAM;
		RETURN( -1 );
	}
	else if( x == 0 )
	{
		MSG( "x is 0, so faculty is 1" );
		RETURN( 1 );
	}
	else
	{
		MSG( "Calling faculty recurisvely with:" );
		VARS( "x - 1", "%d", x - 1 );
		ret = x * faculty( x - 1 );
	}

	VARS( "ret", "%d", ret );
	RETURN( ret );
}

void dbg_demo( void )
{
	int		s = 3;
	int		f;

	DEMO( "dbg_demo" );

	putenv( "TRACEMODULE=*" );

	f = faculty( 3 );
	printf( "The faculty of %d is %d\n", s, f );

	putenv( "TRACEMODULE=" );
}

void xml_demo( void )
{
	XML_T	comp;
	XML_T	div;
	XML_T	emp;
	char*	s;

	DEMO( "xml_demo" );

	comp = xml_new( "company" );

	div = xml_add_child( comp, "division", 0 );
	xml_set_attr( div, "task", "Development" );

	emp = xml_add_child( div, "employee", 0 );
	xml_set_attr( emp, "name", "Eduard Woodstock" );
	emp = xml_add_child( div, "employee", 0 );
	xml_set_attr( emp, "name", "Helga Wacken" );
	emp = xml_add_child( div, "employee", 0 );
	xml_set_attr( emp, "name", "Horst Summerbreeze" );

	xml_set_txt( emp, "Das ist ein Test!" );

	div = xml_add_child( comp, "division", 0 );
	xml_set_attr( div, "task", "Sales" );
	emp = xml_add_child( div, "employee", 0 );
	xml_set_attr( emp, "name", "Peter Paris" );
	emp = xml_add_child( div, "employee", 0 );
	xml_set_attr( emp, "name", "Josephine Julianadorp" );

	s = xml_toxml( comp );

	printf( "%s\n", s );
	pfree( s );
	xml_free( comp );
}

void any_demo( void )
{
	/*
	 * This is a demonstration of the pany object, which implements
	 * a union data type storage. A union type is a variable which is capable to
	 * store different values, by using a type flag, but only one memory store.
	 *
	 * The pany-data type and its support functions of libphorward allows to
	 * store byte, char, int, long, unsigned long (ulong), float, double,
	 * string (char*) and wide-character string (wchar_t*) and their conversion
	 * among each other.
	 *
	 * String memory is always hold with the pany-object, until the
	 * structure is converted into another type or freed.
	 */
	pany*	val;

	DEMO( "any_demo" );

	/* Get new any object, with a string. */
	val = pany_create( "123 Hello World" );

	/* Get the string */
	printf( "val(str) = >%s<\n", pany_get_str( val ) );

	/* Get the string as wide-character value. */
	printf( "val(wcs) = >%ls<\n", pany_get_wcs( val ) );

	/* The the string as integer value - only 123 will be returned! */
	printf( "val(int) = %d\n", pany_get_int( val ) );

	/* Reset the value by a floating point number */
	pany_set_double( val, 123.456 );

	printf( "val(double) = %lf\n", pany_get_double( val ) );
	printf( "val(str) = >%s<\n", pany_get_str( val ) );

	/* Free that object */
	val = pany_free( val );
}

void help( char** argv )
{
	printf( "usage: %s OPTIONS\n\n"

	"   -s  --stepwise        Execute demo, step-by-step.\n"
	"   -h  --help            Show this help, and exit.\n\n", *argv );
}

int main( int argc, char** argv )
{
	int			i;
	int			rc;
	int			next;
	char		opt				[ 20 + 1 ];
	pboolean	stepwise		= FALSE;
	void		(*func[])(void)	= {
		&string_demo,
		&unicode_demo,
		&utf8_demo,
		&plist_demo2,
		&plist_demo,
		&parray_demo,
		&dbg_demo,
		&xml_demo,
		&any_demo
	};
	char*		param;

	setlocale( LC_ALL, "" ); /* for unicode environment */

	/* Parse command line parameters */
	for( i = 0; ( rc = pgetopt( opt, &param, &next, argc, argv,
						"sh",
						"stepwise help", i ) ) == 0; i++ )
	{
		if( !strcmp( opt, "help" ) || *opt == 'h' )
		{
			help( argv );
			return 0;
		}
		else if( !strcmp( opt, "stepwise" ) || *opt == 's' )
			stepwise = TRUE;
	}

	if( rc < 0 && param )
	{
		printf( "Unknown option: '%s'\n\n", param );
		help( argv );
		return 1;
	}

	for( i = 0; i < sizeof( func ) / sizeof( *func ); i++ )
	{
		(*func[i])();

		if( stepwise )
		{
			printf( "\n--- Press ENTER for next demo ---\n" );
			getchar();
		}
	}

	return EXIT_SUCCESS;
}

