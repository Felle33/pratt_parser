#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "tlex.h"

void lexer_init(Lexer *lexer, char *text) {
  lexer->start = text;
  lexer->cur = text; 
}

static Token make_token(Lexer *lexer, Token_type type) {
  Token token = {
    .lexeme = lexer->start,
    .len = lexer->cur - lexer->start,
    .type = type,
  };

  return token;
}

static Token error_token(char *str) {
  Token token = {
    .lexeme = str,
    // maybe should include also cur
    .len = strlen(str),
    .type = TOKEN_ERROR,
  };

  return token;
}

static char advance(Lexer *lexer) {
  char c = lexer->cur[0];
  lexer->cur++;
  return c;
}

static char peek(Lexer *lexer) {
  return lexer->cur[0];
}

//TODO remove the second condition
static bool is_at_end(Lexer *lexer) {
  return peek(lexer) == EOF || peek(lexer) == '\0';
}

static bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

static Token number(Lexer *lexer) {
  while(is_digit(peek(lexer)) && !is_at_end(lexer)) {
    advance(lexer);
  }

  return make_token(lexer, TOKEN_NUMBER);
}

void skip_whitespace(Lexer *lexer) {
  while(peek(lexer) == ' ') {
    advance(lexer);
  }
}

Token peek_token(Lexer *lexer) {
  lexer->cur = lexer->start;

  skip_whitespace(lexer);
  if (is_at_end(lexer)) return make_token(lexer, TOKEN_EOF);

  char c = advance(lexer);
  if (is_digit(c)) return number(lexer);

  switch (c) {
  case '(': return make_token(lexer, TOKEN_LEFT_PAR);
  case ')': return make_token(lexer, TOKEN_RIGHT_PAR);
  case '-': return make_token(lexer, TOKEN_MINUS);
  case '+': return make_token(lexer, TOKEN_PLUS);
  case '/': return make_token(lexer, TOKEN_SLASH);
  case '*': return make_token(lexer, TOKEN_STAR);
  }

  return error_token("Unexpected character.");
}

Token next_token(Lexer *lexer) {
  Token token = peek_token(lexer);
  lexer->start = lexer->cur;
  return token;
}
