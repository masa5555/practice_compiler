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

assert 0 " 12 + 1 - 13 "

assert 5 "1+2*2"
assert 6 "5/1 + 1"
assert 5 "(0*5)+(1*5)"
assert 20 "((10*10)+(10*10))/10"

assert 2 "-1+2-3+4"
assert 0 "(-5)+(+5)"

assert 1 "1==1"
assert 0 "1!=1"
assert 1 "1<2"
assert 1 "1<=2"
assert 1 "2>1"
assert 1 "2>=1"
assert 1 "(1*1+5)/100 == 1*0"

echo OK