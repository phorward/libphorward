# This is a plain Makefile for general Phorward Foundation Toolkit development.
# It compiles libphorward.a quickly and out of the box without autotools.

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I.

AWK=awk
LIBPHORWARD = libphorward.a
PROTOFILE =	proto.h
LIBHEADER = phorward.h
PATHEXT = PATH="../run:$(PATH)"

SRC = \
	base/dbg.c \
	base/llist.c \
	base/plist.c \
	base/memory.c \
	base/stack.c \
	regex/ccl.c \
	regex/dfa.c \
	regex/direct.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/pregex.c \
	regex/range.c \
	regex/ref.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/system.c \
	union/union.c \
	union/union.conv.c \
	union/union.get.c \
	union/union.set.c \
	xml/xml.c \
	\
	grammar/asttype.c \
	grammar/grammar.c \
	grammar/nonterm.c \
	grammar/production.c \
	grammar/symbol.c \
	grammar/terminal.c \
	grammar/token.c \
	\
	lexer/lexer.c \
	\
	parser/lr.gen.c \
	parser/lr.parse.c \
	parser/parser.c

HSRC = \
	base/dbg.h \
	base/llist.h \
	base/plist.h \
	base/stack.h \
	grammar/grammar.h \
	lexer/lexer.h \
	parser/parser.h \
	phorward.tpl.h \
	regex/pregex.h \
	union/union.h \
	xml/xml.h \
	$(PROTOFILE)

OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD) ptest

ptest: ptest.o $(LIBPHORWARD)
	$(CC) -o $@ ptest.o $(LIBPHORWARD)

$(LIBPHORWARD): $(LIBHEADER) $(OBJ)
	ar rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD)
	-rm -f ptest ptest.o

clean_all: clean
	-rm union/union.get.c
	-rm union/union.set.c
	-rm union/union.conv.c

# Prototypes Files
$(PROTOFILE): $(SRC)
	$(PATHEXT) pproto $(SRC) >$@

# Library Header
$(LIBHEADER): $(HSRC)
	$(PATHEXT) pinclude phorward.tpl.h >$@

# Variant Data Type Modules (generated from definitions in var.h comments)
union/union.get.c: union/union.h union/union.gen.awk
	$(AWK) -f union/union.gen.awk -vwith_get=1 union/union.h >$@

union/union.set.c: union/union.h union/union.gen.awk
	$(AWK) -f union/union.gen.awk -vwith_set=1 union/union.h >$@

union/union.conv.c: union/union.h union/union.gen.awk
	$(AWK) -f union/union.gen.awk -vwith_conv=1 union/union.h >$@

