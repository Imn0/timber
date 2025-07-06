#include <config/lex.h>

void lexer_init(Lexer* lex, char* input, int input_size) {
    lex->input = input;
    lex->input_size = input_size;
    lex->current_pos = 0;
    lex->line = 1;
    lex->col = 1;
}

static inline char peek(const Lexer* lex) {
    return lex->input[lex->current_pos];
}

static void skip_blank(Lexer* lex) {
    while (true) {
        char current = peek(lex);
        if (current == '\n') {
            lex->line++;
            lex->col = 0;
        }
        if (current != ' ' && current != '\t') break;
        lex->current_pos++;
        lex->col++;
    }
}

/**
 * @brief advances lexer to next non whitespace char and returns it
 * 
 * @param lex
 * @return char
 */
static char advance(Lexer* lex) {
    char current = peek(lex);
    if (peek(lex) == '\0') return current;

    lex->current_pos++;
    lex->col++;
    skip_blank(lex);

    return peek(lex);
}

static void skip_until(Lexer* lex, char until_what) {
    while (peek(lex) && peek(lex) != until_what) { advance(lex); }
}

static Token parse_section(Lexer* lex) {

    char* section_start = &lex->input[lex->current_pos];
    int start_pos = lex->current_pos;
    const int start_col = lex->col;
    const int start_line = lex->line;
    skip_until(lex, ']');
    int size = lex->current_pos - start_pos - 1;
    advance(lex);

    return (Token) { .type = TOK_SECTION,
                     .value = section_start,
                     .value_size = size,
                     .col = start_col,
                     .line = start_line };
}

Token lexer_lex(Lexer* lex) {
    while (peek(lex) == '#') {
        skip_until(lex, '\n');
        advance(lex);
    }

    while (true) {
        char current_char = peek(lex);
        advance(lex);
        switch (current_char) {
        case '\0':
            return (Token) { .type = TOK_EOF,
                             .col = lex->col,
                             .line = lex->line };
        case '\r':
        case '\n':
            break;
        case '[':
            return parse_section(lex);
        default:
            return (Token) { .type = TOK_ERR,
                             .col = lex->col,
                             .line = lex->line };
        }
    }
}
