#include "9cc.h"

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // tokenizeし、連結リストを作る
  user_input = argv[1];
  token = tokenize();
  Node *node = expr();
  
  // アセンブラのヘッダー部分
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 抽象構文木を降りながらコード生成
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}