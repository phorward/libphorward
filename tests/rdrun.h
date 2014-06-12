/* Some kind of "runtime library" for the generated C code of rd.c */
/* THIS IS A TEST PROJECT SOURCE FOR RESEARCH PURPOSES!! */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BOOLEAN		short
#define TRUE 		1
#define FALSE 		0

typedef struct
{
	enum
	{
		OPEN,
		CLOSE
	} 		flag;

	char*	rule;
	char*	begin;
	char*	end;
} MATCH;

MATCH	ast		[ 1024 ];
int		ast_tos	= -1;

MATCH* open_rule( char* rule, char* begin )
{
	ast_tos++;

	ast[ ast_tos ].flag = OPEN;
	ast[ ast_tos ].rule = rule;
	ast[ ast_tos ].begin = begin;
	ast[ ast_tos ].end = (char*)NULL;

	return &ast[ ast_tos ];
}

BOOLEAN close_rule( char* rule, char* begin, char* end )
{
	int		i;

	for( i = ast_tos; i >= 0; i-- )
	{
		if( strcmp( ast[ i ].rule, rule ) == 0
				&& ast[ i ].begin == begin
					&& !ast[ i ].end )
		{
			ast[ i ].end = end;
			break;
		}
	}

	ast_tos++;

	ast[ ast_tos ].flag = CLOSE;
	ast[ ast_tos ].rule = rule;
	ast[ ast_tos ].begin = begin;
	ast[ ast_tos ].end = end;

	return FALSE;
}


BOOLEAN drop_rule( char* rule, char* begin )
{
	int		i;

	for( i = ast_tos; i >= 0; i-- )
	{
		if(	ast[ i ].flag == OPEN
				&& strcmp( ast[ i ].rule, rule ) == 0
					&& ast[ i ].begin == begin )
		{
			ast_tos = i - 1;
			return TRUE;
		}
	}

	return FALSE;
}

BOOLEAN test_rule( char* rule )
{
	int		i;

	for( i = ast_tos; i >= 0; i-- )
	{
		if( ast[ i ].flag == OPEN
				&& strcmp( ast[ i ].rule, rule ) == 0
					&& !ast[ i ].end )
			return FALSE;
	}

	return TRUE;
}

void print_ast( void )
{
	int		i;
	char	gap		[ 80 ];

	*gap = '\0';

	for( i = 0; i <= ast_tos; i++ )
	{
		if( ast[ i ].flag == CLOSE )
			gap[ strlen( gap ) - 1 ] = '\0';

		printf( "%s%s %s >%.*s<\n", gap,
					ast[ i ].flag == OPEN ? "open " : "close",
						ast[ i ].rule,
							ast[ i ].end - ast[ i ].begin,
								ast[ i ].begin );

		if( ast[ i ].flag == OPEN )
			strcat( gap, " " );
	}
}
