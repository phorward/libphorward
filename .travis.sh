#!/bin/sh
set -ex
./configure
make
sudo make install
pparse "@int /[0-9]+/; f: int | '(' e ')'; t: @mul( t '*' f ) | f; e: @add( e '+' t ) | t;" "1+2*(3+4)+5"
