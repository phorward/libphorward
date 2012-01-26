# ------------------------------------------------------------------------------
# Phorward Software Development Kit
# Copyright (C) 2009-2012 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See $PHOME/LICENSE for more information.
#
# Make-File:	Make.inc
# Author:		Jan Max Meyer
# ------------------------------------------------------------------------------

#Phorward Software Tools
MIN_LALR1			=	min_lalr1
UNICC				=	unicc
VMEXPRESS			=	vmexpress
MKDOC				=	$(AWK) -f $(RUN_DIR)$(PATH_SEP)mkdoc.awk
LOCALINCLUDE		=	lincl
PPP					=	ppp
MKPROTO				=	mkproto

#Phorward Library
LIBPHORWARD			=	libphorward
LIBPHORWARD_H		=	$(INCLUDE_DIR)$(PATH_SEP)phorward.h
LIBPHORWARD_LIB		=	$(LIB_DIR)$(PATH_SEP)$(LIBPHORWARD)$(LIBEXT)

