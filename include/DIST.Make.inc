# ------------------------------------------------------------------------------
# Phorward Software Development Kit
# Copyright (C) 2009-2012 by Phorward Software Technologies, Jan Max Meyer
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

#Documentation Generator
TXT2TAGS			=	t2t
HTML				=	html
DOCTARGET			=	$(HTML)

#Extensions
OBJEXT				=	.o
LIBEXT				=	.a
EXEEXT				=	
TXT2TAGSEXT			=	.$(TXT2TAGS)
HTMLEXT				=	.$(HTML)
DOCEXT				=	$(HTMLEXT)

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
CFLAGS				=	-I"$(INCLUDE_DIR)" $(DEBUG -funsigned-char \
							-DUTF8 -DUNICODE
LDFLAGS				=	-lrt
CSTDOPTS			=	$(CFLAGS)
COPTS				=	-c $(CSTDOPTS)
CLIBOPTS			=	$(COPTS)
CEXEOPTS			=	$(COPTS)

LINKOPTS			=	-lrt -L"$(LIB_DIR)" -o 
LIBOPTS				=	rcs 

#Phorward Library
LIBPHORWARD			=	libphorward
LIBPHORWARD_H		=	$(INCLUDE_DIR)$(PATH_SEP)phorward.h
LIBPHORWARD_LIB		=	$(LIB_DIR)$(PATH_SEP)$(LIBPHORWARD)$(LIBEXT)

