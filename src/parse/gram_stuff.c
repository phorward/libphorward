#define NAMELEN			80
#define DERIVCHAR		'\''


static pboolean traverse_production( ppgram* g, ppsym* lhs, ppast* node );

/* Derive name from basename */
static char* derive_name( ppgram* g, char* base )
{
	int             i;
	static
	char    deriv   [ ( NAMELEN * 2 ) + 1 ];

	sprintf( deriv, "%s%c", base, DERIVCHAR );

	for( i = 0; strlen( deriv ) < ( NAMELEN * 2 ); i++ )
	{
		if( !pp_sym_get_by_name( g, deriv ) )
			return deriv;

		sprintf( deriv + strlen( deriv ), "%c", DERIVCHAR );
	}

	return (char*)NULL;
}

#define NODE_IS( n, s ) 	( !strcmp( (n)->emit, s ) )

static ppsym* traverse_symbol( ppgram* g, ppsym* lhs, ppast* node )
{
	ppsym*		sym			= (ppsym*)NULL;
	ppast*		child;
	int			type		= 0;
	char		name		[ NAMELEN * 2 + 1 ];

	/* fprintf( stderr, "sym >%s<\n", node->emit ); */

	if( NODE_IS( node, "inline") )
	{
		sym = pp_sym_create( g, derive_name( g, lhs->name ),
								PPFLAG_DEFINED | PPFLAG_GENERATED );
		child = node->child;

		if( NODE_IS( child, "flag_emit" ) )
		{
			child = child->next;

			if( NODE_IS( child, "ident" ) )
			{
				sym->emit = pstrndup( child->start, child->length );
				sym->flags |= PPFLAG_FREEEMIT;

				child = child->next;
			}
			else
				sym->emit = lhs->name;
		}

		for( ; child; child = child->next )
			if( !traverse_production( g, sym, child->child ) )
				return (ppsym*)NULL;
	}
	else
	{
		sprintf( name, "%.*s", node->length, node->start );

		if( NODE_IS( node, "ident") )
		{
			if( !( sym = pp_sym_get_by_name( g, name ) ) )
				sym = pp_sym_create( g, name, PPFLAG_NONE );
		}
		else
		{
			/* REWORK!!! *
			/*
			if( NODE_IS( node, "token") || NODE_IS( node, "string" ) )
				type = PPSYMTYPE_STRING;
			else if( NODE_IS( node, "ccl" ) )
				type = PPSYMTYPE_CCL;
			else if( NODE_IS( node, "regex" ) )
				type = PPSYMTYPE_REGEX;

			name[ pstrlen( name ) - 1 ] = '\0';

			if( !( sym = pp_sym_get_nameless_term_by_def( g, name + 1 ) ) )
			{
				sym = pp_sym_create( g, (char*)NULL );

				sym->name = pstrdup( name + 1 );
				sym->flags |= PPFLAG_DEFINED | PPFLAG_NAMELESS;

				if( NODE_IS( node, "token") )
					sym->emit = sym->name;
			}
			*/
		}
	}

	if( sym )
		sym->flags |= PPFLAG_CALLED;

	return sym;
}


static pboolean traverse_production( ppgram* g, ppsym* lhs, ppast* node )
{
	ppsym*		sym;
	ppsym*		csym;
	ppprod*		prod;
	ppprod*		popt;
	ppast*		child;
	char*		str;
	char		name		[ NAMELEN * 2 + 1 ];
	int			i;
	plistel*	e;

	prod = pp_prod_create( g, lhs, (ppsym*)NULL );

	for( ; node; node = node->next )
	{
		/* fprintf( stderr, "prod >%s<\n", node->emit ); */

		if( NODE_IS( node, "symbol" ) )
		{
			if( !( sym = traverse_symbol( g, lhs, node->child ) ) )
				return FALSE;

			pp_prod_append( prod, sym );
		}
		else
		{
			sym = traverse_symbol( g, lhs, node->child->child );
			str = sym->name;

			if( NODE_IS( node, "kle" ) || NODE_IS( node, "pos" ) )
			{
				sprintf( name, "%s%c", str, PPMOD_POSITIVE );

				if( !( csym = pp_sym_get_by_name( g, name ) ) )
				{
					csym = pp_sym_create( g, name, PPFLAG_DEFINED
													| PPFLAG_CALLED
														| PPFLAG_GENERATED );

					if( g->flags & PPFLAG_PREVENTLREC )
						pp_prod_create( g, csym, sym, csym, (ppsym*)NULL );
					else
						pp_prod_create( g, csym, csym, sym, (ppsym*)NULL );

					pp_prod_create( g, csym, sym, (ppsym*)NULL );
				}

				sym = csym;
			}

			if( NODE_IS( node, "opt" ) || NODE_IS( node, "kle" ) )
			{
				sprintf( name, "%s%c", str, PPMOD_OPTIONAL );

				if( !( csym = pp_sym_get_by_name( g, name ) ) )
				{
					csym = pp_sym_create( g, name, PPFLAG_DEFINED
													| PPFLAG_CALLED
														| PPFLAG_GENERATED );

					pp_prod_create( g, csym, sym, (ppsym*)NULL );
					pp_prod_create( g, csym, (ppsym*)NULL );
				}

				sym = csym;
			}

			pp_prod_append( prod, sym );
		}
	}

	/*
		Optimize productions with one generated symbol
		that was introduced via 'inline'.
	*/
	if( ( sym = pp_prod_getfromrhs( prod, 0 ) )
			&& !pp_prod_getfromrhs( prod, 1 ) )
	{
		if( PPSYM_IS_NONTERMINAL( sym )
				&& sym->flags & PPFLAG_GENERATED
					&& sym->emit && !pp_sym_getprod( sym, 1 ) )
		{
			if( sym->flags & PPFLAG_FREEEMIT )
			{
				prod->emit = pstrdup( sym->emit );
				prod->flags |= PPFLAG_FREEEMIT;
			}
			else
				prod->emit = sym->emit;

			pp_prod_remove( prod, sym );

			popt = pp_sym_getprod( sym, 0 );

			for( i = 0; ( csym = pp_prod_getfromrhs( popt, i ) ); i++ )
				pp_prod_append( prod, csym );

			pp_sym_drop( sym );
		}
	}

	return TRUE;
}

static pboolean ast_to_gram( ppgram* g, ppast* ast )
{
	ppsym*		sym;
	ppsym*		nonterm		= (ppsym*)NULL;
	ppast* 		node;
	ppast*		child;
	char		name		[ NAMELEN * 2 + 1 ];
	char		def			[ NAMELEN * 2 + 1 ];

	int			type;
	pboolean	flag_emit;
	pboolean	flag_ignore;

	/* pp_ast_dump_short( stderr, ast ); */

	for( node = ast; node; node = node->next )
	{
		flag_emit = flag_ignore = FALSE;

		/* fprintf( stderr, "gram >%s<\n", node->emit ); */

		if( NODE_IS( node, "nontermdef" ) )
		{
			child = node->child;

			if( NODE_IS( child, "flag_emit" ) )
			{
				flag_emit = TRUE;
				child = child->next;
			}

			sprintf( name, "%.*s", child->length, child->start );

			/* Create the non-terminal symbol */
			if( !( nonterm = sym = pp_sym_get_by_name( g, name ) ) )
				nonterm = sym = pp_sym_create( g, name, PPFLAG_DEFINED );

			for( child = node->child->next; child; child = child->next )
			{
				if( NODE_IS( child, "flag_goal" ) )
				{
					if( !g->goal ) /* fixme */
					{
						g->goal = sym;
						sym->flags |= PPFLAG_CALLED;
					}
				}
				else if( NODE_IS( child, "flag_lexem" ) )
					sym->flags |= PPFLAG_LEXEM;
				else if( NODE_IS( child, "production" ) )
				{
					if( !traverse_production( g, sym, child->child ) )
						return FALSE;
				}
			}

			if( flag_emit )
				sym->emit = sym->name;
		}
		else if( NODE_IS( node, "termdef" ) )
		{
			child = node->child;

			if( NODE_IS( child, "flag_ignore" ) )
			{
				flag_ignore = TRUE;
				child = child->next;
			}

			if( NODE_IS( child, "flag_emit" ) )
			{
				flag_emit = TRUE;
				child = child->next;
			}

			if( NODE_IS( child, "ident" ) )
			{
				sprintf( name, "%.*s", child->length, child->start );
				child = child->next;
			}
			else
				*name = '\0';

			/* REWORK! */
			/*
			if( NODE_IS( child, "token") )
			{
				flag_emit = TRUE;
				type = PPSYMTYPE_STRING;
			}
			else if( NODE_IS( child, "ccl" ) )
				type = PPSYMTYPE_CCL;
			else if( NODE_IS( child, "string" ) )
				type = PPSYMTYPE_STRING;
			else if( NODE_IS( child, "regex" ) )
				type = PPSYMTYPE_REGEX;

			sprintf( def, "%.*s", child->length - 2, child->start + 1 );
			*/

			/* Create the terminal symbol */
			sym = pp_sym_create( g, *name ? name : (char*)NULL,
									PPFLAG_TERMINAL | PPFLAG_DEFINED );

			/* Configure according to flags */
			if( flag_emit )
				sym->emit = sym->name;

			if( flag_ignore )
			{
				sym->flags |= PPFLAG_WHITESPACE;
				plist_push( g->ws, sym );
			}
		}
	}

	/* If there is no goal, then the last defined nonterminal
		becomes the goal symbol */
	if( !g->goal )
		g->goal = nonterm;

	/* Look for unique goal sequence; if this is not the case, wrap it with
		another, generated nonterminal. */
	if( pp_sym_getprod( g->goal, 1 ) )
	{
		nonterm = pp_sym_create( g, derive_name( g, g->goal->name ),
										PPFLAG_DEFINED
											| PPFLAG_CALLED
												| PPFLAG_GENERATED );

		pp_prod_create( g, nonterm, g->goal, (ppsym*)NULL );
		g->goal = nonterm;
	}

	/* pp_gram_dump( stderr, g ); */

	return TRUE;
}



/** Compiles a grammar definition into a grammar.

//g// is the grammar that receives the result of the parse.
//bnf// is the BNF definition string that defines the grammar.
*/
pboolean pp_gram_from_bnf( ppgram* g, char* bnf )
{
	ppgram*		bnfgram;
	char*		s = bnf;
	char*		e;
	ppast*		ast;

	if( !( g && bnf ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	/* Define grammar for BNF */
	bnfgram = pp_gram_create();
	pp_bnf_define( bnfgram );
	pp_gram_prepare( bnfgram );

	if( !pp_lr_parse( &ast, bnfgram, s, &e ) )
	{
		pp_gram_free( bnfgram );
		return FALSE;
	}

	/* pp_ast_simplify( a ); */

	if( !ast_to_gram( g, ast ) )
		return FALSE;

	pp_gram_free( bnfgram );
	pp_ast_free( ast );

	return TRUE;
}
