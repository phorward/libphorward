CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

all: basis

clean:
	-rm -f basis.o
	-rm -f basis$(EXE)

basis: basis.o $(LIBPHORWARD)
	$(CC) -o $@$(EXE) basis.o $(LIBPHORWARD)

