#!/bin/awk -f
# -SCRIPT-----------------------------------------------------------------------
# Phorward Foundation Toolkit
# Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See LICENSE for more information.
#
# File:		any.gen.awk
# Usage:	Generator for pany type conversion functions.
#
# This is a utility script to generate get-/set and conversion-functions for the
# union data type pany implemented in union.h
#
# I was lazy, had not the wish to write every of this functions by hand... so
# I'm sorry for yet another generator script...
# ------------------------------------------------------------------------------

BEGIN								{
										FS = "[ \t;:]+"
										datatype = ""
										member = ""
										within_type = 0
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

											if( with_from )
											{
												for( ok in okdone )
													okdone[ ok ] = 0

												for( j = 1;
														j <= variants_cnt;
															j++ )
												{
													if( getfrom[ \
															variants[i] \
																SUBSEP \
																variants[j] ] \
														&& !okdone[variants[j]]\
															)
													{
														okdone[variants[j]] = 1

														fromfunc( \
															variants[i], \
																variants[j] )
													}
												}
											}

											prev = members[ variants[ i ] \
													SUBSEP "member" ]
										}

										if( with_conv )
											allconv_func()

										if( with_fprint )
											fprint_func()
									}

/(unsigned|signed) [a-zA-Z_]+\*?[ \t]*[a-zA-Z_]+;/	{
										if( !within_type )
										{
											datatype = $2 " " $3
											member = $4
											next
										}
									}

/[a-zA-Z_]+\*?[ \t]*[a-zA-Z_]+;/	{
										if( !within_type )
										{
											datatype = $2
											member = $3
											next
										}
									}

/type:/								{
										within_type = 1
										line = $0
										gsub( "/\\*", "", line )
										gsub( "\\*/", "", line )
										split( line, types, ":" )

										var_type = types[2]
										var_format = types[3]
										var_define = types[4]
										var_emptyval = types[5]

										if( var_define == "" )
											var_define = "PANYTYPE_" \
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
										next
									}

/format:/							{
										if( !within_type )
											next

										split( trim($0), tok, ":" )
										var_format = tok[2]

										members[ datatype SUBSEP \
													"var_format" ] = var_format

										next
									}

/define:/							{
										if( !within_type )
											next

										split( trim($0), tok, ":" )
										var_define = tok[2]

										members[ datatype SUBSEP \
													"var_define" ] = var_define
										next
									}

/empty:/							{
										if( !within_type )
											next

										split( trim($0), tok, ":" )
										var_emptyval = tok[2]

										members[ datatype SUBSEP \
													"var_emptyval" ] = \
														var_emptyval
										next
									}

/to [a-zA-Z_ ]+\*?:/				{
										if( !within_type )
											next

										line = trim( $0 )
										col = index( line, ":" )
										type = substr( line, 4, col - 4 )
										convcode = trim( substr( \
															line, col + 1) )

										if( convcode == "(same)" )
											convcode = lastconvcode

										convert[ type SUBSEP datatype ] = \
																		convcode

										lastconvcode = convcode
										next
									}

/from [a-zA-Z_ ]+\*?:/				{
										if( !within_type )
											next

										line = trim( $0 )
										col = index( line, ":" )
										type = substr( line, 4, col - 4 )
										fromcode = trim( substr( \
															line, col + 1 ) )

										if( fromcode == "(same)" )
											fromcode = lastfromcode

										getfrom[ datatype SUBSEP type ] = \
											fromcode

										lastfromcode = fromcode
										next
									}

/set:/								{
										if( !within_type )
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
										if( !within_type )
											next

										line = trim( $0 )
										if( substr( line, length( line ) \
												 - 1, 2 ) == "*/" )
										{
											within_type = 0

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
	print "/** Sets the ``" datatype "``-value and type of //val//."
	print ""
	print "//val// is the pany-object to be set."
	print "//" member "// is the ``" datatype "``-value to be assigned to" \
			" //val//."
	print ""
	print "The function always returns the value //" member "//."
	print "*/"

	# Code
	print datatype " pany_set_" var_type "( pany* val, " \
				datatype " " member " )"
	print "{"
	print "	PROC( \"pany_set_" var_type "\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print "	PARMS( \"" member "\", \"" var_format "\", " member " );"
	print ""
	print "	if( !val )"
	print "	{"
	print "		WRONGPARAM;"
	print "		RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	pany_reset( val );"
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
	print "/** Returns the ``" datatype "``-value of //val//."
	print ""
	print "//val// is the pointer to the pany-object."
	print ""
	print "If the pany-object exists in another data type, it will be " \
			"converted."
	print "The function returns the value assigned to //val// as " datatype "."
	print "This value could be converted from the original value."
	print "*/"

	# Code
	print datatype " pany_get_" var_type "( pany* val )"
	print "{"
	print "	PROC( \"pany_get_" var_type "\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print ""
	print "	if( !val )"
	print "	{"
	print "		WRONGPARAM;"
	print "		RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	if( val->type != " var_define " )"
	print "	{"
	print "		if( !pany_convert( val, " var_define " ) )"
	print "			RETURN( (" datatype ")" var_emptyval " );"
	print "	}"
	print ""
	print "	RETURN( val->val." member " );"
	print "}\n"
}

function fromfunc( type, from )
{
	member = members[ type SUBSEP "member" ]
	var_type = members[ type SUBSEP "var_type" ]
	var_format = members[ type SUBSEP "var_format" ]
	var_define = members[ type SUBSEP "var_define" ]
	var_emptyval = members[ type SUBSEP "var_emptyval" ]

	from_member = members[ from SUBSEP "member" ]
	from_type = members[ from SUBSEP "var_type" ]
	from_format = members[ from SUBSEP "var_format" ]
	from_code = getfrom[ type SUBSEP from ]
	gsub( "\\$from", from_member, from_code )

	print "/** Reads a value from //" from_member "// into the object //val//,"
	print "by converting it into a " type " value."
	print ""
	print "The function returns the " type " converted value of //" \
				from_member "//."
	print "*/"

	# Code
	print type " pany_set_" var_type "_from_" from_type \
				"( pany* val, " from " " from_member " )"
	print "{"
	print "	PROC( \"pany_from_" from_type "\" );"
	print "	PARMS( \"val\", \"%p\", val );"
	print "	PARMS( \"" from_member "\", \"" from_format "\", " from_member " );"
	print ""
	print "	if( !val )"
	print "		RETURN( " from_code " );"
	print ""
	print "	pany_set_" var_type "( val, " from_code " );"
	print "	RETURN( pany_get_" var_type "( val ) );"
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
	print "//val// is the pany-object to convert from."
	print ""
	print "The function returns the " type "-value of //val//."

	if( var_type ~ /string/ )
		print "This pointer must be manually freed by the caller."
	print "*/"

	# Code
	print type " pany_to_" var_type "( pany* val )"
	print "{"
	print "	PROC( \"pany_to_" var_type "\" );"
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

	print ""
	print "		default:"
	print "			break;"
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
	print "/** Converts a pany-structure to any supported type."
	print ""
	print "//val// is the pany-object to be converted."
	print "//type// is the type define to which //val// should be converted to."
	print ""
	print "The function returns TRUE on success, FALSE else."
	print "*/"

	#Code
	print "pboolean pany_convert( pany* val, panytype type )"
	print "{"
	print "	PROC( \"pany_convert\" );"
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
		print "			pany_set_" members[ variants[i] SUBSEP "var_type" ]\
			  "( val, pany_to_" members[ variants[i] SUBSEP "var_type" ]\
			  "( val ) );"
		print "			RETURN( TRUE );\n"
	}

	print ""
	print "		default:"
	print "			break;"
	print "	}"
	print ""
	print "	RETURN( FALSE );"
	print "}\n"
}

function fprint_func()
{
	# Dump ---------------------------------------------------------------------
	print "/** Dump pany object to trace. */"
	print "/*MACRO:PANY_DUMP( pany* any )*/"
	print "void _dbg_any_dump( char* file, int line, char* function,"
	print "						char* name, pany* val )"
	print "{"
	print "	switch( val->type )"
	print "	{"

	for( i = 1; i <= variants_cnt; i++ )
	{
		if( okdone[ variants[i] ] )
			continue

		okdone[ variants[i] ] = 1

		print "		case " members[ variants[i] SUBSEP "var_define" ] ":"
		print "			_dbg_trace( file, line, " \
							"\"ANY\", function, \"%s (" \
							members[ variants[i] SUBSEP "var_type" ] \
							") = >" \
							members[ variants[i] SUBSEP "var_format" ] \
							"<\",\n\t\t\t\t\t\t\tname, pany_get_" \
							members[ variants[i] SUBSEP "var_type" ] \
								"( val ) );"

		print "			break;\n"
	}

	print ""
	print "		default:"
	print "			break;"

	print "	}"
	print "}\n"

	# Print --------------------------------------------------------------------
	for( i = 1; i <= variants_cnt; i++ )
		okdone[ variants[i] ] = 0

	# Comment
	print "/** Print the type and value of //val// to //stream//"
	print "without any conversion. This function shall be used for debug only."
	print ""
	print "//stream// is the stream to write to."
	print "//val// is the pany-object to be printed."
	print "*/"

	#Code
	print "void pany_fprint( FILE* stream, pany* val )"
	print "{"
	print "	if( !val )"
	print "	{"
	print "		fprintf( stream, \"%p (empty)\", val );"
	print "		return;"
	print "	}"
	print
	print "	switch( val->type )"
	print "	{"

	for( i = 1; i <= variants_cnt; i++ )
	{
		if( okdone[ variants[i] ] )
			continue

		okdone[ variants[i] ] = 1

		print "		case " members[ variants[i] SUBSEP "var_define" ] ":"
		print "			fprintf( stream, \"%p " \
							members[ variants[i] SUBSEP "var_type" ] " >"\
							members[ variants[i] SUBSEP "var_format" ] \
							"<\\n\",\n\t\t\t\t\tval, pany_get_" \
							members[ variants[i] SUBSEP "var_type" ] \
								"( val ) );"

		print "			break;\n"
	}

	print ""
	print "		default:"
	print "			break;"

	print "	}"
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

