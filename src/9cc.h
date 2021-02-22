#include <ctype.h> // 文字の種類判定
#include <stdarg.h> // 可変長引数処理
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize

// token の種類
typedef enum {
  TK_RESERVED, // 記号 = {+, -, *, /, (, ), ==, !=, <, >, <=, >=, =}
  TK_IDENT, // 識別子
  TK_NUM, // 整数token
  TK_EOF, // 入力の終わりを表すtoken
} TokenKind;

// token型
typedef struct Token Token;
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

typedef struct LVar LVar;
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;
  int offset; // RBPからのオフセット
};

//　ローカル変数の連結リスト
LVar *locals;

// エラー表示用
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// parse.cでも使う
bool consume(char *op);
void expect(char *op);
int expect_number();

Token *tokenize();

// parse
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
  ND_ASSIGN, // =
  ND_LVAR, // local variable
} NodeKind;

// 抽象構文木のノードの型
typedef struct Node Node;
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs; // 左辺
  Node *rhs; // 右辺
  int val; // kindがND_NUMのときのみ使う
  int offset; // kindがND_LVARのときのみ使う
};

Node *code[100];
void *program();

// gen
void gen(Node *node);