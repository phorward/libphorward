#-------------------------------------------------------------------------------
# NEWPROJECT
# Copyright (C) 2010 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<AT>phorward<DASH>software<DOT>com
#
# Make-File:	Makefile
# Author:		Jan Max Meyer
#-------------------------------------------------------------------------------

include			../../include/Make.inc

PROJECT		=	NEWPROJECT

PROGRAM		=	$(PROJECT)$(EXEEXT)

SOURCE		=	main.c

OBJECTS		=	main$(OBJEXT)

LIBS		=	$(PSTRING_LIB) \
				$(PREGEX_LIB) \
				$(PBASIS_LIB)

PROTOFILE	=	proto.h
				
#-------------------------------------------------------------------------------

all: $(PROGRAM)
	@echo
	@echo --- Compilation succeeded! ---
	
$(PROGRAM): $(PROTOFILE) $(OBJECTS) $(SOURCE) $(LIBS) Makefile
	$(LLINK) $(LINKOPTS)$@ $(OBJECTS) $(LIBS) $(LIBNAME)

$(OBJECTS): $(SOURCE)
	$(CC) $(CEXEOPTS) $(DEBUG) $(SOURCE)

$(PROTOFILE): $(SOURCE)
	cproto_all "$(SOURCE)" $(INCLUDE_DIR) $(PROTOFILE)

clean_obj:
	-$(RM) $(OBJECTS)

clean: clean_obj
	-$(RM) $(PROGRAM)

backup: clean
	-@$(RM) ../$(PROJECT).tar
	tar cvf ../$(PROJECT).tar ../$(PROJECT)

