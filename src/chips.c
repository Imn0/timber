#include <chips.h>

#define MAKE_CHIP(_chip_fn, _data, _free_fn)                                   \
    (tmb_chip_t) {                                                             \
        .chip_data = _data, .chip_fn = _chip_fn, .free_fn = _free_fn           \
    }

// const val

struct const_chip_data {
    char* val;
    size_t val_len;
};

static void const_val(tmb_string_builder_t* sb,
                      const tmb_log_ext_ctx_t* ctx,
                      void* chip_data) {
    struct const_chip_data* data = chip_data;
    UNUSED ctx;
    sb_appendn(sb, data->val, data->val_len);
}

tmb_chip_t tmb_chip_const_val_make(const char* val, size_t val_len) {
    struct const_chip_data* data = malloc(sizeof(*data));
    data->val                    = malloc(val_len * sizeof(char));
    memcpy(data->val, val, val_len);
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
