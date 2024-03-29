#include "9cc.h"

static int for_label_cnt = 0;
static int while_label_cnt = 0;
static int else_label_cnt = 0;

void gen_lval(Node *node)
{
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node)
{
    Node **p = node->stmt;
    char *func_name = NULL;

    // 数値ノードは終端ノードであるのでpushを行いreturnする
    switch (node->kind)
    {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_FOR:
        gen(node->init);
        printf(".Lforstart%d:\n", for_label_cnt);

        gen(node->end);
        printf("  pop rax\n");                      // for文終了判定式の結果をraxに格納
        printf("  cmp rax, 0\n");                   // for文終了判定式の審議を判定(真: 0, 偽: 1)
        printf("  push rax\n");                     // for文の評価結果を再びstackトップに戻す
        printf("  je .Lforend%d\n", for_label_cnt); // (評価値) == 1なら.Lendxxxラベルにジャンブ

        gen(node->loop);
        gen(node->inc);
        printf("  jmp .Lforstart%d\n", for_label_cnt);

        printf(".Lforend%d:\n", for_label_cnt);
        for_label_cnt++;
        return;
    case ND_IF:
        gen(node->cond);
        printf("  pop rax\n");                     // if文の結果をraxに格納
        printf("  cmp rax, 0\n");                  // if文の評価値の審議を判定(真: 0, 偽: 1)
        printf("  push rax\n");                    // if文の評価結果を再びstackトップに戻す
        printf("  je .Lelse%d\n", else_label_cnt); // (評価値) == 1なら.Lendxxxラベルにジャンブ
        gen(node->then);
        printf(".Lelse%d:\n", else_label_cnt);
        else_label_cnt++;
        if (node->els)
            gen(node->els);
        return;
    case ND_WHILE:
        printf(".Llpstart%d:\n", while_label_cnt);
        gen(node->lhs);                              // condの結果をpush
        printf("  pop rax\n");                       // if文の結果をraxに格納(condの結果をpop)
        printf("  cmp rax, 0\n");                    // if文の評価値の審議を判定(真: 0, 偽: 1)
        printf("  push rax\n");                      // condの結果をstackトップに戻す
        printf("  je .Llpend%d\n", while_label_cnt); // (評価値) == 1なら.Llpendxxxラベルにジャンブ
        gen(node->rhs);                              // stmtの結果をpush
        printf("  jmp .Llpstart%d\n", while_label_cnt);
        printf(".Llpend%d:\n", while_label_cnt);
        while_label_cnt++;
        return;
    case ND_BLOCK:
        for (p; *p != NULL; p++)
        {
            printf("  pop rax\n");
            gen(*p);
        }
        return;
    case ND_FUNC:
        func_name = slice_str(
            node->tok->str, 0, node->tok->len - 1);
        printf("  pop rax\n");
        printf("  call %s\n", func_name);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;
    case ND_EQU: // ==
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NEQ: // !=
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_GRT: // >
        printf("    cmp rdi, rax\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_GRE: // >=
        printf("    cmp rdi, rax\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LOW: // <
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LOE: // <=
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }
    printf("    push rax\n");
}

extern Node *code[];
extern LVar *locals;

void code_gen(void)
{
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // プロローグ
    // ローカル変数分の領域を確保する
    int offset;
    if (locals != NULL)
        offset = locals->offset;
    else
        offset = 0;
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
}