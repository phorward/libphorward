all:
	cd src; make -f Makefile.gnu
	cd tools; make -f Makefile.gnu
#	cd examples; make -f Makefile.gnu
#	cd DEMO; make -f Makefile.gnu

clean:
	cd src; make -f Makefile.gnu clean
	cd tools; make -f Makefile.gnu clean
#	cd examples; make -f Makefile.gnu clean
#	cd DEMO; make -f Makefile.gnu clean

make_install:
	cp Makefile.gnu Makefile
	cp src/Makefile.gnu src/Makefile
	cp tools/Makefile.gnu tools/Makefile
#	cp examples/Makefile.gnu examples/Makefile
#	cp DEMO/Makefile.gnu DEMO/Makefile

make_update:
	cp Makefile Makefile.gnu
	cp src/Makefile src/Makefile.gnu
	cp tools/Makefile tools/Makefile.gnu
#	cp examples/Makefile examples/Makefile.gnu
#	cp DEMO/Makefile DEMO/Makefile.gnu

make_uninstall:
	-rm Makefile
	-rm src/Makefile
	-rm tools/Makefile
#	-rm examples/Makefile
#	-rm DEMO/Makefile

