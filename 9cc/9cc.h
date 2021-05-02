#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

// トークン型
typedef struct Token Token;
struct Token
{
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークンの文字列
    int len;
};

Token *tokenize();

// 抽象構文木のノードの種類
typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_GRT, // >
    ND_LOW, // <
    ND_EQU, // ==
    ND_NEQ, // !=
    ND_GRE, // >=
    ND_LOE, // <=
    ND_NUM, // 整数
} NodeKind;

// 抽象構文木ノード
typedef struct Node Node;
struct Node
{
    NodeKind kind; // ノードの型
    Node *lhs;     // 左子ノード
    Node *rhs;     // 右子ノード
    int val;       // kindがND_NUMのとき数値を格納する
};

Node *expr();

extern Token *token;
extern char *user_input;

void gen(Node *node);