#!/bin/sh
set -ex
./configure --with-trace
make
sudo make install

# Run test suite
hash -r
LD_LIBRARY_PATH="/usr/lib" LDLIBS="-lphorward" CFLAGS="-DDEBUG -DUTF8 -DUNICODE -g" ptest -v src/base/array.c src/base/list.c src/string/string.c
