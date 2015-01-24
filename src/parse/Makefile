
PATHEXT = PATH="../../run:$(PATH)"
GRAMTPL = comp.gram.tpl
GRAMSRC = comp.gram.c

# generating parser for ppgram
$(GRAMSRC): bnf.syn $(GRAMTPL)
	$(PATHEXT) filehead $@ "Self-hosted grammar for ppgram." >$@
	$(PATHEXT) pinclude $(GRAMTPL) >>$@

clean:
	rm $(GRAMSRC)
