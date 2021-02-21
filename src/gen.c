#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入式の左辺値が変数でありません");
  }
  if (node->offset == 0) {
    error("offsetに正しく代入されていません");
  }
  printf("  mov rax, rbp\n"); // rax = rbp
  printf("  sub rax, %d\n", node->offset); // rax(rsp) = rbp - offset 
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  mov rax, %d\n", node->val);
      printf("  push rax\n");
      return;
    case ND_LVAR:
      printf("  mov rax, [rbp-%d]\n", node->offset);
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen(node->rhs);
      printf("pop rdi\n");

      gen_lval(node->lhs);
      printf("  pop rax\n");
      
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
  }
  
  // 左右ノードに再帰的に遷移
  gen(node->lhs);
  gen(node->rhs);
  // 2つの引数をスタックから取り出す
  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_EQ:
      printf("  cmp rax, rdi\n"); // 比較結果をフラグレジスタにセット
      printf("  sete al\n"); // 比較結果をフラグレジスタからal(raxの下位1byte)にコピー
      printf("  movzx rax, al\n"); // バイトをワードにゼロ拡張して転送
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzx rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzx rax, al\n");
      break;
    case ND_LTE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzx rax, al\n");
      break;
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n"); // rax を rdxとraxの2倍に伸ばしてセット
      printf("  idiv rdi\n"); // rax, rdxを16バイトとみなして取り、商をrax, 余りをrdxにセットする
      break;
  }

  printf("  push rax\n");
}
