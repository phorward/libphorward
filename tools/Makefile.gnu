CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

PLEX		= ../run/plex
PPARSE		= ../run/pparse
PREGEX		= ../run/pregex
PPGRAM2C	= ../run/ppgram2c
PVM			= ../run/pvm

all: $(PLEX) $(PPARSE) $(PREGEX) $(PPGRAM2C) $(PVM)

clean:
	-rm plex.o
	-rm pparse.o
	-rm pregex.o
	-rm ppgram2c.o
	-rm pvm.o
	-rm $(PLEX) $(PPARSE) $(PREGEX) $(PPGRAM2C) $(PVM)

$(PLEX): plex.o $(LIBPHORWARD)
	$(CC) -o $@ plex.o $(LIBPHORWARD)

$(PPARSE): pparse.o $(LIBPHORWARD)
	$(CC) -o $@ pparse.o $(LIBPHORWARD)

$(PREGEX): pregex.o $(LIBPHORWARD)
	$(CC) -o $@ pregex.o $(LIBPHORWARD)

$(PPGRAM2C): ppgram2c.o $(LIBPHORWARD)
	$(CC) -o $@ ppgram2c.o $(LIBPHORWARD)

$(PVM): pvm.o $(LIBPHORWARD)
	$(CC) -o $@ pvm.o $(LIBPHORWARD)

