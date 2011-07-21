#!/bin/sh

gcc -pedantic -Wall -Wno-variadic-macros -lm input.c expr.c parse.c \
	extra-math.c compile.c errors.c chain.c 1.test.c -o 1.test -dA -ggdb
