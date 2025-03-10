#ifndef TLEX
#define TLEX
#include <stddef.h>

typedef enum {
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_POW,
  TOKEN_BANG,
  TOKEN_LEFT_PAR,
  TOKEN_RIGHT_PAR,
  TOKEN_LEFT_SQUARE,
  TOKEN_RIGHT_SQUARE,
  TOKEN_QUESTION_MARK,
  TOKEN_COLUMN,
  TOKEN_SEMICOLUMN,
  TOKEN_HASHTAG,
  TOKEN_NUMBER,
  TOKEN_EOF,
  TOKEN_ERROR,
} Token_type;

typedef struct {
  char* start;
  char* cur;
} Lexer;

typedef struct {
  char* lexeme;
  size_t len;
  Token_type type;
} Token;

void lexer_init(Lexer *lexer, char *text);
Token next_token(Lexer *lexer);
Token peek_token(Lexer *lexer);
void expect(Lexer *lexer, Token_type);

#endif // TLEX
