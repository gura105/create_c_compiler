#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
    TK_RETURN,   // リターン文
    TK_IF,       // IF文
    TK_ELSE,     // ELSE文
    TK_WHILE,    // WHILE文
    TK_FOR       // FOR文
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

// 外部からのtoken操作関数
void error(char *fmt, ...);
bool consume(char *op);
bool check_ident();
Token *consume_ident();
bool consume_keyword(char *op, TokenKind tk);
void expect(char *op);
int expect_number();
char expect_ident();
bool at_eof();

// ローカル変数の型
typedef struct LVar LVar;
struct LVar
{
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // RBPからのオフセット
};

// 抽象構文木のノードの種類
typedef enum
{
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_GRT,    // >
    ND_LOW,    // <
    ND_EQU,    // ==
    ND_NEQ,    // !=
    ND_GRE,    // >=
    ND_LOE,    // <=
    ND_ASSIGN, // =
    ND_LVAR,   // ローカル変数
    ND_NUM,    // 整数
    ND_RETURN, // return
    ND_IF,     // if
    ND_WHILE,  //while
    ND_FOR,    // for
    ND_BLOCK   // { }
} NodeKind;

// 抽象構文木ノード
typedef struct Node Node;
struct Node
{
    NodeKind kind; // ノードの型

    Node *lhs; // 左子ノード
    Node *rhs; // 右子ノード

    Node *cond; // if: 条件式
    Node *then; // if: 条件式が真のとき
    Node *els;  // if: 条件式が偽のとき

    Node *stmt[100]; // ブロックに含まれる式のリスト

    int val;    // kindがND_NUMのとき数値を格納する
    int offset; // kindがND_LVARの場合のみ使う
};

extern char *user_input;
extern Node *code[];

Token *tokenize();
void parse();

void gen(Node *node);
