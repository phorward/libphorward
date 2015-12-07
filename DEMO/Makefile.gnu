CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

all: basis regex parse testvm

clean:
	-rm -f basis.o
	-rm -f regex.o
	-rm -f parse.o
	-rm -f testvm.o
	-rm -f regex$(EXE)
	-rm -f basis$(EXE)
	-rm -f parse$(EXE)
	-rm -f testvm$(EXE)

basis: basis.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) basis.o $(LIBPHORWARD)

regex: regex.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) regex.o $(LIBPHORWARD)

parse: parse.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) parse.o $(LIBPHORWARD)

testvm: testvm.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) testvm.o $(LIBPHORWARD)

