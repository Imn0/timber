#include <assert.h>
#include <config/lex.h>
#include <string.h>

int main(void) {
    Lexer lex = { 0 };
    const char in[] = "#w\n"
                      "#j\n"
                      "   # dawd\n"
                      "#a\n"
                      "[hello]";
    lexer_init(&lex, in, strlen(in));

    Token tok = { 0 };
    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_NEWLINE);

    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_NEWLINE);

    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_NEWLINE);

    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_NEWLINE);

    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_SECTION);
    assert(strncmp(tok.data, "hello", tok.data_size) == 0);

    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_EOF);
    lexer_lex(&lex, &tok);
    assert(tok.type == TOK_EOF);
    lexer_lex(&lex, &tok);
    assert(tok.line == 5);
    assert(tok.column == 8);
}
