PATHEXT = PATH="../../run:$(PATH)"
GRAMBNF = bnf.syn
GRAMSRC = bnf.c

# generating parser for ppgram
$(GRAMSRC): $(GRAMBNF)
	$(PATHEXT) filehead $@ \
		"Self-hosted grammar for ppgram, generated from $(GRAMBNF)" >$@
	echo "/* DO NOT CHANGE THIS FILE MANUALLY. IT WILL GO AWAY! */" >>$@
	echo "" >>$@
	echo "#include \"phorward.h\"" >>$@
	echo "" >>$@
	echo "void pp_bnf_define( ppgram* g )" >>$@
	echo "{" >>$@
	$(PATHEXT) ppgram2c --indent 1 bnf.syn >>$@
	echo "}" >>$@

touch:
	touch $(GRAMBNF)

clean:
	rm $(GRAMSRC)
