#!/bin/sh
. ./run/.phorward >/dev/null

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

cp -v src/phorward.h src/*.c src/regex/*.c LICENSE run/.phorward run/pdoc run/pinclude run/pproto run/ptest tools/*.[ch] doc/phorward.html doc/phorward.css $TARGET

if [ $? -ne 0 ]
then
	echo "Unable to copy files into `$TARGET`"
	exit 1
fi

cd $TARGET

# Generating a Makefile

cat <<ENDL >Makefile
LIBPHORWARD = libphorward.a
PREGEX = pregex
PLEX = plex

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I. -Wall
LDLIBS += libphorward.a

ENDL

echo "SRC = \\" >>Makefile
find . -name "*.c" -printf "\t\$(PREFIX)%P \\\\\n" >>Makefile

cat <<ENDL >>Makefile


OBJ = \$(patsubst %.c,%.o,\$(SRC))

all: \$(LIBPHORWARD) \$(PREGEX) \$(PLEX)

\$(LIBPHORWARD): phorward.h \$(OBJ)
	\$(AR) rv \$@ \$(OBJ)

clean:
	-rm -f \$(OBJ)
	-rm -f \$(LIBPHORWARD) \$(PREGEX) \$(PLEX)

ENDL
 
cat <<ENDL >README.md
libphorward v$VERSION
------------------------------------------------------------------------------
This is a stand-alone version of the Phorward C/C++ library that was generated
on `date +"%Y-%m-%d"` from the official phorward repository.

It is not intended to be used for further development. To contribute changes, please check out the official repository, which can be found here:

	https://github.com/phorward/phorward

Thank you & have fun!
ENDL

