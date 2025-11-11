#include <config_interlal.h>
#include <string.h>

int main() {
    tmb_cfg_lexer_t lex = { 0 };
    const char* cfg     = "[loggers]\n"
                          "hello = \"aaa%d%$\"\n"
                          "a = 3123\n"
                          "a = oihwrg\n"
                          "a = true\n";
    tmb_lex(&lex, cfg, (int)strlen(cfg));

    for (int i = 0; i < lex.length; i++) { tmb_token_print(&lex.items[i]); }
    if (tmb_lex_expect(
                &lex,
                4,
                (tmb_cfg_tok_type[]) { TMB_TOK_SB_OPEN,
                                       TMB_TOK_IDENT,
                                       TMB_TOK_SB_CLOSE,
                                       TMB_TOK_NEWLINE })) { // "[loggers]\n"
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
    } else {
        ASSERT(0);
    }

    if (tmb_lex_expect(&lex,
                       4,
                       (tmb_cfg_tok_type[]) {
                               TMB_TOK_IDENT,
                               TMB_TOK_EQUALS,
                               TMB_TOK_STRING,
                               TMB_TOK_NEWLINE })) { // "hello = \"aaa%d%$\"\n"
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
    } else {
        ASSERT(0);
    }

    if (tmb_lex_expect(
                &lex,
                4,
                (tmb_cfg_tok_type[]) { TMB_TOK_IDENT,
                                       TMB_TOK_EQUALS,
                                       TMB_TOK_INT,
                                       TMB_TOK_NEWLINE })) { // "a = 3123\n"
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
        tmb_lex_advance(&lex);
    } else {
        ASSERT(0);
    }
}
