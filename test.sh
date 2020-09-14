#!/bin/bash

assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > asm.s
	cc -o exe asm.s
	./exe
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

assert 0 0
assert 42 42

assert 0 "1+2-3"

echo OK