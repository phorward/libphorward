# ------------------------------------------------------------------------------
# Phorward Software Development Kit
# Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
#
# Make-File:	Make.inc
# Author:		Jan Max Meyer
# ------------------------------------------------------------------------------

#Operating system dependent
PATH_SEP			=	/
PLATFORM			=	$(PSYSTEM)

#Misc
PROTOP				=	.proto

#Extensions
OBJEXT				=	.o
LIBEXT				=	.a
EXEEXT				=	
DOCEXT				=	.tex

#Main C compiler
CC					=	gcc

#Unix tools
CPROTO				=	cproto
LLIB				=	ar
LLINK				=	$(CC)
CAT					=	cat
CP					=	cp
MV					=	mv
RM					=	rm
AWK					=	gawk
TAR					=	tar
GZIP				=	gzip
GUNZIP				=	gunzip
CHMOD				=	chmod

#Paths
INCLUDE_DIR			=	$(PHOME)$(PATH_SEP)include
LIB_DIR				=	$(PHOME)$(PATH_SEP)lib
SRC_DIR				=	$(PHOME)$(PATH_SEP)src
RUN_DIR				=	$(PHOME)$(PATH_SEP)run
DOC_DIR				=	$(PHOME)$(PATH_SEP)doc

#Phorward Software Tools
MIN_LALR1			=	min_lalr1
UNICC				=	unicc
VMEXPRESS			=	vmexpress
MKDOC				=	$(AWK) -f $(RUN_DIR)$(PATH_SEP)mkdoc.awk
LOCALINCLUDE		=	lincl
PPP					=	ppp
MKPROTO				=	mkproto

#Standard options
CFLAGS				=	-funsigned-char -DUTF8 -DUNICODE
CSTDOPTS			=	-I"$(INCLUDE_DIR)" $(CFLAGS)
COPTS				=	-c $(CSTDOPTS)
CLIBOPTS			=	$(COPTS)
CEXEOPTS			=	$(COPTS)

LINKOPTS			=	-lrt -L"$(LIB_DIR)" -o 
LIBOPTS				=	rcs 

#Libnames
PBASIS				=	pbasis
PSTRING				=	pstring
PREGEX				=	pregex

#Header
PBASIS_LIB_H		=	$(INCLUDE_DIR)$(PATH_SEP)$(PBASIS).h
PSTRING_LIB_H		=	$(INCLUDE_DIR)$(PATH_SEP)$(PSTRING).h
PREGEX_LIB_H		=	$(INCLUDE_DIR)$(PATH_SEP)$(PREGEX).h

#Libraries
PBASIS_LIB			=	$(LIB_DIR)$(PATH_SEP)$(PBASIS)$(LIBEXT)
PSTRING_LIB			=	$(LIB_DIR)$(PATH_SEP)$(PSTRING)$(LIBEXT)
PREGEX_LIB			=	$(LIB_DIR)$(PATH_SEP)$(PREGEX)$(LIBEXT)

