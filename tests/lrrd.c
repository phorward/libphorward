/*
	This program is a test parser that implements a
	left-recursive recursive-descent parser.

	More about this is in my blog at
	http://bierpilot.phorward-software.com/?p=1119

	Compile with
	cc -g -o lrrd lrrd.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define	MAXSTACK	512
#define	MAXRULES	8
#define	MAXRULELEN	10
#define	BOOLEAN		short
#define	TRUE		1
#define	FALSE		0

typedef struct
{
	char	rule		[ MAXRULELEN ];
	BOOLEAN	is_lrec;
} RULE;

#define IS_VAR( N )		( islower( N ) )

typedef struct
{
	char	name;

	RULE	rules		[ MAXRULES ];
	int		rules_cnt;

	BOOLEAN	is_lrec;
} VAR;

typedef struct
{
	VAR*	var;

	char*	istart;
	char*	start;
	char*	end;

	int		rule;
	int		dot;

	BOOLEAN	is_lrec;
	int		consumed;
	int		matches;

	BOOLEAN	used		[ MAXRULES ];
	int		smatch;
} STATE;

typedef struct
{
	VAR*	var;
	char*	start;
	char*	end;
} MATCH;

VAR	vars[] =
{
	{
		's',
		{
			{	"e"		}
		},
		1
	},
	{
		'e',
		{
			{	"e+t"	},
			{	"e-t"	},
			{	"t"		}
		},
		3
	},
	{
		't',
		{
			{	"t*f"	},
			{	"t/f"	},
			{	"f"		}
		},
		3
	},
	{
		'f',
		{
			{	"X"		},
			{	"(e)"	}
		},
		2
	}
};

int vars_cnt	=	sizeof( vars ) / sizeof( VAR );

VAR* getvar( char name )
{
	int	i;

	for( i = 0; i < vars_cnt; i++ )
		if( vars[ i ].name == name )
			return &vars[ i ];

	return (VAR*)NULL;
}

int lrecs( VAR* var )
{
	int		lrecs	= 0;
	VAR*	tvar;
	VAR*	stack	[ MAXSTACK ];
	int		tos		= 0;
	int		i;
	int		j;

	stack[ tos++ ] = var;

	while( tos > 0 )
	{
		var = stack[ --tos ];

		for( i = 0; i < var->rules_cnt; i++ )
		{
			if( IS_VAR( var->rules[ i ].rule[ 0 ] ) )
			{
				tvar = getvar( var->rules[ i ].rule[ 0 ] );
				if( tvar->is_lrec )
				{
					var->is_lrec = var->rules[ i ].is_lrec = TRUE;
					lrecs++;
					continue;
				}

				stack[ tos++ ] = tvar;

				for( j = tos - 1; j >= 0; j-- )
				{
					if( stack[ j ] == var )
					{
						var->is_lrec = var->rules[ i ].is_lrec = TRUE;

						tos--;
						lrecs++;
						break;
					}
				}
			}
		}
	}

	return lrecs;
}

int run( VAR* var, char* start )
{
	char*	end;
	STATE	states	[ MAXSTACK ];
	STATE*	tos		= states;
	STATE*	state;
	RULE*	rule;
	int		i;
	MATCH	match		[ MAXSTACK ];
	int		nmatch	= 0;

	memset( &states[ 0 ], 0, sizeof( STATE ) );

	tos->var = var;
	tos->rule = -1;
	tos->istart = tos->start = tos->end = start;

	while( tos )
	{
		/* Find first non-used production */
		if( tos->rule < 0 )
		{
			for( i = 0; i < tos->var->rules_cnt; i++ )
			{
				if( !tos->used[ i ] )
				{
					tos->rule = i;
					tos->used [ i ] = TRUE;
					break;
				}
			}

			if( tos->rule < 0 )
			{
				if( tos > states )
				{
					if( tos->matches > 0 )
					{
						( tos - 1 )->dot++;
						( tos - 1 )->end += tos->consumed;
					}
					else
					{
						( tos - 1 )->dot = 0;
						( tos - 1 )->rule = -1;
						( tos - 1 )->end = ( tos - 1 )->start;
					}

					tos--;
				}

				continue;
			}
		}

		/* Dump states */
		for( state = states; state <= tos; state++ )
		{
			rule = &( state->var->rules[ state->rule ] );

			printf( "%s %c => ", state->is_lrec ? "L" : " ", state->var->name );
			for( i = 0; i < strlen( rule->rule ) + 1; i++ )
			{
				if( i == state->dot )
					printf( "." );

				if( rule->rule[ i ] )
					printf( "%c", rule->rule[ i ] );
			}

			printf( "  >%s<", state->start );

			printf( "\n" );
		}

		/* getchar(); */

		rule = &( tos->var->rules[ tos->rule ] );

		if( tos->dot == strlen( rule->rule ) )
		{
			printf( "rule completed\n" );

			tos->consumed = tos->consumed + (int)( tos->end - tos->start );
			tos->matches++;

			match[ nmatch ].var = tos->var;
			match[ nmatch ].start = tos->istart;
			match[ nmatch ].end = tos->end;
			nmatch++;

			if( rule->is_lrec )
			{
				tos->dot = 1;
				tos->end = tos->start += ( tos->end - tos->start );
				tos->smatch = nmatch;

				/* Only allow left-recursives */
				for( i = 0; i < tos->var->rules_cnt; i++ )
				{
					if( tos->var->rules[ i ].is_lrec )
						tos->used[ i ] = FALSE;
					else
						tos->used[ i ] = TRUE;
				}

				tos->rule = -1;
			}
			else
			{
				if( tos > states )
				{
					( tos - 1 )->dot++;
					( tos - 1 )->end += ( tos->end - tos->start );
					tos--;
				}
				else
					break;
			}
		}
		else if( IS_VAR( rule->rule[ tos->dot ] ) )
		{
			printf( "call variable\n" );

			memset( tos + 1, 0, sizeof( STATE ) );

			( tos + 1 )->var = getvar( rule->rule[ tos->dot ] );
			( tos + 1 )->smatch = nmatch;
			( tos + 1 )-> istart = ( tos + 1 )->start
										= ( tos + 1 )->end = tos->end;
			tos++;

			if( ( tos->is_lrec = tos->var->is_lrec ) )
			{
				for( state = tos - 1; state >= states; state-- )
				{
					if( state->var == tos->var && state->end == tos->end )
					{
						tos->is_lrec = !state->is_lrec;
						break;
					}
				}

				/* Only allow non-left-recursives */
				printf( "tos->is_lrec = %d\n", tos->is_lrec );
				if( !tos->is_lrec )
				{
					for( i = 0; i < tos->var->rules_cnt; i++ )
						if( tos->var->rules[ i ].is_lrec )
							tos->used[ i ] = TRUE;
				}
			}

			tos->rule = -1;
		}
		else if( *( tos->end ) == rule->rule[ tos->dot ] )
		{
			printf( "matching char >%c<\n", *( tos->end ) );
			tos->dot++;
			tos->end++;
		}
		else
		{
			printf( "rule failed\n" );
			tos->rule = -1;
			nmatch = tos->smatch;

			if( tos->is_lrec && tos->matches > 0 )
				tos->dot = 1;
			else
				tos->dot = 0;

			tos->end = tos->start;
		}
	}

	for( i = 0; i < nmatch; i++ )
		printf( "%c >%.*s<\n",
			match[ i ].var->name,
				match[ i ].end - match[ i ].start,
					match[ i ].start );

	printf( "nmatch = %d\n", nmatch );

	return tos ? tos->consumed : 0;
}

int print( void )
{
	int		i;
	int		j;

	for( i = 0; i < vars_cnt; i++ )
	{
		for( j = 0; j < vars[ i ].rules_cnt; j++ )
			printf( "%c : %s%s\n", vars[ i ].name, vars[ i ].rules[ j ].rule,
									vars[ i ].rules[ j ].is_lrec ?
										" (left-recursive)" : "" );
	}
}

int main( int argc, char** argv )
{
	int		len;

	lrecs( &vars[ 0 ] );
	print();

	if( argc > 1 )
	{
		len = run( &vars[ 0 ], argv[ 1 ] );
		printf( "\nlen = %d >%.*s<\n", len, len, argv[ 1 ] );
	}

	return 0;
}
