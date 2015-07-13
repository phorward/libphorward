/* Clear, use of spaces behind and before brackets,
	curly-bracket blocks are _ALWAYS_ written in SINGLE lines: */
if( a && !b( c ) )
{
	/* newlines on every block */
	c = 0;
	we_like_this_style();
}

/* Block only where more than one statement is executed */
if( !c )
	fuck_yeah();
else
{
	c = 0;
	this_is_better();
}

/* Cascading view on multiple lines, never go over 80 chars
	per line */
if( this_would_be_a_long_condition()
		&& i_would_prefer_a_cascading_code_style()
			&& i < 300
				&& that_looks_cooler_eh() ? FALSE : TRUE )
	that_s_libphorward_style( TRUE );
