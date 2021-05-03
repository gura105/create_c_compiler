#include "9cc.h"

LVar *locals;

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

LVar *new_lvar(Token *tok)
{
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;
    // 最初のローカル変数登録のときはlocalsはnull
    if (locals == NULL)
        lvar->offset = 8;
    else
        lvar->offset = locals->offset + 8;
    lvar->next = locals;
    locals = lvar;
    return lvar;
}

LVar *find_lvar(Token *tok)
{
    for (LVar *var = locals; var; var = var->next)
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) //memcmpは真のときの戻り地が0のため!で否定している
            return var;
    return NULL;
}

Node *new_node_ident(Token *tok)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    // offsetはlvarチェーンに同名の変数が存在するかで変わる
    LVar *lvar = find_lvar(tok);
    if (lvar)
    {
        node->offset = lvar->offset;
    }
    else
    {
        lvar = new_lvar(tok);
        node->offset = lvar->offset;
    }
    return node;
}

Node *code[100];

// パーサーのプロトタイプ宣言
void program();
Node *stmt();
Node *expr(void);
Node *assign();
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

void parse()
{
    program();
}

// トークンを読み取って,抽象構文木を作成する
void program()
{
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt()
{
    Node *node = expr();
    expect(";");
    return node;
}

Node *expr()
{
    return assign();
}

Node *assign()
{
    Node *node = equality();
    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
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

    Token *tok = consume_ident();
    if (tok)
    {
        return new_node_ident(tok);
    }

    return new_node_num(expect_number());
}