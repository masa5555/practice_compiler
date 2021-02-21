#include "9cc.h"

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
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
