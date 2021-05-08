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

    // asseblyコードの生成
    code_gen();

    return 0;
}