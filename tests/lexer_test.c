#include <assert.h>
#include <config/lex.h>

int main(void) {
    Lexer lex = { 0 };
    const char in[] = "#w\n"
                      "#j\n"
                      "   # dawd\n"
                      "#a\n"
                      "[hello]";
    lexer_init(&lex, in, strlen(in));

    // ignore all comments
    // after file ends just return eof
    Token tok = lexer_lex(&lex);
    assert(tok.type == TOK_SECTION);
    tok = lexer_lex(&lex);
    assert(tok.type == TOK_EOF);
    assert(lexer_lex(&lex).type == TOK_EOF);
    tok = lexer_lex(&lex);
    assert(tok.line == 5);
    assert(tok.col == 8);
}
