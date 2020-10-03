#include <ctype.h> // 文字の種類判定
#include <stdarg.h> // 可変長引数処理
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token の種類
typedef enum {
  TK_RESERVED, // 記号 = {+, -}
  TK_NUM, // 整数token
  TK_EOF, // 入力の終わりを表すtoken
} TokenKind;

typedef struct Token Token;

// token型
struct Token {
  TokenKind kind; // tokenの型
  Token *next; // 次の入力token
  int val; // kind が TK_NUM の時、　その数値
  char *str; // token 文字列 
};

// 現在着目しているtoken
Token *token;

char *user_input;

// エラー報告のための関数    
// printfと同じ引数をとる
// "..."は可変長引数
void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を表示する
void error_at(char *loc, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  // 変更部分
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); //空白で列合わせる
  fprintf(stderr, "^ "); 

  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のtokenが期待している記号ならば、tokenを１つ読み進めて、真を返す。
// それ以外ならば、偽を返す。
bool consume(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op){
    return false;
  }

  token = token->next;
  return true;
}

// 次のtokenが期待している記号ならば、tokenを１つ読み進めて、真を返す。
// それ以外ならば、エラーを報告する。
void expect(char op){
  if(token->kind != TK_RESERVED || token->str[0] != op){
    error_at(token->str, "'%c'ではありません", op);
  }
  token = token->next;
}

// 次のtokenが数値ならば、tokenを１つ読み進めてその数値を返す。
// それ以外ならば、エラーを報告する。
int expect_number(){
  if(token->kind != TK_NUM){
    error_at(token->str, "数値ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

// 新しいtokenを作成してcursorにつなげる
Token *new_token(TokenKind kind, Token *cur, char *str){
  Token *tok = calloc(1, sizeof(Token)); // 動的にメモリ確保し、値を０初期化
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをtokenizeしたものを返す
Token *tokenize(){
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p){
    // 空白文字　スキップ
    if(isspace(*p)){
      p++;
      continue;
    }

    if(*p == '+' || *p == '-'){
      cur = new_token(TK_RESERVED, cur, p++); //代入してからp進める
      continue;
    }

    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "tokenizeできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // tokenizeし、連結リストを作る
  user_input = argv[1];
  token = tokenize();
  
  // アセンブラのヘッダー部分
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  printf("main:\n");
  // 式の最初は数字を確認
  printf("  mov rax, %d\n", expect_number());

  // `+ <数>` or `- <数>`というtokenの並びを消費する
  while(!at_eof()){
    if(consume('+')){
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}