#!/bin/sh
. ./run/.phorward >/dev/null

TODAY=`date +"%Y-%m-%d"`
SCRIPT=`basename $0`

# defaults
tools="pdoc pinclude plex pproto pregex ptest"

copy()
{
		if [ $# -ne 2 ];
		then
				echo "Called 'copy' with invalid number of parameters!"
				exit 1
		fi

		for file in $1
		do
				echo -n "Copying $file to $2..."
				cp $file $2

				if [ $? -ne 0 ]
				then
						echo "Unable to copy files into '$2'"
						exit 1
				fi

				echo "Done"
		done
}

merge()
{
		if [ $# -ne 2 ];
		then
				echo "Called 'merge' with invalid number of parameters!"
				exit 1
		fi

		filehead `basename $2` "Phorward C/C++ library; Merged by $SCRIPT on $TODAY" 2>/dev/null >$2
		if [ $? -ne 0 ]
		then
				echo "Unable to write into write file '$2'"
				exit 1
		fi

		echo "#include \"phorward.h\"" >>$2

		for file in $1
		do
				echo -n "Merging $file into $2..."
				sed ':a;N;$!ba;s:/\*[^*]*\*/::;s/^\n\n//g' $file | sed "s/#include.*$//" >>$2
				echo "Done"
		done
}

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
	echo "Folder '$TARGET' exists, please specify another output path"
	exit 1
fi

mkdir $TARGET
if [ $? -ne 0 ]
then
	echo "Failed to create '$TARGET'"
	exit 1
fi

# Assembling files

copy src/phorward.h $TARGET
merge "src/*.c src/regex/*.c" $TARGET/phorward.c
copy "tools/*.[ch]" $TARGET
copy "LICENSE" $TARGET
copy "run/.phorward run/pdoc run/pinclude run/pproto run/ptest" $TARGET
copy "doc/phorward.html doc/phorward.css" $TARGET

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

This is a stand-alone copy of the Phorward C/C++ library that was generated on $TODAY from the [official phorward repository](https://github.com/phorward/libphorward).

It is not intended to be used for further development, but for simple and fast integration into existing projects. To contribute changes, please check out the official repository at https://github.com/phorward/libphorward.

Thank you & have fun!
ENDL

sed "1,3d" README.md >>$TARGET/README.md
