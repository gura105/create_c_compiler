# Prototype C Compiler

## A simple compiler written in C

- Currently, the following code can be compiled into assembly code.

```
5*(9-6) <= 15;

a = 1; b = 2; a + b == 3;

clk = 0; cnt = 0; while(clk < 5){clk = clk + 1; if(clk == 2){cnt = cnt + 1;} else if(clk == 4){cnt = cnt + 1;}} return cnt;

cond = 1; if(cond == 0) return 0; else if(cond == 1) return 1; else return 2;
```

## How to use

`make`<br>
`./9cc '<statement you want to compile>' >> tmp.s`<br>
`gcc -o tmp tmp.s`<br>

## Reference

- [低レイヤを知りたい人のための C コンパイラ作成入門](https://www.sigbus.info/compilerbook)
