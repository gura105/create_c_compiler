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
assert 6 'foo = 2; bar = 3; return foo * bar;'
assert 10 'ans = 10; return ans; return 5;'
assert 0 "ans = 1; if(ans == 1) return 0; return 1;"
assert 1 "ans = 0; if(ans == 1) return 0; return 1;"
assert 2 "ans = 0; if(ans == 0) ans = ans + 1; if(ans == 1) ans = ans + 1; return ans;"
assert 0 "cond = 0; if(cond == 0) return 0; else return 1;"
assert 1 "cond = 1; if(cond == 0) return 0; else return 1;"
assert 1 "cond = 1; if(cond == 0) return 0; else if(cond == 1) return 1; else return 2;"
assert 10 "mer = 2; cnt = 0; while(cnt <= 9) cnt = cnt + mer; return cnt;"
assert 1 "while(0==0) return 1; return 0;"
assert 6 "a = 0; while(a<=5){a = a + 1;} return a;"
assert 1 "if(1==1){return 1;} return 0;"
assert 0 "ans = 0; if(ans==1){} return ans;"
assert 0 "ans = 0; return ans;"
assert 0 "ans = 0; if(ans==1)0==0; return ans;"
assert 0 "ans = 0; if(ans==1){ans = 1;} return ans;"
assert 1 "ans = 0; if(ans==0){ans = 1;} return ans;"
assert 2 "clk = 0; cnt = 0; while(clk < 5){clk = clk + 1; if(clk == 2){cnt = cnt + 1;} else if(clk == 4){cnt = cnt + 1;}} return cnt;"


echo OK