#!/bin/awk -f
#This is a utility script to generate get-/set and conversion-functions for the
#union data type punion implemented in union.h
#
#I was lazy, had not the wish to write every of  this functions by hand... so
#I'm sorry for yet another generator script...

BEGIN								{
										FS = "[ \t;:]+"
										datatype = ""
										member = ""
										within_vargen = 0
										first = 0
										variants_cnt = 0
										on_vargen = 0

										SUBSEP = "#"

										print "/*"
										print " * WARNING:"
										print " * THIS FILE IS GENERATED - DO NOT EDIT MANUALLY - IT WILL GO AWAY!"
										print " */"
										print ""
										print "#include \"phorward.h\""
										print ""
									}

END									{
										prev = ""
										for( i = 1; i <= variants_cnt; i++ )
										{
											if( prev == members[ variants[ i ] \
													SUBSEP "member" ] )
												continue

											if( with_conv )
												convfunc( variants[ i ] );

											prev = members[ variants[ i ] \
													SUBSEP "member" ]
										}

										if( with_conv )
											allconv_func()
									}

/[a-zA-Z_]+\*?[ \t]*[a-zA-Z_]+;/	{
										if( !within_vargen )
										{
											datatype = $2
											member = $3
											next
										}
									}

/vargen:/							{
										within_vargen = 1
										line = $0
										gsub( "/\\*", "", line )
										gsub( "\\*/", "", line )
										split( line, types, ":" )

										var_type = types[2]
										var_format = types[3]
										var_define = types[4]
										var_emptyval = types[5]

										if( var_define == "" )
											var_define = "PUNION_" \
												toupper( var_type )

										variants[ ++variants_cnt ] = datatype
										members[ datatype SUBSEP \
													"member" ] = member
										members[ datatype SUBSEP \
													"var_type" ] = var_type
										members[ datatype SUBSEP \
													"var_format" ] = var_format
										members[ datatype SUBSEP \
													"var_define" ] = var_define
										members[ datatype SUBSEP \
													"var_emptyval" ] = var_emptyval

										setcode = ""
										first = 1
										lastconvcode = ""
									}

/to [a-zA-Z_]+\*?:/					{
										if( !within_vargen )
											next

										convcode = trim( substr( $0, \
														index( $0, ":" ) + 1) )

										if( convcode == "(same)" )
											convcode = lastconvcode

										#print $3 SUBSEP datatype
										convert[ $3 SUBSEP datatype ] = convcode

										lastconvcode = convcode
									}

/set:/								{
										if( !within_vargen )
											next

										gsub( "set:", "", $0 )

										if( !first )
										{
											if( setcode != "" )
												setcode = setcode "\n"

											setcode = setcode "\t" trim( $0 )
										}

										first = 0
										next
									}

									{
										if( !within_vargen )
											next

										line = trim( $0 )
										if( substr( line, length( line ) \
												 - 1, 2 ) == "*/" )
										{
											within_vargen = 0

											if( with_set )
												setfunc()

											if( with_get )
												getfunc()
										}

										first = 0
									}

#--- Utility functions go here ---

function setfunc()
{
	# Comment
	print "/** Sets a variants " datatype " data value and type."
	print ""
	print "//var// is the pointer to the punion structure."
	print ""
	print "//" member "// is the " datatype " value to be assigned to //var//."
	print ""
	print "The function always returns the value //" member "//."
	print "*/"

	# Code
	print datatype " punion_set_" var_type "( punion* var, " \
				datatype " " member " )"
	print "{"
	print "	PROC( \"punion_set_" var_type "\" );"
	print "	PARMS( \"var\", \"%p\", var );"
	print "	PARMS( \"" member "\", \"" var_format "\", " member " );"
	print ""
	print "	punion_reset( var );"
	print "	var->type &= ~0x0F;"
	print "	var->type |= " var_define ";"
	print "	var->val." member " = " member ";"

	if( setcode != "" )
		print setcode

	print ""
	print "	RETURN( var->val." member " );"
	print "}\n"
}

function getfunc()
{
	# Comment
	print "/** Returns a punions " datatype " data value."
	print ""
	print "If the variant exists in another data type, it will be converted,"
	print "so use it carefully if data loss is not desired."
	print ""
	print "//var// is the pointer to the punion structure."
	print ""
	print "The function returns the value assigned to //var// as " datatype "."
	print "This value could be converted from the punions original value."
	print "*/"

	# Code
	print datatype " punion_get_" var_type "( punion* var )"
	print "{"
	print "	PROC( \"punion_get_" var_type "\" );"
	print "	PARMS( \"var\", \"%p\", var );"
	print ""
	print "	if( punion_type( var ) != " var_define ")"
	print "	{"
	print "		if( punion_is_convertible( var ) )"
	print "		{"
	print "			MSG( \"Conversion allowed and required\" );"
	print "			if( punion_convert( var, " var_define " ) != ERR_OK )"
	print "				RETURN( (" datatype ")" var_emptyval " );"
	print "		}"
	print "		else"
	print "			RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	RETURN( var->val." member " );"
	print "}\n"
}

function convfunc( type )
{
	for( ok in okdone )
		okdone[ ok ] = 0

	member = members[ type SUBSEP "member" ]
	var_type = members[ type SUBSEP "var_type" ]
	var_format = members[ type SUBSEP "var_format" ]
	var_define = members[ type SUBSEP "var_define" ]
	var_emptyval = members[ type SUBSEP "var_emptyval" ]

	print "/** Converts a variant's current value into a " type " value."

	if( var_type ~ /string/ )
		print "The returned memory is allocated, and must be freed by the caller."

	print ""
	print "//var// is a pointer to the punion structure to convert from."
	print ""
	print "The function returns the " type "-value of the variant."

	if( var_type ~ /string/ )
		print "This pointer must be manually freed by the caller."
	print "*/"

	# Code
	print type " punion_to_" var_type "( punion* var )"
	print "{"
	print "	PROC( \"punion_to_" var_type "\" );"
	print "	PARMS( \"var\", \"%p\", var );"
	print ""

	print "	switch( punion_type( var ) )"
	print "	{"

	for( j = 1; j <= variants_cnt; j++ )
	{
		if( okdone[ variants[j] ] )
			continue

		okdone[ variants[j] ] = 1
		print "		case " members[ variants[j] SUBSEP "var_define" ] ":"

		if( variants[j] == type )
		{
			print "			RETURN( var->val." member " );"
			continue
		}

		if( ( conv = convert[ type SUBSEP variants[j] ] ) == "" )
			conv = "var->val." members[ variants[j] SUBSEP "member" ]

		print "			RETURN( (" type ")" conv " );"
	}

	print "	}\n"
	print "	MSG( \"Can't convert this type!\" );"
	print "	RETURN( (" type ")" var_emptyval " );"
	print "}\n"
}

function allconv_func()
{
	for( ok in okdone )
		okdone[ ok ] = 0

	# Comment
	print "/** Converts a punion-structure to any supported type."
	print ""
	print "//var// is the pointer to punion structure to be converted."
	print ""
	print "//type// is the type define to which //var// should be converted to."
	print ""
	print "The function returns ERR_OK on success, else an ERR_-define."
	print "*/"

	#Code
	print "pint punion_convert( punion* var, pbyte type )"
	print "{"
	print "	PROC( \"punion_convert\" );"
	print "	PARMS( \"var\", \"%p\", var );"
	print "	PARMS( \"type\", \"%d\", type );"
	print ""
	print "	if( punion_type( var ) == type )"
	print "		RETURN( ERR_OK );"
	print ""
	print "	switch( type )"
	print "	{"

	for( i = 1; i <= variants_cnt; i++ )
	{
		if( okdone[ variants[i] ] )
			continue

		okdone[ variants[i] ] = 1

		print "		case " members[ variants[i] SUBSEP "var_define" ] ":"
		print "			punion_set_" members[ variants[i] SUBSEP "var_type" ] \
			  "( var, punion_to_" members[ variants[i] SUBSEP "var_type" ] \
			  "( var ) );"
		print "			RETURN( ERR_OK );\n"
	}

	print "	}"
	print ""
	print "	RETURN( ERR_FAILURE );"
	print "}\n"
}

function ltrim( s )
{
	sub( /^[ \t\n]+/, "", s )
	return s
}

function rtrim( s )
{
	sub( /[ \t\n]+$/, "", s )
	return s
}

function trim( s )
{
	return rtrim( ltrim( s ) )
}

