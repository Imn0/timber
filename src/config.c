#include <config.h>
#include <ctype.h>
#include <stdio.h>

typedef struct tmb_cfg_lexer_impl {
    const char* input;
    size_t pos;
    size_t length;
    int line;
    int column;
    char current_char;
} tmb_cfg_lexer_impl_t;

static void advance(tmb_cfg_lexer_impl_t* t) {
    if (t->pos < t->length) {
        if (t->current_char == '\n') {
            t->line++;
            t->column = 1;
        } else {
            t->column++;
        }
        t->pos++;
        t->current_char = (char)((t->pos < t->length) ? t->input[t->pos]
                                                      : '\0');
    }
}

// skips non newline whitespace
static void skip_whitespace(tmb_cfg_lexer_impl_t* lex) {
    while (lex->current_char && isspace(lex->current_char) &&
           lex->current_char != '\n') {
        advance(lex);
    }
}

static void token_init(tmb_cfg_tok_t* tok,
                       tmb_cfg_tok_type type,
                       const char* start,
                       size_t length,
                       int line,
                       int column) {
    tok->type   = type;
    tok->items  = start;
    tok->length = length;
    tok->line   = line;
    tok->column = column;
}

static void read_string(tmb_cfg_lexer_impl_t* lex, tmb_cfg_tok_t* tok) {
    int start_line         = lex->line;
    int start_column       = lex->column;
    const char string_char = *(lex->input + lex->pos);
    const char* start      = lex->input + lex->pos + 1; // skip opening quote
    size_t content_length  = 0;

    // skip opening quote
    advance(lex);

    while (lex->current_char && lex->current_char != string_char) {
        if (lex->current_char == '\n') {
            // unterminated string
            token_init(tok,
                       TMB_TOK_ERROR,
                       lex->input + lex->pos,
                       1,
                       start_line,
                       start_column);
            return;
        } else {
            advance(lex);
            content_length++;
        }
    }

    if (lex->current_char != string_char) {
        token_init(tok,
                   TMB_TOK_ERROR,
                   lex->input + lex->pos,
                   1,
                   start_line,
                   start_column);
        return;
    }

    // skip closing quote
    advance(lex);

    token_init(tok,
               TMB_TOK_STRING,
               start,
               content_length,
               start_line,
               start_column);
    return;
}

static void read_identifier(tmb_cfg_lexer_impl_t* lex, tmb_cfg_tok_t* tok) {
    int start_line    = lex->line;
    int start_column  = lex->column;
    const char* start = lex->input + lex->pos;
    size_t length     = 0;

    while (lex->current_char &&
           (isalnum(lex->current_char) || lex->current_char == '_')) {
        advance(lex);
        length++;
    }

    token_init(tok, TMB_TOK_IDENT, start, length, start_line, start_column);
    return;
}

static void skip_comment(tmb_cfg_lexer_impl_t* t) {
    advance(t);
    while (t->current_char && t->current_char == ' ') { advance(t); }
    while (t->current_char && t->current_char != '\n') { advance(t); }
}

static void read_section(tmb_cfg_lexer_impl_t* t, tmb_cfg_tok_t* tok) {
    int start_line   = t->line;
    int start_column = t->column;

    // skip opening bracket
    advance(t);

    const char* start = t->input + t->pos;
    size_t length     = 0;

    while (t->current_char && t->current_char != ']') {
        if (t->current_char == '\n') {
            // unterminated section
            token_init(tok,
                       TMB_TOK_ERROR,
                       t->input + t->pos,
                       1,
                       start_line,
                       start_column);
            return;
        }
        advance(t);
        length++;
    }

    if (t->current_char != ']') {
        token_init(tok,
                   TMB_TOK_ERROR,
                   t->input + t->pos,
                   1,
                   start_line,
                   start_column);
        return;
    }

    // skip closing bracket
    advance(t);

    token_init(tok, TMB_TOK_SECTION, start, length, start_line, start_column);
    return;
}

static void lexer_lex(tmb_cfg_lexer_impl_t* lex, tmb_cfg_tok_t* tok) {
    while (lex->current_char) {
        int line   = lex->line;
        int column = lex->column;

        if (isspace(lex->current_char) && lex->current_char != '\n') {
            skip_whitespace(lex);
            continue;
        }

        if (lex->current_char == '\n') {
            const char* start = lex->input + lex->pos;
            advance(lex);
            token_init(tok, TMB_TOK_NEWLINE, start, 1, line, column);
            return;
        }

        if (lex->current_char == '[') {
            read_section(lex, tok);
            return;
        }

        if (lex->current_char == '\'' || lex->current_char == '"') {
            read_string(lex, tok);
            return;
        }

        if (lex->current_char == '=') {
            const char* start = lex->input + lex->pos;
            advance(lex);
            token_init(tok, TMB_TOK_EQUALS, start, 1, line, column);
            return;
        }

        if (lex->current_char == '#') {
            skip_comment(lex);
            continue;
        }

        if (isalpha(lex->current_char) || lex->current_char == '_') {
            read_identifier(lex, tok);
            return;
        }

        // unknown character
        const char* start = lex->input + lex->pos;
        advance(lex);
        token_init(tok, TMB_TOK_ERROR, start, 1, line, column);
        return;
    }

    token_init(tok, TMB_TOK_EOF, NULL, 0, lex->line, lex->column);
    return;
}

static void print_token_value(tmb_cfg_tok_t* token) {
    if (token->type == TMB_TOK_NEWLINE) {
        printf("'\\n'");
    } else if (token->items && token->length > 0) {
        printf("'%.*s'", (int)token->length, token->items);
    } else {
        printf("(empty)");
    }
}

void tmb_lex(tmb_cfg_lexer_t* lex, const char* input, size_t input_size) {
    tmb_cfg_lexer_impl_t lex_impl = {
        .input        = input,
        .length       = input_size,
        .pos          = 0,
        .line         = 1,
        .column       = 1,
        .current_char = (char)(lex_impl.length > 0 ? lex_impl.input[0] : '\0')
    };
    tmb_cfg_tok_t tok = { 0 };
    while (true) {
        lexer_lex(&lex_impl, &tok);
        da_append(lex, tok);
        if (tok.type == TMB_TOK_EOF) { break; }
    }
}

#ifndef TMB_WINDOWS_MSVC
bool tmb_lex_expect(tmb_cfg_lexer_t* lex,
                    int n,
                    tmb_cfg_tok_type toks[static n])
#else
bool tmb_lex_expect(tmb_cfg_lexer_t* lex,
                    int n,
                    tmb_cfg_tok_type toks[])
#endif
                     {
    if (lex->current_tok_idx + n > lex->length) { return false; }

    int j = 0;
    for (int i = lex->current_tok_idx; i < lex->length && j < n; (i++, j++)) {
        if (lex->items[i].type != toks[j]) { return false; }
    }
    return true;
}

void tmb_lex_advance(tmb_cfg_lexer_t* lex) {
    if (lex->current_tok_idx < lex->length) { lex->current_tok_idx++; }
}

tmb_cfg_tok_t tmb_lex_get(tmb_cfg_lexer_t* lex) {
    return lex->items[lex->current_tok_idx];
}

tmb_cfg_tok_t tmb_lex_get_and_advance(tmb_cfg_lexer_t* lex) {
    tmb_cfg_tok_t tok = tmb_lex_get(lex);
    tmb_lex_advance(lex);
    return tok;
}

void tmb_token_print(tmb_cfg_tok_t* token) {
    printf("Token: %-12s ", TokenTypeStr[token->type]);
    printf(" L: %3d, C: %3d", token->line, token->column);
    if (token->items || token->length > 0) {
        printf(" Value: ");
        print_token_value(token);
    }
    printf("\n");
}
