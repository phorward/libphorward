# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: Regular Expression Library, Version 2
# Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include ../../include/Make.inc

SRC			=	comp.c \
				dfa.c \
				direct.c \
				nfa.c \
				ref.c \
				util.c

OBJ			=	comp$(OBJEXT) \
				dfa$(OBJEXT) \
				direct$(OBJEXT) \
				nfa$(OBJEXT) \
				ref$(OBJEXT) \
				util$(OBJEXT) 

PROTO		=	pregex.proto.h
SPROTO		=	pregex.sproto.h
PROTO_H		=	$(INCLUDE_DIR)$(PATH_SEP)$(PROTO)

HEADERS		=	pregex.h $(SPROTO)

DOCFILE		= 	$(DOC_DIR)$(PATH_SEP)pregex.html

# ------------------------------------------------------------------------------

all: $(PREGEX_LIB_H) $(PREGEX_LIB)
	@echo
	@echo --- Compilation succeeded! ---

$(PREGEX_LIB): $(SRC) $(HEADERS) Makefile
	$(CC) $(CLIBOPTS) $(DEBUG) $(SRC)
	$(LLIB) $(LIBOPTS)$(PREGEX_LIB) $(OBJ)

$(PREGEX_LIB_H): $(SRC) $(HEADERS) Makefile $(PROTO)
	$(LOCALINCLUDE) pregex.h >$@

$(PROTO): $(SRC) $(HEADERS)
	cproto_all "$(SRC)" $(INCLUDE_DIR) $@

$(SPROTO): $(SRC) 
	cproto_all "$(SRC)" $(INCLUDE_DIR) $@ -S

clean_obj:
	-@$(RM) $(OBJ)

clean: clean_obj
	-$(RM) $(PREGEX_LIB)
	-$(RM) $(PREGEX_LIB_H)
	-$(RM) $(PROTO_H)
	-$(RM) $(DOCFILE)

backup: clean
	-@$(RM) ../pregex.tar
	tar cvf ../pregex.tar ../pregex

#make doc
doc: 
	$(MKDOC) -ns -i \
		-var module pregex \
		-var title "Phorward Regular Expression Library" \
		-var libext $(LIBEXT) \
		 $(SRC) >$(DOCFILE)
	@echo
	@echo --- Done making documentation! ---

