#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];
  
  // アセンブラのヘッダー部分
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  printf("main:\n");
  // long strtol(const char *s, char **endptr, int base);
  // 読み込んだ最後の文字の次の数字を指す
  printf("  mov rax, %ld\n", strtol(p, &p, 10));

  while(*p){
    // 1 ループで 2 回インクリメント
    if(*p == '+'){
      p++;
      printf("  add rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    if(*p == '-'){
      p++;
      printf("  sub rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "予期しない文字です: '%c'\n", *p);
    return 1;
  }

  printf("  ret\n");

  return 0;
}