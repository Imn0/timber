#include <config.h>
#include <string.h>

int main() {
    Lexer lex       = { 0 };
    const char* cfg = "[loggers]\nhello = \"fiwsrhoighio%$\"";
    lexer_init(&lex, cfg, strlen(cfg));

    Token tok = {0};
    while (true) {
        lexer_lex(&lex, &tok);
        if(tok.type == TOK_EOF){
            break;
        }
        else {
            token_print(&tok);
        }
    }
}