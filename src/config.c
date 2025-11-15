#include <config.h>
#include <config_interlal.h>
#include <ctype.h>
#include <stdio.h>

typedef struct tmb_cfg_lexer_impl {
    const char* input;
    int pos;
    int length;
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
                       int length,
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
    int content_length     = 0;

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
    int length        = 0;

    while (lex->current_char &&
           (isalnum(lex->current_char) || lex->current_char == '_')) {
        advance(lex);
        length++;
    }

    if (length == 4 && strncmp(start, "true", 4) == 0) {
        token_init(tok, TMB_TOK_BOOL, start, length, start_line, start_column);
        tok->data.bool_val = true;
        return;
    }

    if (length == 5 && strncmp(start, "false", 5) == 0) {
        token_init(tok, TMB_TOK_BOOL, start, length, start_line, start_column);
        tok->data.bool_val = false;
        return;
    }

    token_init(tok, TMB_TOK_IDENT, start, length, start_line, start_column);
    return;
}

static void read_int(tmb_cfg_lexer_impl_t* lex, tmb_cfg_tok_t* tok) {
    int start_line    = lex->line;
    int start_column  = lex->column;
    const char* start = lex->input + lex->pos;
    int length        = 0;

    while (isdigit(lex->current_char)) {
        advance(lex);
        length++;
    }

    token_init(tok, TMB_TOK_INT, start, length, start_line, start_column);
    return;
}

static void skip_comment(tmb_cfg_lexer_impl_t* t) {
    advance(t);
    while (t->current_char && t->current_char == ' ') { advance(t); }
    while (t->current_char && t->current_char != '\n') { advance(t); }
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
            const char* start = lex->input + lex->pos;
            advance(lex);
            token_init(tok, TMB_TOK_SB_OPEN, start, 1, line, column);
            return;
        }

        if (lex->current_char == ']') {
            const char* start = lex->input + lex->pos;
            advance(lex);
            token_init(tok, TMB_TOK_SB_CLOSE, start, 1, line, column);
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

        if (isdigit(lex->current_char)) {
            read_int(lex, tok);
            return;
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

void tmb_lex(tmb_cfg_lexer_t* lex, const char* input, int input_size) {
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
bool tmb_lex_expect(tmb_cfg_lexer_t* lex, int n, tmb_cfg_tok_type toks[])
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
    printf("Token: %-12s ", token_type_str[token->type]);
    printf(" L: %3d, C: %3d", token->line, token->column);
    if (token->items || token->length > 0) {
        printf(" Value: ");
        print_token_value(token);
    }
    printf("\n");
}

static bool parse_global(tmb_cfg_lexer_t* lex, tmb_config_t* target_config) {
    tmb_string_view_t log_level_sv     = sv_make("log_level");
    tmb_string_view_t enable_colors_sv = sv_make("enable_colors");

    while (tmb_lex_expect(
            lex, 2, (tmb_cfg_tok_type[]) { TMB_TOK_IDENT, TMB_TOK_EQUALS })) {
        tmb_cfg_tok_t tok = tmb_lex_get_and_advance(lex);
        tmb_lex_advance(lex); // =
        tmb_string_view_t tok_sv = (tmb_string_view_t) { .items  = tok.items,
                                                         .length = tok.length };

        if (tmb_sv_cmp(&log_level_sv, &tok_sv)) {
            if (tmb_lex_expect(lex,
                               2,
                               (tmb_cfg_tok_type[]) { TMB_TOK_STRING,
                                                      TMB_TOK_NEWLINE })) {
                tmb_cfg_tok_t log_level_val_tok = tmb_lex_get_and_advance(lex);
                tmb_string_view_t log_level_val_sv = (tmb_string_view_t) {
                    .items  = log_level_val_tok.items,
                    .length = log_level_val_tok.length
                };
                tmb_lex_advance(lex);
                bool set = false;
                for (int i = 0; i < TMB_LOG_LEVEL_COUNT; i++) {
                    if (tmb_sv_cmp(
                                &log_level_val_sv,
                                &(tmb_string_view_t) {
                                        .items  = tmb_log_level_str[i],
                                        .length = tmb_log_level_str_len[i] })) {
                        tmb_cfg.log_level = (tmb_log_level)i;
                        set               = true;
                        break;
                    }
                }
                if (!set) {
                    // todo print error
                    return false;
                }
            } else {
                // todo print error
                return false;
            }
        }
        if (tmb_sv_cmp(&enable_colors_sv, &tok_sv)) {
            if (tmb_lex_expect(lex,
                               2,
                               (tmb_cfg_tok_type[]) { TMB_TOK_BOOL,
                                                      TMB_TOK_NEWLINE })) {
                tmb_cfg_tok_t enable_colors_tok = tmb_lex_get_and_advance(lex);
                if (enable_colors_tok.data.bool_val == true) {
                    tmb_cfg.enable_colors = true;
                } else {
                    tmb_cfg.enable_colors = false;
                }
                tmb_lex_advance(lex);

            } else {
                // todo print error
                return false;
            }
        }
    }
    UNUSED target_config;
    return true;
}

static bool parse_formats(tmb_cfg_lexer_t* lex, tmb_config_t* target_config) {
    while (tmb_lex_expect(lex,
                          4,
                          (tmb_cfg_tok_type[]) { TMB_TOK_IDENT,
                                                 TMB_TOK_EQUALS,
                                                 TMB_TOK_STRING,
                                                 TMB_TOK_NEWLINE })) {
        tmb_cfg_tok_t tok_ident = tmb_lex_get_and_advance(lex);
        tmb_lex_advance(lex);
        tmb_cfg_tok_t tok_string = tmb_lex_get_and_advance(lex);
        tmb_lex_advance(lex);

        tmb_string_view_t indent_sv = (tmb_string_view_t) {
            .items = tok_ident.items, .length = tok_ident.length
        };
        tmb_string_view_t string_sv = (tmb_string_view_t) {
            .items = tok_string.items, .length = tok_string.length
        };

        char* i = tmb_sv_to_ctst_copy(&indent_sv);
        char* s = tmb_sv_to_ctst_copy(&string_sv);
        hm_put(&target_config->formats, i, s);

        free(i);
    }
    return true;
}

static tmb_config_t* tmb_config_load_impl(
        const tmb_string_view_t* config_contents) {
    tmb_cfg_lexer_t lex = { 0 };

    tmb_lex(&lex, config_contents->items, config_contents->length);
    tmb_config_t* config = NULL;

    tmb_string_view_t formats_sv = sv_make("formats");
    tmb_string_view_t global_sv  = sv_make("global");

    config  = malloc(sizeof(*config));
    *config = (tmb_config_t) { 0 };

    while (tmb_lex_expect(&lex,
                          4,
                          (tmb_cfg_tok_type[]) { TMB_TOK_SB_OPEN,
                                                 TMB_TOK_IDENT,
                                                 TMB_TOK_SB_CLOSE,
                                                 TMB_TOK_NEWLINE })) {
        tmb_lex_advance(&lex); // [
        tmb_cfg_tok_t tok        = tmb_lex_get_and_advance(&lex);
        tmb_string_view_t tok_sv = (tmb_string_view_t) { .items  = tok.items,
                                                         .length = tok.length };
        tmb_lex_advance(&lex); // ]
        tmb_lex_advance(&lex); // \n

        if (tmb_sv_cmp(&formats_sv, &tok_sv)) { // ident
            if (!parse_formats(&lex, config)) goto fail;
        }

        if (tmb_sv_cmp(&global_sv, &tok_sv)) { // ident
            if (!parse_global(&lex, config)) goto fail;
        }
    }

    return config;
fail:
    hm_free(&config->formats);
    free(config);
    return NULL;
}

struct tmb_config* tmb_config_load(const char* filename) {
    struct tmb_config* r    = NULL;
    tmb_string_builder_t sb = { 0 };
    bool ok                 = load_entire_file(filename, &sb);
    if (!ok) { goto end; }
    sb_append(&sb, '\n');

    tmb_string_view_t sv = sv_from_sb(&sb);

    r = tmb_config_load_impl(&sv);
end:
    sb_free(&sb);
    return r;
}

struct tmb_config* tmb_config_from_string(const char* config) {
    struct tmb_config* r    = NULL;
    tmb_string_builder_t sb = { 0 };
    sb_append_cstr(&sb, config);
    sb_append(&sb, '\n');
    tmb_string_view_t sv = sv_from_sb(&sb);

    r = tmb_config_load_impl(&sv);
    sb_free(&sb);
    return r;
}

const char* tmb_config_get_format(struct tmb_config* config,
                                  const char* format_name) {
    if (config == NULL) { return NULL; }
    if (format_name == NULL) { return NULL; }

    if (hm_has(&config->formats, format_name)) {
        return hm_get(&config->formats, format_name);
    }
    return NULL;
}
