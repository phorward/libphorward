#!/bin/awk -f
#This is a utility script to generate get-/set and conversion-functions for the
#union data type pgvalue implemented in union.h
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
										print " * THIS FILE IS GENERATED "\
												"- DO NOT EDIT MANUALLY - "\
												"IT WILL GO AWAY!"
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
											var_define = "PGVALUE_" \
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
													"var_emptyval" ] = \
														var_emptyval

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

										#print "/*" $3 SUBSEP datatype " = " \
										# convcode " */"
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
	print "/** Sets the " datatype " data value and type."
	print "//val// is the pgvalue-object to be set."
	print ""
	print "//" member "// is the " datatype " value to be assigned to //val//."
	print ""
	print "The function always returns the value //" member "//."
	print "*/"

	# Code
	print datatype " pgvalue_set_" var_type "( pgvalue* val, " \
				datatype " " member " )"
	print "{"
	print "	PROC( \"pgvalue_set_" var_type "\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print "	PARMS( \"" member "\", \"" var_format "\", " member " );"
	print ""
	print "	if( !val )"
	print "	{"
	print "		WRONGPARAM;"
	print "		RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	pgvalue_reset( val );"
	print "	val->type = " var_define ";"
	print "	val->val." member " = " member ";"

	if( setcode != "" )
		print setcode

	print ""
	print "	RETURN( val->val." member " );"
	print "}\n"
}

function getfunc()
{
	# Comment
	print "/** Returns the " datatype " data value if //val//."
	print ""
	print "If the pgvalue contains another data type, it will be converted,"
	print "so use it carefully if data loss is not wanted."
	print ""
	print "//val// is the pointer to the pgvalue structure."
	print ""
	print "The function returns the value assigned to //val// as " datatype "."
	print "This value could be converted from the original value."
	print "*/"

	# Code
	print datatype " pgvalue_get_" var_type "( pgvalue* val )"
	print "{"
	print "	PROC( \"pgvalue_get_" var_type "\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print ""
	print "	if( !val )"
	print "	{"
	print "		WRONGPARAM;"
	print "		RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	if( val->type != " var_define ")"
	print "	{"
	print "		if( pgvalue_get_autoconvert( val ) )"
	print "		{"
	print "			MSG( \"Conversion allowed and required\" );"
	print "			if( !pgvalue_convert( val, " var_define " ) )"
	print "				RETURN( (" datatype ")" var_emptyval " );"
	print "		}"
	print "		else"
	print "			RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	RETURN( val->val." member " );"
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

	print "/** Converts the current value of //val// into a " type " value."

	if( var_type ~ /string/ )
		print "The returned memory is allocated, "\
			"and must be freed by the caller."

	print ""
	print "//val// is the pgvalue-object to convert from."
	print ""
	print "The function returns the " type "-value of the variant."

	if( var_type ~ /string/ )
		print "This pointer must be manually freed by the caller."
	print "*/"

	# Code
	print type " pgvalue_to_" var_type "( pgvalue* val )"
	print "{"
	print "	PROC( \"pgvalue_to_" var_type "\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print ""
	print "	if( !val )"
	print "	{"
	print "		WRONGPARAM;"
	print "		RETURN( (" type ")" var_emptyval " );"
	print "	}"
	print ""
	print "	switch( val->type )"
	print "	{"

	for( j = 1; j <= variants_cnt; j++ )
	{
		if( okdone[ variants[j] ] )
			continue

		okdone[ variants[j] ] = 1
		print "		case " members[ variants[j] SUBSEP "var_define" ] ":"

		if( variants[j] == type )
		{
			print "			RETURN( val->val." member " );"
			continue
		}

		if( ( conv = convert[ type SUBSEP variants[j] ] ) == "" )
			conv = "val->val." members[ variants[j] SUBSEP "member" ]

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
	print "/** Converts a pgvalue-structure to any supported type."
	print ""
	print "//val// is the pgvalue-object to be converted."
	print "//type// is the type define to which //val// should be converted to."
	print ""
	print "The function returns TRUE on success, FALSE else."
	print "*/"

	#Code
	print "pboolean pgvalue_convert( pgvalue* val, pgvaluetype type )"
	print "{"
	print "	PROC( \"pgvalue_convert\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print "	PARMS( \"type\", \"%d\", type );"
	print ""
	print "	if( !val )"
	print "	{"
	print "		WRONGPARAM;"
	print "		RETURN( FALSE );"
	print "	}"
	print
	print "	if( val->type == type )"
	print "		RETURN( TRUE );"
	print ""
	print "	switch( type )"
	print "	{"

	for( i = 1; i <= variants_cnt; i++ )
	{
		if( okdone[ variants[i] ] )
			continue

		okdone[ variants[i] ] = 1

		print "		case " members[ variants[i] SUBSEP "var_define" ] ":"
		print "			pgvalue_set_" members[ variants[i] SUBSEP "var_type" ] \
			  "( val, pgvalue_to_" members[ variants[i] SUBSEP "var_type" ] \
			  "( val ) );"
		print "			RETURN( TRUE );\n"
	}

	print "	}"
	print ""
	print "	RETURN( FALSE );"
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

