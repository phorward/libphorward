
SUBDIRS = src run min_lalr1 DEMO

man1_MANS = phorward.man
phorwarddir = $(datadir)/phorward
dist_phorward_DATA = LICENSE README

if WITH_TXT2TAGS
# --- UniCC Documentation ------------------------------------------------------
#
# Now documentation generation follows, using txt2tags.
#

README: doc/readme.t2t
	-rm -f $@
	txt2tags -t txt -H -o - $? | sed -E -n '1h;1!H;$${;g;s/ +([-A-Z ]+)\n +(=+)/\2==\n \1 \n\2==/g;p;}' >$@.tmp
	cat doc/readme.hdr $@.tmp >>$@
	rm -f $@.tmp


phorward.man: doc/readme.t2t
	txt2tags -o $@ -t man $?

endif

