#include "9cc.h"

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
bool consume(char *op){
  if(
    token->kind != TK_RESERVED ||
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len)
  ){
    return false;
  }

  token = token->next;
  return true;
}

// 次のtokenが期待している記号ならば、tokenを１つ読み進めて、真を返す。
// それ以外ならば、エラーを報告する。
void expect(char *op){
  if(
    token->kind != TK_RESERVED ||
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len)
  ){
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

// 新しいtokenを作成してcursorにつなげる
static Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  Token *tok = calloc(len, sizeof(Token)); // 動的にメモリ確保し、値を０初期化
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
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

    if(
      memcmp(p, "==", strlen("==")) == 0 ||
      memcmp(p, "!=", strlen("!=")) == 0 ||
      memcmp(p, "<=", strlen("<=")) == 0 ||
      memcmp(p, ">=", strlen(">=")) == 0 
    ){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }  

    if(strchr("+-*/()<>", *p)){
      cur = new_token(TK_RESERVED, cur, p++, 1); //代入してからp進める
      continue;
    }

    if(isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p, "tokenizeできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}