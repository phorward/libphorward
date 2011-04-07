#-------------------------------------------------------------------------------
# Phorward Software Development Kit
# Copyright (C) 2009, 2010 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
#
# Script:		mkdoc.awk
# Author:		Jan Max Meyer
# Usage:		Template-based documentation generator
#-------------------------------------------------------------------------------

BEGIN	{
			SUBSEP = "#"

			print_sections = 1 
			full_index = 0
			tpldir = getenv( "PHOME" ) "/tpl"
			manual = ""

			t = 1

			# Begin of program
			for( i = 1; i < ARGC; i++ )
			{
				if( ARGV[i] == "-o" )
					output = ARGV[++i]
				else if( ARGV[i] == "-i" )
					full_index = 1
				else if( ARGV[i] == "-ns" )
					print_sections = 0
				else if( ARGV[i] == "-tpl" && i+1 < ARGC )
					tpldir = ARGV[++i]
				else if( ARGV[i] == "-m" && i+1 < ARGC )
					manual = readfile( ARGV[++i] )
				else if( ARGV[i] == "-c" && i+1 < ARGC )
					allcopyright = ARGV[++i]
				else if( ARGV[i] == "-t" && i+1 < ARGC )
					alltitle = ARGV[++i]
				else if( ARGV[i] == "-u" && i+1 < ARGC )
					allusage = ARGV[++i]
				else
					files[ j++ ] = ARGV[i]
			}
			
			if( substr( tpldir, length( tpldir ) - 1, 1 ) != "/" )
				tpldir = tpldir "/"
			
			# Read all templates
			tpl[ "DOCUMENT" ] = readfile( tpldir "document.tpl" )
			tpl[ "MODULE" ] = readfile( tpldir "module.tpl" )
			tpl[ "FUNCTION" ] = readfile( tpldir "function.tpl" )
			tpl[ "parameters" ] = readfile( tpldir "parameter.tpl" )
			tpl[ "returns" ] = readfile( tpldir "returns.tpl" )
			tpl[ "refentry" ] = readfile( tpldir "refentry.tpl" )
			
			document = tpl[ "DOCUMENT" ]
			
			# Process the files
			for( i in files )
			{
				process( files[i] )

				if( print_sections )
				{
					#Output the header and index of contents
					gsub( "!!" "index", print_idx(), head_text )
					gsub( "!!" "contents", print_ref(), head_text )

					#all_text = all_text head_text
					#all_text = all_text print_ref()
					
					all_text = all_text head_text

					head_text = ""
				}

				if( full_index )
				{
					for( s in functions )
						all_functions[ s ] = functions[ s ]
				} 
				
				for( s in functions )
					delete functions[s]
			}

			if( full_index )
			{
				for( s in all_functions )
					functions[ s ] = all_functions[ s ]

				if( !print_sections )
					all_text = print_ref()

				allindex = print_idx()
			}
			
			if( document != "" )
			{
				gsub( "!!" "title", alltitle, document )
				gsub( "!!" "usage", allusage, document )
				gsub( "!!" "copyright", allcopyright, document )
				gsub( "!!" "manual", manual, document )
				gsub( "!!" "index", allindex, document )
				gsub( "!!" "reference", all_text, document )
			}
			else
				document = all_text
				
			#Finally print all!
			print document
		}
		

function process( file )
{
	section = ""
	current_item = ""
	old_item = ""
	nodoc = 0
	line = 1
	
	while( getline line <file > 0 )
	{
		#this could be windoof files
		gsub( "\r", "", line )		
		line = trim( line )
		
		section_line++

		if( line == "" )
		{
			if( current_item == "usage" || current_item == "copyright" )
				items[ current_item ] = items[ current_item ] "\n"

			continue
		}
		
		#Look for section?
		if( section == "NODOC" && index( line, "COD_ON" ) >= 1 )
			section = "";
		else if( section == "" )
		{
			if( index( line, "/* -MODULE" ) == 1 )
				section = "MODULE"
			else if( index( line, "/* -FUNCTION" ) == 1 )
				section = "FUNCTION"
			else if( index( line, "NO_DOC" ) >= 1 )
				section = "NODOC";

			current_item = ""
			section_line = 0
		}
		else if( section != "NODOC" )
		{
			#End of a section
			if( index( line, "--------------------------------------------" \
					"--------------------------------- */" ) == 1 \
				|| index( line, "~~~ CHANGES & NOTES ~~~" ) > 0 )
			{
				# create a function parameter synopsis!
				if( section == "FUNCTION" )
				{
					#Generate RETURNTYPE and RETURNDESC
					if( items[ "returns" ] != "" )
					{
						if( index( items[ "returns" ], " " ) )
						{
							datatype = substr( items[ "returns" ], 1, \
										index( items[ "returns" ], " " ) - 1 )
							desc = 	substr( items[ "returns" ], \
										index( items[ "returns" ], " " ) + 1 )
						}
						else
						{
							datatype = items[ "returns" ]
							desc = "Nothing"
						}

						items[ "returntype" ] = datatype

						if( desc == "" )
							desc = "Nothing."

						items[ "returndesc" ] = desc
					}
					
					#Generate function-name
					gsub( /\(.*\)$/, "", items[ "function" ] )

					#GENERATE SYNOPSIS
					items[ "synopsis" ] = items[ "function" ]
					#gsub( /\(.*\)$/, "", items[ "synopsis" ] )
					items[ "synopsis" ] = items[ "synopsis" ] "( "
					
					first = 1
					for( item in items )
					{
						if( match( item, "parameters" SUBSEP "[0-9]+" ) )
						{
							datatype = substr( items[ item ], 1, \
								index( items[ item ], " " ) - 1 )
							desc = substr( items[ item ], \
										index( items[ item ], " " ) + 1 )

							if( datatype != "..." )
							{
								param = substr( desc, 1, index( desc, " " ) - 1 )
								desc = 	substr( desc, index( desc, " " ) + 1 )
							}
							else
								param = ""
							
							if( !first )
								items[ "synopsis" ] = items[ "synopsis" ] ", "
							first = 0
							
							items[ "synopsis" ] = items[ "synopsis" ] datatype
							if( param != "" )
								items[ "synopsis" ] = items[ "synopsis" ] \
									" " param
						}
					}
					
					if( first )
						items[ "synopsis" ] = items[ "synopsis" ] "void )"
					else
						items[ "synopsis" ] = items[ "synopsis" ] " )"

					items[ "synopsis" ] = items[ "returntype" ] " " \
											items[ "synopsis" ]
					
					#Put code in array first, for sorting
					funcname =  items[ "function" ]
					#gsub( /_|\(|\)/, "", funcname )

					# GENERATE OUTPUT...
					#write index of contents as a quick reference
					if( !index( items[ "usage" ], "." ) )
						items[ "usage" ] = trim( items[ "usage" ] ) "."

					content[ funcname ] = substr( items[ "usage" ], \
											1, index( items[ "usage" ], "." ) )

					#Write output into functions array
					functions[ funcname ] = fill_tex_tpl( section, items )
				}
				else if( head_text == "" )
					head_text = fill_tex_tpl( section, items )
				
				#for( item in items )
					#print item " => \"" items[ item ] "\""
				
				for( item in items )
					delete items[ item ]
					
				section = ""
				current_item = ""
				continue
			}
			else if( index( line, "File:" ) == 1 )
				current_item = "file"
			else if( index( line, "Author:" ) == 1 )
				current_item = "author"
			else if( index( line, "Usage:" ) == 1 )
				current_item = "usage"
			else if( index( line, "Function:" ) == 1 )
				current_item = "function"
			else if( index( line, "Parameters:" ) == 1 )
			{
				akt_parm = 1
				current_item = "parameters" SUBSEP akt_parm
			}
			else if( index( line, "Returns:" ) == 1 )
			{
				current_item = "returns"
			}
			else if( section == "MODULE" && section_line == 1 )
				current_item = "title"
			else if( section == "MODULE" && section_line == 2 )
				current_item = "copyright"

			#Read in the item information
			if( current_item != "" )
			{
				if( current_item != old_item && items[ current_item ] == "" )
				{
					if( current_item != "title" \
						&& current_item != "copyright" )
					{
						line = trim( substr( line, index( line, ":" ) + 1 ) )
					}

					gsub( /[ \t]+/, " ", line )

					items[ current_item ] = line
				}
				else
				{
					if( match( current_item, "parameters" SUBSEP "[0-9]+" ) )
					{
						if( match( line, \
							"[A-Za-z0-9_*]+[ \t][ \t]+[A-Za-z0-9_*()]+[ \t]+|\\.\\.\\.[ \t]+" ) )
						{
							akt_parm++
							current_item = "parameters" SUBSEP akt_parm 
							
							gsub( /[ \t]+/, " ", line )
							items[ current_item ] = line
						}
					}
					
					if( old_item == current_item )
					{
						gsub( /[ \t]+/, " ", line )

						if( ( section == "MODULE" && current_item == "usage" ) \
							|| current_item == "copyright" )
							delim = "\n"
						else
							delim = " "

						items[ current_item ] = items[ current_item ] delim line
					}
				}
			}
	
			old_item = current_item
		}
		
		line++;
	}
	
	close( getline line <file > 0 )
}

function fill_tex_tpl( section, items )
{
	final = tpl[ section ]
	parameters = ""

	for( item in items )
	{
		#print "!!" item " " items[ item ]

		#for LaTeX
		#gsub( "_", "\\_", items[ item ] )

		if( match( item, "parameters" SUBSEP "[0-9]+" ) )
		{
			datatype = substr( items[ item ], 1, \
						index( items[ item ], " " ) - 1 )
			desc = 	substr( items[ item ], index( items[ item ], " " ) + 1 )
			
			if( datatype != "..." )
			{
				param = substr( desc, 1, index( desc, " " ) - 1 )
				desc = 	substr( desc, index( desc, " " ) + 1 )
			}
			else
				param = ""
								
			subfinal = tpl[ "parameters" ]
			gsub( "!!" "datatype", datatype, subfinal )
			gsub( "!!" "param", param, subfinal )
			gsub( "!!" "desc", desc, subfinal )
			
			parameters = parameters subfinal
		}
		else if( item == "returns" )
		{
			subfinal = tpl[ "returns" ]
			gsub( "!!" "datatype", items[ "returntype" ], subfinal )
			gsub( "!!" "desc", items[ "returndesc" ], subfinal )
			
			gsub( "!!" item, subfinal, final )
		}
		else
		{			
			gsub( "!!" item, items[ item ], final )
		}
	}

	if( section == "FUNCTION" )
		gsub( "!!parameters", parameters, final )
	
	return final
}

function print_idx()
{
	t = 1
	for( s in functions )
		sort[t++] = s

	n = asort( sort )

	refindex = ""
	for( s = 1; s <= n; s++ )
	{
		refentry = tpl[ "refentry" ]
		
		gsub( "!!" "function", sort[s], refentry )
		gsub( "!!" "usage", content[ sort[s] ], refentry )
		refindex = refindex refentry
	}

	for( s in sort )
		delete sort[s]

	return refindex
}

function print_ref()
{
	t = 1
	for( s in functions )
		sort[t++] = s

	n = asort( sort )

	text = ""
	for( s = 1; s <= n; s++ )
		text = text functions[sort[s]]

	for( s in sort )
		delete sort[s]

	return text
}

#--- Utility functions go here ---

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

function readfile( s )
{
	f = ""

	while( ( getline l < s ) > 0 )
		f = f l "\n"
	
	close( s )
	return f
}

function getenv( s )
{
	"echo $" s | getline v
	return v
}
