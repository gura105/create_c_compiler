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

assert 0 0
assert 42 42
assert 23 '12+15-4'
assert 41 " 12 + 34 - 5"
assert 47 '5+6*7'
assert 3 '2 * 6 / 4'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 10 '-10+20'
assert 1 '42==42'
assert 0 '41==42'
assert 1 '41!=42'
assert 1 '(42==42)==1'
assert 1 '5*(9-6)==15'
assert 1 ' 2 < 3 '
assert 0 ' 5*(9-6) < 14'
assert 1 ' 5*(9-6) <= 15'
assert 1 ' 4 <= 5'


echo OK