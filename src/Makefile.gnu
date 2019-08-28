# This is a plain Makefile for general Phorward C/C++ Library development.
# It compiles libphorward.a quickly and out of the box without autotools.

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I. -Wall

AWK=awk
LIBPHORWARD = libphorward.a
PROTOFILE =	proto.h
LIBHEADER = phorward.h
PATHEXT = PATH="../run:$(PATH)"

REF = ../doc/ref.t2t

SRC = \
	array.c \
	ccl.c \
	convert.c \
	dbg.c \
	list.c \
	memory.c \
	string.c \
	system.c \
	utf8.c \
	regex/dfa.c \
	regex/direct.c \
    regex/lex.c \
	regex/misc.c \
	regex/nfa.c \
	regex/ptn.c \
	regex/regex.c

HSRC = \
	array.h \
	ccl.h \
	dbg.h \
	list.h \
	regex/regex.h \
	version.h \
	phorward.tpl.h \
	$(PROTOFILE)

OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD)

version.h:
	echo "#define LIBPHORWARD_VERSION \"`../run/.phorward`\"" >$@

$(LIBPHORWARD): $(LIBHEADER) $(OBJ)
	$(AR) rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD)

clean_all: clean
	-rm $(LIBHEADER)
	-rm version.h

ref: $(SRC)
	echo "% --- DON'T EDIT THIS FILE, IT IS GENERATED FROM SRC/ ---" >$(REF)
	echo "" >>$(REF)
	echo "" >>$(REF)
	echo "" >>$(REF)
	pdoc -d 2 $(SRC) $(HSRC) >>$(REF)


# Prototypes Files
$(PROTOFILE): $(SRC)
	$(PATHEXT) pproto $(SRC) >$@

# Library Header
$(LIBHEADER): $(HSRC)
	$(PATHEXT) filehead phorward.h "Phorward C/C++ Library Global Header" \
		>$@
	$(PATHEXT) pinclude phorward.tpl.h >>$@

# Test suite
test: $(LIBPHORWARD)
	LD_LIBRARY_PATH="`pwd`" LDFLAGS="-lphorward" CFLAGS="-I `pwd` -DDEBUG -DUTF8 -DUNICODE -g" ptest -v $(SRC)
