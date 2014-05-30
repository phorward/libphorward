# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Toolkit
# Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See LICENSE for more information.
#
# File:	Makefile
# Usage:	
# ------------------------------------------------------------------------------

INCPHORWARD=../src
LIBPHORWARD=../src/libphorward.a

all: gexpr rd lrrd

clean:
	-rm -f gexpr rd lrrd

gexpr: gexpr.c
	cc -g -o $@ $?

rd: rd.c
	cc -g -o $@ -I $(INCPHORWARD) $? $(LIBPHORWARD)

lrrd: lrrd.c
	cc -g -o $@ $?

