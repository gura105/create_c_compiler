#!/bin/bash
assert()
{
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 '0;'
assert 42 '42;'
assert 23 '12+15-4;'
assert 41 " 12 + 34 - 5;"
assert 47 '5+6*7;'
assert 3 '2 * 6 / 4;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 1 '42==42;'
assert 0 '41==42;'
assert 1 '41!=42;'
assert 1 '(42==42)==1;'
assert 1 '5*(9-6)==15;'
assert 1 ' 2 < 3 ;'
assert 0 ' 5*(9-6) < 14;'
assert 1 ' 5*(9-6) <= 15;'
assert 1 ' 4 <= 5;'
assert 1 ' 5 > 2;'
assert 0 ' (3 + 5) / 2 > 6 ;'
assert 1 ' (3 + 5) / 2 >= 4 ;'
assert 0 ' 5 >= 6;'
assert 5 ' x = 5; '
assert 1 'a = 1; b = 2; a + b == 3;'
assert 0 'a = 1; b = 2; a > b;'
assert 1 'foo = 1; fee = 2; foo + fee == 3;'
assert 1 'Zoo = 1; Xar = 2; Zoo + Xar == 3;'
assert 1 '_foo = 1; _foo = 2; _foo == 2;'
assert 1 'a=1; b=1; c=1; d=1; e=1; f=1; g=1; h=1; i=1; j=1; k=1; l=1; m=1; n=1; o=1; p=1; q=1; c=r; s=1; t=1; u=1; v=1; w=1; x=1; y=1; z=1; aa=1;'


echo OK