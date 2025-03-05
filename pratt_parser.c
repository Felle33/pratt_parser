#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "arena.h"
#include "tlex.h"

typedef enum {
  NUM,
  POS,
  NEG,
  SUM,
  MIN,
  MUL,
  DIV,
} Parse_type;

typedef struct Node Node;

typedef struct {
  Node *lhs;
  Node *rhs;
} Node_binop;

struct Node {
  Parse_type type;
  union {
    long num;
    Node_binop chldr;
    Node *operand;
  };
};

typedef struct {
  unsigned int f;
  unsigned int s;
} Pair;

Arena nodes_arena;

bool is_binary_operator(Token_type type) {
  return type == TOKEN_PLUS || type == TOKEN_MINUS || type == TOKEN_STAR
    || type == TOKEN_SLASH;
}

Pair prefix_binding_power(Token_type type) {
  switch(type) {
  case TOKEN_PLUS:
  case TOKEN_MINUS: return (Pair) {0, 5};
  default:
    fprintf(stderr, "The prefix binding power function should never reach the default case");
    exit(-1);
  }
}

Pair infix_binding_power(Token_type type) {
  switch(type) {
  case TOKEN_PLUS:
  case TOKEN_MINUS: return (Pair) {1, 2};
  case TOKEN_STAR:
  case TOKEN_SLASH: return (Pair) {3, 4};
  default:
    fprintf(stderr, "The infinx binding power function should never reach the default case");
    exit(-1);
  }
}

Parse_type parse_node_type(Token_type type) {
  switch(type) {
  case TOKEN_NUMBER: return NUM;
  case TOKEN_PLUS:   return SUM;
  case TOKEN_MINUS:  return MIN;
  case TOKEN_STAR:   return MUL;
  case TOKEN_SLASH:  return DIV;
  default:
    fprintf(stderr, "The parse node type function should never reach the default case");
    exit(-1);
  }
}

char parse_type_to_char(Parse_type type) {
  switch(type) {
  case POS:
  case SUM: return '+';
  case NEG:
  case MIN: return '-';
  case MUL: return '*';
  case DIV: return '/';
  default:  return '?';
  }
}

Node* expr_bp(Lexer *lexer, unsigned int min_bp) {
  Token token = next_token(lexer);
  Node* lhs = arena_alloc(&nodes_arena, sizeof(Node));

  switch(token.type) {
  case TOKEN_NUMBER:
    lhs->type = NUM;
    lhs->num = strtol(token.lexeme, NULL, 10);
    break;
  case TOKEN_PLUS:
    lhs->type = POS;
    lhs->operand = expr_bp(lexer, prefix_binding_power(TOKEN_MINUS).s);
    break;
  case TOKEN_MINUS:
    lhs->type = NEG;
    lhs->operand = expr_bp(lexer, prefix_binding_power(TOKEN_MINUS).s);
    break;
  default:
    fprintf(stderr, "Expected a number or a prefix operator, received a '%.*s'\n", (int)token.len, token.lexeme);
    exit(-1);
  }

  while(1) {
    token = peek_token(lexer);
    if (token.type == TOKEN_EOF) {
      break;
    } else if(!is_binary_operator(token.type)) {
      fprintf(stderr, "Expected a binary operator instead received a '%s'\n", token.lexeme);
      exit(-1);
    }

    Token_type token_type = token.type;
    Pair bp = infix_binding_power(token_type);

    if (bp.f < min_bp) break;
    next_token(lexer);

    Node *rhs = expr_bp(lexer, bp.s);
    Node *p = arena_alloc(&nodes_arena, sizeof(Node));
    p->type = parse_node_type(token_type);
    p->chldr.lhs = lhs;
    p->chldr.rhs = rhs;

    lhs = p;
  }

  return lhs;
}

void print_tree(const Node* node) {
  if(node->type == NUM) {
    printf("%ld", node->num);
    return;
  }

  printf("(%c ", parse_type_to_char(node->type));

  if(node->type == POS || node->type == NEG) {
    print_tree(node->operand);
    printf(")");
    return;
  }
  
  print_tree(node->chldr.lhs);
  printf(" ");
  print_tree(node->chldr.rhs);
  printf(")");
}

int main(void) {
  Lexer lexer;
  arena_init(&nodes_arena);
  lexer_init(&lexer, "-24 + 12 * 43 - 143 / 3");

  Node* result = expr_bp(&lexer, 0);
  print_tree(result);

  arena_free(&nodes_arena);
  return 0;
}
