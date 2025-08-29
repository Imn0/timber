#include <config.h>
#include <ctype.h>
#include <stdio.h>

void lexer_init(Lexer* lex, const char* input, size_t input_size) {
    lex->input        = input;
    lex->length       = input_size;
    lex->pos          = 0;
    lex->line         = 1;
    lex->column       = 1;
    lex->current_char = (char)(lex->length > 0 ? lex->input[0] : '\0');
}

static void advance(Lexer* t) {
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
static void skip_whitespace(Lexer* lex) {
    while (lex->current_char && isspace(lex->current_char) &&
           lex->current_char != '\n') {
        advance(lex);
    }
}

static void token_init(Token* tok,
                       TokenType type,
                       const char* start,
                       size_t length,
                       int line,
                       int column) {
    tok->type      = type;
    tok->data      = start;
    tok->data_size = length;
    tok->line      = line;
    tok->column    = column;
}

static void read_string(Lexer* lex, Token* tok) {
    int start_line        = lex->line;
    int start_column      = lex->column;
    const char* start     = lex->input + lex->pos + 1; // skip opening quote
    size_t content_length = 0;

    // skip opening quote
    advance(lex);

    while (lex->current_char && lex->current_char != '\'') {
        if (lex->current_char == '\n') {
            // unterminated string
            token_init(tok,
                       TOK_ERROR,
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

    if (lex->current_char != '\'') {
        token_init(tok,
                   TOK_ERROR,
                   lex->input + lex->pos,
                   1,
                   start_line,
                   start_column);
        return;
    }

    // skip closing quote
    advance(lex);

    token_init(
            tok, TOK_STRING, start, content_length, start_line, start_column);
    return;
}

static void read_identifier(Lexer* lex, Token* tok) {
    int start_line    = lex->line;
    int start_column  = lex->column;
    const char* start = lex->input + lex->pos;
    size_t length     = 0;

    while (lex->current_char &&
           (isalnum(lex->current_char) || lex->current_char == '_')) {
        advance(lex);
        length++;
    }

    token_init(tok, TOK_IDENT, start, length, start_line, start_column);
    return;
}

static void skip_comment(Lexer* t) {
    advance(t);
    while (t->current_char && t->current_char == ' ') { advance(t); }
    while (t->current_char && t->current_char != '\n') { advance(t); }
}

static void read_section(Lexer* t, Token* tok) {
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
                       TOK_ERROR,
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
        token_init(
                tok, TOK_ERROR, t->input + t->pos, 1, start_line, start_column);
        return;
    }

    // skip closing bracket
    advance(t);

    token_init(tok, TOK_SECTION, start, length, start_line, start_column);
    return;
}

void lexer_lex(Lexer* lex, Token* tok) {
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
            token_init(tok, TOK_NEWLINE, start, 1, line, column);
            return;
        }

        if (lex->current_char == '[') {
            read_section(lex, tok);
            return;
        }

        if (lex->current_char == '\'') {
            read_string(lex, tok);
            return;
        }

        if (lex->current_char == '=') {
            const char* start = lex->input + lex->pos;
            advance(lex);
            token_init(tok, TOK_EQUALS, start, 1, line, column);
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
        token_init(tok, TOK_ERROR, start, 1, line, column);
        return;
    }

    token_init(tok, TOK_EOF, NULL, 0, lex->line, lex->column);
    return;
}

static void print_token_value(Token* token) {
    if (token->type == TOK_NEWLINE) {
        printf("'\\n'");
    } else if (token->data && token->data_size > 0) {
        printf("'%.*s'", (int)token->data_size, token->data);
    } else {
        printf("(empty)");
    }
}

void token_print(Token* token) {
    printf("Token: %-12s ", TokenTypeStr[token->type]);
    printf(" L: %3d, C: %3d", token->line, token->column);
    if (token->data || token->data_size > 0) {
        printf(" Value: ");
        print_token_value(token);
    }
    printf("\n");
}
