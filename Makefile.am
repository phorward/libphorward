# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Toolkit
# Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See LICENSE for more information.
#
# File:		Makefile.am
# Usage:	Automake makefile.
# ------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src tools

man1_MANS = phorward.1.man
phorwarddir = $(datadir)/phorward
dist_phorward_DATA = LICENSE README.md CHANGELOG doc/phorward.html doc/phorward.css doc/phorward.png

if WITH_TXT2TAGS
# --- Phorward Documentation --------------------------------------------------
#
# Now documentation generation follows, using txt2tags.
#

README.md: doc/readme.t2t
	-rm -f $@
	txt2tags -t md -H -o $@ $?

phorward.1.man: doc/readme.t2t
	txt2tags -o $@ -t man $?

endif

