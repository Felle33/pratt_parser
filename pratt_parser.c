#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "arena.h"
#include "tlex.h"
#include "optional.h"

typedef enum {
  NUM,
  POS,
  NEG,
  SUM,
  SUB,
  MUL,
  DIV,
  FAC,
  IDX,
  QMK,
  COL,
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

DEFINE_OPTION_TYPE(Pair, Option_pair)

Arena nodes_arena;

bool is_binary_operator(Token_type type) {
  return type == TOKEN_PLUS || type == TOKEN_MINUS || type == TOKEN_STAR
    || type == TOKEN_SLASH;
}

bool is_postfix_operator(Token_type type) {
  return type == TOKEN_BANG;
}

Pair prefix_binding_power(Token_type type) {
  switch(type) {
  case TOKEN_PLUS:
  case TOKEN_MINUS: return (Pair) {0, 7};
  default:
    fprintf(stderr, "The prefix binding power function should never reach the default case");
    exit(-1);
  }
}

Option_pair infix_binding_power(Token_type type) {
  switch(type) {
  case TOKEN_PLUS:
  case TOKEN_MINUS:         return (Option_pair) {{3, 4}, true};
  case TOKEN_STAR:
  case TOKEN_SLASH:         return (Option_pair) {{5, 6}, true};
  case TOKEN_QUESTION_MARK: return (Option_pair) {{2, 1}, true};
  default:                  return (Option_pair) {{0, 0}, false};
  }
}

Option_pair postfix_binding_power(Token_type type) {
  switch(type) {
  case TOKEN_BANG:
  case TOKEN_LEFT_SQUARE: return (Option_pair) {{9, 0}, true};
  default:                return (Option_pair) {{0, 0}, false};
  }
}

Parse_type parse_node_type(Token_type type) {
  switch(type) {
  case TOKEN_NUMBER:        return NUM;
  case TOKEN_PLUS:          return SUM;
  case TOKEN_MINUS:         return SUB;
  case TOKEN_STAR:          return MUL;
  case TOKEN_SLASH:         return DIV;
  case TOKEN_BANG:          return FAC;
  case TOKEN_LEFT_SQUARE:   return IDX;
  case TOKEN_QUESTION_MARK: return QMK;
  case TOKEN_COLUMN:        return COL;
  default:
    fprintf(stderr, "The parse node type function should never reach the default case");
    exit(-1);
  }
}

Node* expr_bp(Lexer *lexer, unsigned int min_bp) {
  Token token = next_token(lexer);
  Node* lhs;

  switch(token.type) {
  case TOKEN_NUMBER:
    lhs = arena_alloc(&nodes_arena, sizeof(Node));
    lhs->type = NUM;
    lhs->num = strtol(token.lexeme, NULL, 10);
    break;
  case TOKEN_PLUS:
    lhs = arena_alloc(&nodes_arena, sizeof(Node));
    lhs->type = POS;
    lhs->operand = expr_bp(lexer, prefix_binding_power(TOKEN_MINUS).s);
    break;
  case TOKEN_MINUS:
    lhs = arena_alloc(&nodes_arena, sizeof(Node));
    lhs->type = NEG;
    lhs->operand = expr_bp(lexer, prefix_binding_power(TOKEN_MINUS).s);
    break;
  case TOKEN_LEFT_PAR:
    lhs = expr_bp(lexer, 0);
    assert(next_token(lexer).type == TOKEN_RIGHT_PAR && "Expected a closed round parenthesis");
    break;
  default:
    fprintf(stderr, "Expected a number or a prefix operator, received a '%.*s'\n", (int)token.len, token.lexeme);
    exit(-1);
  }

  while(1) {
    token = peek_token(lexer);
    if (token.type == TOKEN_EOF) {
      break;
    }

    Token_type token_type = token.type;
    Option_pair opt;
    Pair bp;
    Node *p;

    // trying to parse as a postfix operator
    opt = postfix_binding_power(token_type);
    if(opt.has_value) {
      bp = opt.value;
      if (bp.f < min_bp) {
	break;
      }

      next_token(lexer);

      p = arena_alloc(&nodes_arena, sizeof(Node));
      if(token_type == TOKEN_LEFT_SQUARE) {
	Node *rhs = expr_bp(lexer, 0);
	assert(next_token(lexer).type == TOKEN_RIGHT_SQUARE && "Expected a ']'");
	p->type = parse_node_type(token_type);
	p->chldr.lhs = lhs;
	p->chldr.rhs = rhs;
      } else {
	p->type = parse_node_type(token_type);
	p->operand = lhs;
      }
      lhs = p;
      
      continue;
    }

    // trying to parse as an infix operator
    opt = infix_binding_power(token_type);
    if(opt.has_value) {
      bp = opt.value;
      if (bp.f < min_bp) {
	break;
      }

      next_token(lexer);
      p = arena_alloc(&nodes_arena, sizeof(Node));
	
      if(token.type == TOKEN_QUESTION_MARK) {
	Node *mhs = expr_bp(lexer, bp.s);
	assert(next_token(lexer).type == TOKEN_COLUMN && "Expected a ':'");
	Node *rhs = expr_bp(lexer, bp.s);
	Node *rhs_chld = arena_alloc(&nodes_arena, sizeof(Node));
	rhs_chld->type = parse_node_type(TOKEN_COLUMN);
	rhs_chld->chldr.lhs = mhs;
	rhs_chld->chldr.rhs = rhs;

	p->type = parse_node_type(token_type);
	p->chldr.lhs = lhs;
	p->chldr.rhs = rhs_chld;
      } else {
	Node *rhs = expr_bp(lexer, bp.s);
	p->type = parse_node_type(token_type);
	p->chldr.lhs = lhs;
	p->chldr.rhs = rhs;
      }

      lhs = p;

      continue;
    }
    
    break;
  }

  return lhs;
}

char parse_type_to_char(Parse_type type) {
  switch(type) {
  case POS:
  case SUM: return '+';
  case NEG:
  case SUB: return '-';
  case MUL: return '*';
  case DIV: return '/';
  case FAC: return '!';
  case IDX: return '[';
  case QMK: return '?';
  case COL: return ':';
  default:  return '?';
  }
}

void print_tree(const Node* node) {
  if(node->type == NUM) {
    printf("%ld", node->num);
    return;
  }

  printf("(%c ", parse_type_to_char(node->type));

  if(node->type == POS || node->type == NEG || node->type == FAC) {
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
  //  lexer_init(&lexer, "100[1 + 2 - 3][10 * 4 - 2!]");
  lexer_init(&lexer, "10 ? 5 + 2 : 3 * 6");

  Node* result = expr_bp(&lexer, 0);
  print_tree(result);
  printf("\n");

  arena_free(&nodes_arena);
  return 0;
}
