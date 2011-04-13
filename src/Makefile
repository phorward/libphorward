#Global Makefile

all:
	makeall
	@echo
	@echo --- $@ done. ---

clean doc:
	makeall $@
	rm -f tags
	@echo
	@echo --- $@ done. ---

tags:
	@find . -name "*.[ch]" -type f ! -regex ".*SCCS.*" | xargs ctags --tag-relative=no 
	@echo
	@echo --- $@ done. ---
