#ifndef TMB_FORMATTER_H_
#define TMB_FORMATTER_H_

#include <tmb_internal.h>

enum CHIP_TYPE {
    CHIP_TYPE_UNKNOWN,
    CHIP_TYPE_MESSAGE,
    CHIP_TYPE_CONST_VAL,
    CHIP_TYPE_LEVEL_L,
    CHIP_TYPE_LEVEL_S,
    CHIP_TYPE_BASEFILE,
    CHIP_TYPE_FILE,
    CHIP_TYPE_LINE,
    CHIP_TYPE_FUNC,
    CHIP_TYPE_COLOR,
};

enum CHIP_COLOR : u8 {
    CHIP_COLOR_BLUE,
    CHIP_COLOR_BLACK,
    CHIP_COLOR_RESET,
    CHIP_COLOR_RED,
};

typedef struct tmb_chip {
    enum CHIP_TYPE type;
    union {
        tmb_string_view_t const_val;
    };
    int just_amount;
    enum tmb_sb_just_opt just_opt;
    enum tmb_sb_truncate_opt truncate_opt;
} tmb_chip_t;
#endif
