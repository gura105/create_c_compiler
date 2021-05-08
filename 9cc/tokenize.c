#include "9cc.h"

// プログラムの文字列
char *user_input;
char *kwd_list[] = {"return", "if", "else", "while", "for", NULL};
char *reserved_list[] = {"==", "!=", ">=", "<=",
                         "+", "-", "*", "/",
                         "(", ")", ">", "<", "=", ";", "{", "}",
                         NULL};

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

// 次のトークンが識別子の場合、トークンを一つ進めてその識別子トークンを返す。
// それ以外はNULLを返す。
Token *consume_ident()
{
    if (token->kind != TK_IDENT)
    {
        return NULL;
    }
    Token *tok = token;
    token = token->next;
    return tok;
}

// 次のトークンが特定のキーワードの場合、トークンを一つ進めて真を返す。
// それ以外の場合は偽を返す。
bool consume_keyword(char *op, TokenKind tk)
{
    // トークンの種類が記号でない、またはトークン文字列長がトークナイズ時と異なるとき、またはトークンの文字列が予測値と異なるとき
    if (token->kind != tk ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len) != 0)
        return false;

    token = token->next;
    return true;
}

// 現在指しているトークンの次のトークンが期待している記号のときは真を返す
// それ以外は偽を返す
bool predict(char *op)
{
    Token *next_tk = token->next;
    if (next_tk == NULL)
        return false;
    if (next_tk->kind != TK_RESERVED ||
        strlen(op) != next_tk->len ||
        memcmp(next_tk->str, op, next_tk->len) != 0)
        return false;

    return true;
}

// 次のトークンが期待している値が識別子のときは、トークンを一つ読み進めて真を返す。
// それ以外のはときは偽を返す。
// TODO: 使われていないので削除すること
bool check_ident()
{
    if (token->kind == TK_IDENT)
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

int is_alnum(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

static bool is_kwd(char *op, char *kwd)
{
    return !strncmp(op, kwd, strlen(kwd)) && !is_alnum(op[strlen(kwd)]);
}

bool check_str(char *op, char *str)
{
    return strncmp(op, str, strlen(str)) == 0;
}

TokenKind kwd_map(char *op)
{
    if (check_str(op, "return"))
        return TK_RETURN;
    if (check_str(op, "if"))
        return TK_IF;
    if (check_str(op, "else"))
        return TK_ELSE;
    if (check_str(op, "while"))
        return TK_WHILE;
    if (check_str(op, "for"))
        return TK_FOR;
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

        // 数値をトークナイズ
        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        // keywordをトークナイズ
        int flg_kwd = 0;
        char **kwd_p = kwd_list;
        for (kwd_p; *kwd_p != NULL; kwd_p++)
        {
            if (is_kwd(p, *kwd_p))
            {
                cur = new_token(kwd_map(*kwd_p), cur, p);
                cur->len = strlen(*kwd_p);
                p += strlen(*kwd_p);
                flg_kwd = 1;
                break;
            }
        }
        // keywordのtokenize処理が終了したらwhile文を最初からやり直す
        if (flg_kwd)
            continue;

        // 識別子をトークナイズ
        // alnumのときは識別子として認識する
        if (is_ident1(*p))
        {
            char *start = p;
            while (is_ident2(*p))
            {
                p++;
            }
            cur = new_token(TK_IDENT, cur, start);
            cur->len = p - start;
            continue;
        }

        // keywordをtokenize
        // 入力文字列がkeywordと一致するかfor文で探索する
        int flg_reserved = 0;
        char **reseved_p = reserved_list;
        for (reseved_p; *reseved_p != NULL; reseved_p++)
        {
            if (check_str(p, *reseved_p))
            {
                cur = new_token(TK_RESERVED, cur, p);
                cur->len = strlen(*reseved_p);
                p += strlen(*reseved_p);
                flg_reserved = 1;
                break;
            }
        }
        // reservedのtokenize処理が終了したらwhile文を最初からやり直す
        if (flg_reserved)
            continue;

        error_at(p, "トークナイズ出来ません");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}