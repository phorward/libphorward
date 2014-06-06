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
	base/list.c \
	base/memory.c \
	base/stack.c \
	ccl/ccl.c \
	regex/dfa.c \
	regex/direct.c \
	regex/misc.c \
	regex/nfa.c \
	regex/pattern.c \
	regex/regex.c \
	regex/range.c \
	regex/ref.c \
	string/convert.c \
	string/string.c \
	string/utf8.c \
	util/system.c \
	xml/xml.c \
	\
	value/value.c \
	value/value.conv.c \
	value/value.get.c \
	value/value.set.c \
	grammar/ast.c \
	grammar/astnode.c \
	grammar/bnf.c \
	grammar/error.c \
	grammar/grammar.c \
	grammar/nonterm.c \
	grammar/prod.c \
	grammar/symbol.c \
	grammar/term.c \
	grammar/token.c \
	\
	lexer/lexer.c \
	\
	parser/ll.gen.c \
	parser/ll.parse.c \
	parser/lr.gen.c \
	parser/lr.parse.c \
	parser/parser.c

HSRC = \
	base/dbg.h \
	base/list.h \
	base/stack.h \
	grammar/grammar.h \
	lexer/lexer.h \
	parser/parser.h \
	phorward.tpl.h \
	regex/regex.h \
	value/value.h \
	xml/xml.h \
	$(PROTOFILE)

OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD) lrtest$(EXE) lltest$(EXE)

lrtest$(EXE): lrtest.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) lrtest.o $(LIBPHORWARD)

lltest$(EXE): lltest.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) lltest.o $(LIBPHORWARD)

$(LIBPHORWARD): $(LIBHEADER) $(OBJ)
	$(AR) rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD)
	-rm -f lrtest lrtest.o
	-rm -f lltest lltest.o

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

