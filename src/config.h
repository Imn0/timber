#ifndef TMB_CONFIG_LEX_H_
#define TMB_CONFIG_LEX_H_

#include <stddef.h>
#include <tmb_lib.h>

typedef enum {
    TMB_TOK_EOF,
    TMB_TOK_SECTION, // [section_name]
    TMB_TOK_IDENT,   // key_name
    TMB_TOK_STRING,  // 'quoted string' | "quoted string"
    TMB_TOK_EQUALS,  // =
    TMB_TOK_NEWLINE, // \n
    TMB_TOK_ERROR
} tmb_cfg_tok_type;

static const char* const TokenTypeStr[] = {
    [TMB_TOK_EOF] = "TOK_EOF",       [TMB_TOK_SECTION] = "TOK_SECTION",
    [TMB_TOK_IDENT] = "TOK_IDENT",   [TMB_TOK_STRING] = "TOK_STRING",
    [TMB_TOK_EQUALS] = "TOK_EQUALS", [TMB_TOK_NEWLINE] = "TOK_NEWLINE",
    [TMB_TOK_ERROR] = "TOK_ERROR",
};

typedef struct tmb_cfg_tok {
    tmb_cfg_tok_type type;
    const char* items;
    size_t length;
    int line;
    int column;
} tmb_cfg_tok_t;

typedef struct tmb_cfg_lexer {
    _da_header_(tmb_cfg_tok_t*);
    int current_tok_idx;
} tmb_cfg_lexer_t;

void tmb_lex(tmb_cfg_lexer_t* lex, const char* input, size_t input_size);
#ifdef TMB_UNIX
bool tmb_lex_expect(tmb_cfg_lexer_t* lex,
                    int n,
                    tmb_cfg_tok_type toks[static n]);
#else
bool tmb_lex_expect(tmb_cfg_lexer_t* lex,
                    int n,
                    tmb_cfg_tok_type toks[]);
#endif
void tmb_lex_advance(tmb_cfg_lexer_t* lex);
tmb_cfg_tok_t tmb_lex_get(tmb_cfg_lexer_t* lex);
tmb_cfg_tok_t tmb_lex_get_and_advance(tmb_cfg_lexer_t* lex);
void tmb_token_print(tmb_cfg_tok_t* tok);

#endif //  TMB_CONFIG_LEX_H_
