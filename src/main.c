#include "9cc.h"

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // tokenizeし、連結リストを作る
  token = tokenize(argv[1]);

  // 文ごとに抽象構文木をつくる
  Node *code[100];
  program(code);
  
  // アセンブラのヘッダー部分
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 変数26個分の領域を確保
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 26*8

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]); // 抽象構文木を降りながらコード生成   
    printf("  pop rax\n"); // 式の評価結果を取り出す
  }

  // 最後の式の結果がRAXにある
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}