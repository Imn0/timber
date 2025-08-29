#ifndef TMB_CHIPS_H_
#define TMB_CHIPS_H_
#include <tmb_internal.h>

typedef void tmb_chip_fn_t(tmb_string_builder_t* sb,
                           const tmb_log_ext_ctx_t* ctx,
                           void* chip_data);

typedef struct tmb_chip {
    void* chip_data;
    tmb_free_fn_t* free_fn;
    tmb_chip_fn_t* chip_fn;
    int just_amount;
    enum tmb_sb_just_opt just_opt;
    enum tmb_sb_truncate_opt truncate_opt;
} tmb_chip_t;

tmb_chip_t tmb_chip_const_val_make(const char* val, int val_len);
#define TMB_CHIP_CONST(val, val_len) tmb_chip_const_val_make(val, val_len)

tmb_chip_t tmb_chip_message_make();
#define TMB_CHIP_MESSAGE() tmb_chip_message_make()

tmb_chip_t tmb_chip_log_level_short_make();
#define TMB_CHIP_LEVEL_S() tmb_chip_log_level_short_make()

tmb_chip_t tmb_chip_log_level_long_make();
#define TMB_CHIP_LEVEL_L() tmb_chip_log_level_long_make()

tmb_chip_t tmb_chip_line_number_make();
#define TMB_CHIP_LINE() tmb_chip_line_number_make()

tmb_chip_t tmb_chip_filename_make();
#define TMB_CHIP_FILE() tmb_chip_filename_make()

tmb_chip_t tmb_chip_base_filename_make();
#define TMB_CHIP_BASEFILE() tmb_chip_base_filename_make()

tmb_chip_t tmb_chip_function_name_make();
#define TMB_CHIP_FUNC() tmb_chip_function_name_make()

#endif
