#include <config.h>
#include <string.h>

int main() {
    tmb_cfg_lexer_t lex = { 0 };
    const char* cfg     = "[loggers]\nhello = \"aaa%d%$\"";
    tmb_lex(&lex, cfg, strlen(cfg));

    for (int i = 0; i < lex.length; i++) { tmb_token_print(&lex.items[i]); }
    if (tmb_lex_expect(
                &lex,
                2,
                (tmb_cfg_tok_type[]) { TMB_TOK_SECTION, TMB_TOK_NEWLINE })) {
        tmb_cfg_tok_t tok = tmb_lex_get_and_advance(&lex);
        tmb_token_print(&tok);
    } else {
        ASSERT(0);
    }
}
