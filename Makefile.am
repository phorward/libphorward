# -MAKEFILE---------------------------------------------------------------------
# Phorward Foundation Toolkit
# Copyright (C) 2006-2016 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
# All rights reserved. See LICENSE for more information.
#
# File:		Makefile.am
# Usage:	Automake makefile.
# ------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src run tools DEMO

man1_MANS = phorward.man
phorwarddir = $(datadir)/phorward
dist_phorward_DATA = LICENSE README.md CHANGELOG

if WITH_TXT2TAGS
# --- Phorward Documentation --------------------------------------------------
#
# Now documentation generation follows, using txt2tags.
#

README.md: doc/readme.t2t
	-rm -f $@
	txt2tags -t txt -H -o - $? | sed -E -n '1h;1!H;$${;g;s/ +([-A-Z ]+)\n +(=+)/\1\n\2/g;p;}' | sed 's/^  /    /g' >$@

phorward.man: doc/readme.t2t
	txt2tags -o $@ -t man $?

endif

