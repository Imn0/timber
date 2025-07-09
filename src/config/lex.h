#ifndef TMB_CONFIG_LEX_H_
#define TMB_CONFIG_LEX_H_

#include <stddef.h>
#include <tmb_lib.h>

typedef enum {
    TOK_EOF,
    TOK_SECTION, // [section_name]
    TOK_IDENT,   // key_name
    TOK_STRING,  // 'quoted string'
    TOK_EQUALS,  // =
    TOK_NEWLINE, // \n
    TOK_ERROR
} TokenType;

static const char* const TokenTypeStr[] = {
    [TOK_EOF] = "TOK_EOF",       [TOK_SECTION] = "TOK_SECTION",
    [TOK_IDENT] = "TOK_IDENT",   [TOK_STRING] = "TOK_STRING",
    [TOK_EQUALS] = "TOK_EQUALS", [TOK_NEWLINE] = "TOK_NEWLINE",
    [TOK_ERROR] = "TOK_ERROR",
};

typedef struct {
    TokenType type;
    const char* data;
    size_t data_size;
    int line;
    int column;
} Token;

typedef struct {
    const char* input;
    size_t pos;
    size_t length;
    int line;
    int column;
    char current_char;
} Lexer;

void lexer_init(Lexer* lex, const char* input, size_t input_size);
void lexer_lex(Lexer* lex, Token* tok);
void token_print(Token* tok);

#endif //  TMB_CONFIG_LEX_H_
