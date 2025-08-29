#include <chips.h>

#define MAKE_CHIP(_chip_fn, _data, _free_fn)                                   \
    (tmb_chip_t) {                                                             \
        .chip_data = _data, .chip_fn = _chip_fn, .free_fn = _free_fn           \
    }

// const val

struct const_chip_data {
    char* val;
    int val_len;
};

static void const_val(tmb_string_builder_t* sb,
                      const tmb_log_ext_ctx_t* ctx,
                      void* chip_data) {
    struct const_chip_data* data = chip_data;
    UNUSED ctx;
    sb_appendn(sb, data->val, data->val_len);
}

tmb_chip_t tmb_chip_const_val_make(const char* val, int val_len) {
    struct const_chip_data* data = malloc(sizeof(*data));
    data->val                    = malloc((size_t)val_len * sizeof(char));
    memcpy(data->val, val, (size_t)val_len);
    data->val_len = val_len;

    return MAKE_CHIP(const_val, data, free);
}

// message chip

static void message(tmb_string_builder_t* sb,
                    const tmb_log_ext_ctx_t* ctx,
                    void* chip_data) {
    UNUSED chip_data;
    sb_appendn(sb, ctx->message, ctx->message_len);
}
tmb_chip_t tmb_chip_message_make() {
    return MAKE_CHIP(message, NULL, NULL);
}

// level short chip

static void level_short(tmb_string_builder_t* sb,
                        const tmb_log_ext_ctx_t* ctx,
                        void* chip_data) {
    UNUSED chip_data;
    sb_append(sb, tmb_log_level_char[ctx->log_level]);
}
tmb_chip_t tmb_chip_log_level_short_make() {
    return MAKE_CHIP(level_short, NULL, NULL);
}

// level long chip

static void level_long(tmb_string_builder_t* sb,
                       const tmb_log_ext_ctx_t* ctx,
                       void* chip_data) {
    UNUSED chip_data;
    sb_appendn(sb,
               tmb_log_level_str[ctx->log_level],
               tmb_log_level_str_len[ctx->log_level]);
}
tmb_chip_t tmb_chip_log_level_long_make() {
    return MAKE_CHIP(level_long, NULL, NULL);
}

static void line_number(tmb_string_builder_t* sb,
                        const tmb_log_ext_ctx_t* ctx,
                        void* chip_data) {
    UNUSED chip_data;
    int line_number = ctx->line_no;
    while (line_number > 0) {
        sb_append(sb, (char)((line_number % 10) + '0'));
        line_number /= 10;
    }
}
tmb_chip_t tmb_chip_line_number_make() {
    return MAKE_CHIP(line_number, NULL, NULL);
}

static void filename(tmb_string_builder_t* sb,
                     const tmb_log_ext_ctx_t* ctx,
                     void* chip_data) {
    UNUSED chip_data;
    sb_appendn(sb, ctx->filename, ctx->filename_len);
}
tmb_chip_t tmb_chip_filename_make() {
    return MAKE_CHIP(filename, NULL, NULL);
}

static void base_filename(tmb_string_builder_t* sb,
                          const tmb_log_ext_ctx_t* ctx,
                          void* chip_data) {
    UNUSED chip_data;
    sb_appendn(sb, ctx->filename_base, ctx->filename_base_len);
}
tmb_chip_t tmb_chip_base_filename_make() {
    return MAKE_CHIP(base_filename, NULL, NULL);
}

static void function_name(tmb_string_builder_t* sb,
                          const tmb_log_ext_ctx_t* ctx,
                          void* chip_data) {
    UNUSED chip_data;
    sb_appendn(sb, ctx->funcname, ctx->filename_len);
}
tmb_chip_t tmb_chip_function_name_make() {
    return MAKE_CHIP(function_name, NULL, NULL);
}
