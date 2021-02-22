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

assert 0 "0;"
assert 42 "42 ;"

assert 0 "1+2-3 ;"

assert 0 " 12 + 1 - 13 ;"

assert 5 "1+2*2 ;"
assert 6 "5/1 + 1 ;"
assert 5 "(0*5)+(1*5) ;"
assert 20 "((10*10)+(10*10))/10 ;"

assert 2 "-1+2-3+4 ;"
assert 0 "(-5)+(+5) ;"

assert 1 "1==1 ;"
assert 0 "1!=1 ;"
assert 1 "1<2 ;"
assert 1 "1<=2 ;"
assert 1 "2>1 ;"
assert 1 "2>=1 ;"
assert 1 "(1*1+5)/100 == 1*0 ;"

assert 3 "1; 2; 3;"
assert 1 "a = 1; a;"
assert 4 "a = 4; c = 100; a;"
assert 55 "a = d = 55; a;"
assert 100 "a = 4; d = 5; c = 100; d = c; d;"
assert 104 "a = 4; c = 100; a + c;"
assert 100 "a = 1; b = 2; c = 100; b = c; a = b; a;"

assert 2 "foo = 1; foo + foo ;"
assert 20 "var1 = 1; var2 = 10; var1 = var2; var1 + var1;"

assert 9 "return 9;"
assert 3 "return 1+1+1;"
assert 10 "var1 = 10 * 5; return 10;"

echo OK