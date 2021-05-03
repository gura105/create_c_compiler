#include "9cc.h"

// プログラムの文字列
char *user_input;

// 現在注目しているトークン
Token *token;

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    // エラーが起きているトークンが何番目か,プログラム文字列の戦闘と現在のトークン文字の
    // 相対アドレスから求める(ポインタを引き算して何番目の文字か求める)
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを一つ読み進めて真を返す。
// それ以外の場合は偽を返す。
bool consume(char *op)
{
    // トークンの種類が記号でない、またはトークン文字列長がトークナイズ時と異なるとき、またはトークンの文字列が予測値と異なるとき
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len) != 0)
        return false;

    token = token->next;
    return true;
}

// 次のトークンが期待している値が識別子のときは、トークンを一つ読み進めて真を返す。
// それ以外のはときは偽を返す。
bool check_ident()
{
    if ('a' <= token->str[0] && token->str[0] <= 'z')
    {
        return true;
    }
    return false;
}

// 次のトークンが期待している記号のときには、トークンを一つ読み進める。
// それ以外のエラーを報告する。
void expect(char *op)
{
    // トークンの種類が記号でない、またはトークン文字列の最初の文字がopでないとき
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "'%c'ではありません", *op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

char expect_ident()
{
    if (token->kind != TK_IDENT)
        error_at(token->str, "識別子ではありません");
    char ident = token->str[0];
    token = token->next;
    return ident;
}

// トークンの次の要素が最終要素を指している時に真を返す
bool at_eof()
{
    return token->kind == TK_EOF;
}

// Returns true if c is valid as the first character of an identifier.
static bool is_ident1(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

// Returns true if c is valid as a non-first character of an identifier.
static bool is_ident2(char c)
{
    return is_ident1(c) || ('0' <= c && c <= '9');
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// 入力文字列(の先頭アドレス)pをトークナイズしてそれを返す
Token *tokenize()
{
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        // 空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (is_ident1(*p))
        {
            char *start = p;
            do
            {
                p++;
            } while (is_ident2(*p));
            cur = new_token(TK_IDENT, cur, start);
            cur->len = p - start;
            continue;
        }

        // 記号なら新たにトークンを作成してcurに接続する
        // 2文字のオペレータはstrncmpで比較する
        if (strncmp(p, "==", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, "==");
            cur->len = 2;
            p += 2;
            continue;
        }
        if (strncmp(p, "!=", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, "!=");
            cur->len = 2;
            p += 2;
            continue;
        }
        if (strncmp(p, ">=", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, ">=");
            cur->len = 2;
            p += 2;
            continue;
        }
        if (strncmp(p, "<=", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, "<=");
            cur->len = 2;
            p += 2;
            continue;
        }
        if (*p == '+' ||
            *p == '-' ||
            *p == '*' ||
            *p == '/' ||
            *p == '(' ||
            *p == ')' ||
            *p == '>' ||
            *p == '<' ||
            *p == '=' ||
            *p == ';')
        {
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        if (isdigit(*p))
        {
            // 新たなトークンの作成とカーソルの移動を同時に行っている
            cur = new_token(TK_NUM, cur, p);
            // 値を初期化している
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズ出来ません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}