#include "9cc.h"

int main(int argc, char **argv)
// argc -> コマンドも含めた引数の個数
// **argv -> コマンドを文字列としたリスト
{
    // 引数が1個以外のときにエラーにする
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません");
        return 1;
    }

    user_input = argv[1];
    // ソースコードをトークナイズする
    token = tokenize(user_input);
    // トークンを抽象構文木に変換する
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 抽象構文木からスタックマシンを使ってバイナリコードを生成
    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}