#
# SCons build file for Phorward Foundation Toolkit
#

import os

env = Environment( ENV = os.environ, CFLAGS = "-DUNICODE -DUTF8" )

env.Append( BUILDERS =
	{ 	
		"mkproto" :
		Builder(
			action = "pproto $SOURCES >$TARGET",
			suffix=".h",
			src_suffix=".c" 
		),
		"lincl" :
		Builder(
			action = "pinclude $SOURCE >$TARGET",
			suffix=".h",
			src_suffix=".h" 
		) 
	} )


env[ "libdir" ] = "#lib"
env[ "rundir" ] = "#run"
env[ "incdir" ] = "#include"

env[ "lphorward" ] = "phorward"

env[ "CPPPATH" ] = env[ "incdir" ]
#env[ "LIBPATH" ] = env[ "libdir" ]
env.AppendENVPath( "PATH", env[ "rundir" ] )
env.Decider( "timestamp-newer" )

Export( 'env' )

SConscript( 'src/SConscript' )

env.Program( "#run/min_lalr1", "min_lalr1/min_lalr1.c" )
env.Program( "DEMO/basis", "DEMO/basis.c",
				LIBS = '$lphorward', LIBPATH = '$libdir' )
env.Program( "DEMO/regex", "DEMO/regex.c", 
				LIBS = '$lphorward', LIBPATH = '$libdir' )
