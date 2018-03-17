#!/bin/sh
set -ex
./configure --with-trace
make
sudo make install
pparse "Int := /[0-9]+/; factor : Int | '(' expr ')'; term : term '*' factor = mul | factor ; expr : expr '+' term = add | term ;" "1+2*3+4*5"
