# -MAKE-------------------------------------------------------------------------
# Phorward Foundation Libraries :: String Object Library
# Copyright (C) 2010, 2011 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
# ------------------------------------------------------------------------------

include		../../include/Make.inc

SRC			=	get.c \
				set.c \
				pstring.c \
				util.c

OBJ			=	get$(OBJEXT) \
				set$(OBJEXT) \
				pstring$(OBJEXT) \
				util$(OBJEXT)

PROTO		=	pstring.proto.h

HEADERS		=	pstring.h
LHEADERS	=	local.h

DOCFILE		= 	$(DOC_DIR)$(PATH_SEP)pstring$(DOCEXT)

# ------------------------------------------------------------------------------

all: $(PSTRING_LIB_H) $(PSTRING_LIB)
	@echo
	@echo --- Compilation succeeded! ---

$(PSTRING_LIB): $(SRC) $(LHEADERS) $(HEADERS) Makefile
	$(CC) $(CLIBOPTS) $(DEBUG) $(SRC)
	$(LLIB) $(LIBOPTS)$(PSTRING_LIB) $(OBJ)

$(PSTRING_LIB_H): $(SRC) $(HEADERS) $(PROTO) Makefile
	$(LOCALINCLUDE) pstring.h >$@

$(PROTO): $(SRC) $(HEADERS)
	cproto_all "$(SRC)" $(INCLUDE_DIR) $@

clean_obj:
	-@$(RM) $(OBJ)

clean: clean_obj
	-$(RM) $(PSTRING_LIB)
	-$(RM) $(PSTRING_LIB_H)
	-$(RM) $(PROTO_H)
	-$(RM) $(DOCFILE)

backup: clean
	-@$(RM) ../pstring.tar
	tar cvf ../pstring.tar ../pstring

#make doc
doc: 
	$(MKDOC) -ns -i \
		-t $(DOCTARGET) \
		-var module pstring \
		-var title "Phorward String Object Library" \
		-var libext $(LIBEXT) \
		 $(SRC) >$(DOCFILE)
	@echo
	@echo --- Done making documentation! ---

