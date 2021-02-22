#include "9cc.h"

//　ローカル変数の連結リスト
LVar *locals;

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node)); // Node一個分のサイズのメモリ確保
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

static Node *new_node_val() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  return node;
}

static LVar *new_lvar() {
  LVar *lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = token->str;
  lvar->offset = (locals == NULL)? 0 : locals->offset + 8; 
  lvar->len = token->len;
  locals = lvar;
  return lvar;
}

static LVar *find_lvar() {
  for (LVar *var = locals; var; var = var->next) {
    if (
      var->len == token->len && 
      !memcmp(token->str, var->name, var->len)
    ){
      return var;
    }
  }
  return NULL;
}

void *program();
static Node *statement();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

// program = statement*
void *program(Node *code[]) {
  int i = 0;
  while (token->kind != TK_EOF) {
    code[i++] = statement();
  }
  code[i] = NULL;
}

// statement = expr ";" | "return" expr ";"
static Node *statement() {
  Node *node;
  if (token->kind == TK_RETURN) {
    token = token->next;
    node = new_node(ND_RETURN, expr(), NULL);
  } else {
    node = expr();
  }

  if (!consume(";")) {
    error_at(token->str, token->str, "';'ではないトークンです.");
  }
  return node;
}

// expr = assign
static Node *expr() {
  return assign();
}

// assign = equality ("=" assign)?
static Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality() {
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
static Node *relational() {
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
static Node *add() {
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
static Node *mul() {
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
static Node *unary() {
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }
  return primary();
}

// primary = "(" expr ")" | ident | num
static Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  if (token->kind == TK_IDENT) {
    Node *node = new_node_val();
    LVar *lvar = find_lvar();
    if (!lvar) {
      lvar = new_lvar();
    }

    token = token->next;
    node->offset = lvar->offset;
    return node;
  }

  return new_node_num(expect_number());
}