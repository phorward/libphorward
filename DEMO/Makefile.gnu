CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

all: basis regex

clean:
	-rm -f basis.o
	-rm -f regex.o
	-rm -f regex$(EXE)
	-rm -f basis$(EXE)

basis: basis.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) basis.o $(LIBPHORWARD)

regex: regex.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) regex.o $(LIBPHORWARD)
