CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I../src
LIBPHORWARD = ../src/libphorward.a

PPGRAM2C	= ../run/ppgram2c

all: $(PPGRAM2C)

clean:
	-rm ppgram2c.o
	-rm $(PPGRAM2C)

$(PPGRAM2C): ppgram2c.o $(LIBPHORWARD)
	$(CC) -o $@ ppgram2c.o $(LIBPHORWARD)

