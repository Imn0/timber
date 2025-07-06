#ifndef TMB_CONFIG_LEX_H_
#define TMB_CONFIG_LEX_H_

#include <stddef.h>
#include <tmb_lib.h>

typedef enum {
    TOK_IDENT,
    TOK_STRING,
    TOK_SECTION,
    TOK_EOF,
    TOK_EQUALS,
    TOK_ERR,

    TOK_COUNT,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int value_size;
    int line;
    int col;
} Token;

typedef struct {
    char* input;
    int input_size;
    int line;
    int col;
    int current_pos;
} Lexer;

void lexer_init(Lexer* lex, char* input, int input_size);
Token lexer_lex(Lexer* lex);

#endif //  TMB_CONFIG_LEX_H_
