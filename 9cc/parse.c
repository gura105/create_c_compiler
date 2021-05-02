#include "9cc.h"

// ツリーノードを新たに作成する
Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// ツリーノードを新たに作成する(数値用)
Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

// パーサーのプロトタイプ宣言
Node *expr(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

Node *parse()
{
    Node *node = expr();
}

// トークンを読み取って,抽象構文木を作成する
// expr表現を展開する
Node *expr()
{
    Node *node = equality();
    return node;
}

Node *equality()
{
    Node *node = relational();

    for (;;)
    {
        if (consume("=="))
            node = new_node(ND_EQU, node, relational());
        else if (consume("!="))
            node = new_node(ND_NEQ, node, relational());
        else
            return node;
    }
}

Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (consume(">="))
            node = new_node(ND_GRE, node, add());
        else if (consume(">"))
            node = new_node(ND_GRT, node, add());
        else if (consume("<="))
            node = new_node(ND_LOE, node, add());
        else if (consume("<"))
            node = new_node(ND_LOW, node, add());
        else
            return node;
    }
}

Node *add()
{
    // こちらを下記のif文より先に書いているため、左ノードを深さ優先で探索する
    Node *node = mul();

    // 無限ループ
    for (;;)
    {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

// トークンを読み取って,抽象構文木を作成する
// mul表現を展開する
Node *mul()
{
    Node *node = unary();

    for (;;)
    // 無限ループ
    {
        // 既存の親ノードを子ノードに入れ替えるテクニック
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary()
{
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

// トークンを読み取って,抽象構文木を作成する
// primary表現を展開する
Node *primary()
{
    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_node_num(expect_number());
}