CFLAGS = -DDEBUG -I..

pp: main.o ../libphorward.a
	$(CC) -g -o $@ main.o ../libphorward.a

clean:
	rm -f pp
