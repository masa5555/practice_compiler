#include <ctype.h> // 文字の種類判定
#include <stdarg.h> // 可変長引数処理
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token の種類
typedef enum {
  TK_RESERVED, // 記号 = {+, -, *, /, (, ), ==, !=, <, >, <=, >=}
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
  int len; // tokenの長さ
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
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

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQ, // ==
  ND_NE, // != 
  ND_LT, // <
  ND_LTE, // <=
  ND_GT, // >
  ND_LGE, // >=
  ND_NUM, // 整数
} NodeKind;

// 抽象構文木のノードの型
typedef struct Node Node;
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs; // 左辺
  Node *rhs; // 右辺
  int val; // kindがND_NUMのときのみ使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node)); // Node一個分のサイズのメモリ確保
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// expr = equality
Node *expr() {
  return equality();
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  while(1) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}
// relational = add ('<' add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();
  
  while(1) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume("<=")) {
      node = new_node(ND_LTE, node, add());
    } else if (consume(">")) {
      node = new_node(ND_GT, add(), node);
    } else if (consume(">=")) {
      node = new_node(ND_LGE, add(), node);
    } else {
      return node;
    }
  }
}

// add = mul ('+' mul | '-' mul)*
Node *add() {
  Node *node = mul();

  while(1) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

// mul = unary ('*' unary | '/' unary)*
Node *mul() {
  Node *node = unary();

  while (1) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

// unary = ("+" | "-")? primary
Node *unary() {
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

// primary = "(" expr ")" | num
Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  return new_node_num(expect_number());
}

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