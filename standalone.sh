#!/bin/sh
. ./run/.phorward >/dev/null

# defaults
tools="pdoc pinclude plex pproto pregex ptest"

help()
{
	echo "Usage: `basename $0` OPTIONS target-directory"
	echo
	echo "    -h  --help               Show this help, and exit."
	#echo "    -t  --tools    LIST      List of tools to copy"
	#echo "                             (default: $tools)"
	echo "    -V  --version            Show version info and exit."
	echo
}

#assemble options

while [ "$1" ]
do
	case "$1" in
		-h|--help)
		    help
		    exit 0
		    ;;
		#-t|--tools)
		#	tools=$2
		#	shift
		#	;;
		-V|--version)
		    version `basename $0` "Stand-alone packager"
		    exit 0
		    ;;
		*)
			break
			;;
	esac

	shift
done


TARGET=${1:-"dist"}

if [ -d "$TARGET" ]
then
	echo "Folder `$TARGET` exists, please specify another output path"
	exit 1
fi

mkdir $TARGET
if [ $? -ne 0 ]
then
	echo "Failed to create `$TARGET`"
	exit 1
fi

# Copy source files
cp -v src/phorward.h src/*.c src/regex/*.c tools/*.[ch] $TARGET

if [ $? -ne 0 ]
then
	echo "Unable to copy files into `$TARGET`"
	exit 1
fi

# Copy tools
cp -v run/.phorward run/pdoc run/pinclude run/pproto run/ptest $TARGET

if [ $? -ne 0 ]
then
	echo "Unable to copy files into `$TARGET`"
	exit 1
fi

# Copy docs
cp -v doc/phorward.html doc/phorward.css $TARGET

if [ $? -ne 0 ]
then
	echo "Unable to copy files into `$TARGET`"
	exit 1
fi

# Generating a Makefile

cat <<ENDL >$TARGET/Makefile
LIBPHORWARD = libphorward.a
PREGEX = pregex
PLEX = plex

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I. -Wall
LDLIBS += libphorward.a

ENDL

echo "SRC = \\" >>$TARGET/Makefile
find $TARGET -name "*.c" -printf "\t\$(PREFIX)%P \\\\\n" >>$TARGET/Makefile

cat <<ENDL >>$TARGET/Makefile


OBJ = \$(patsubst %.c,%.o,\$(SRC))

all: \$(LIBPHORWARD) \$(PREGEX) \$(PLEX)

\$(LIBPHORWARD): phorward.h \$(OBJ)
	\$(AR) rv \$@ \$(OBJ)

clean:
	-rm -f \$(OBJ)
	-rm -f \$(LIBPHORWARD) \$(PREGEX) \$(PLEX)

ENDL
 
cat <<ENDL >$TARGET/README.md
# phorward v$VERSION

This is a stand-alone copy of the Phorward C/C++ library that was generated on `date +"%Y-%m-%d"` from the official phorward repository.

It is not intended to be used for further development. To contribute changes, please check out the official repository, which can be found here:
https://github.com/phorward/phorward

Thank you & have fun!
ENDL

sed "1,3d" README.md >>$TARGET/README.md
