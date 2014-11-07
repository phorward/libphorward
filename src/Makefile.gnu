# This is a plain Makefile for general Phorward Foundation Toolkit development.
# It compiles libphorward.a quickly and out of the box without autotools.

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I.

AWK=awk
LIBPHORWARD = libphorward.a
PROTOFILE =	proto.h
LIBHEADER = phorward.h
PATHEXT = PATH="../run:$(PATH)"

SRC = \
	base/array.c \
	base/dbg.c \
	base/list.c \
	base/memory.c \
	ccl/ccl.c \
	regex/dfa.c \
	regex/direct.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/regex.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/system.c \
	xml/xml.c \
	parse/ast.c \
	parse/gram.c \
	parse/ll.c \
	parse/lr.c \
	parse/scan.c \
	value/value.c \
	value/value.conv.c \
	value/value.get.c \
	value/value.set.c

HSRC = \
	base/array.h \
	base/dbg.h \
	base/list.h \
	parse/parse.h \
	phorward.tpl.h \
	regex/regex.h \
	value/value.h \
	xml/xml.h \
	$(PROTOFILE)

OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD)

$(LIBPHORWARD): $(LIBHEADER) $(OBJ)
	$(AR) rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD)

clean_all: clean
	-rm value/value.get.c
	-rm value/value.set.c
	-rm value/value.conv.c

# Prototypes Files
$(PROTOFILE): $(SRC)
	$(PATHEXT) pproto $(SRC) >$@

# Library Header
$(LIBHEADER): $(HSRC)
	$(PATHEXT) pinclude phorward.tpl.h >$@

# Variant Data Type Modules (generated from definitions in var.h comments)
value/value.get.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_get=1 value/value.h >$@

value/value.set.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_set=1 value/value.h >$@

value/value.conv.c: value/value.h value/value.gen.awk
	$(AWK) -f value/value.gen.awk -vwith_conv=1 value/value.h >$@

