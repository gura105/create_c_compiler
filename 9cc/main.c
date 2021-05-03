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
    extern Token *token;
    token = tokenize(user_input);

    // トークンを抽象構文木に変換する
    parse();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    extern LVar *locals;
    int offset;
    if (locals != NULL)
        offset = locals->offset;
    else
        offset = 0;

    // プロローグ
    // ローカル変数分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", offset);

    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++)
    {
        gen(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので、スタックが溢れないようにポップしておく
        printf("  pop rax\n");
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}