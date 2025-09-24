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

enum CHIP_ANSI {
    CHIP_ANSI_INVALID,
    CHIP_ANSI_RESET,
    CHIP_ANSI_BOLD,
    CHIP_ANSI_DIM,
    CHIP_ANSI_ITALIC,
    CHIP_ANSI_UNDERLINE,
    CHIP_ANSI_BLINK,
    CHIP_ANSI_REVERSE,
    CHIP_ANSI_BLACK,
    CHIP_ANSI_RED,
    CHIP_ANSI_GREEN,
    CHIP_ANSI_YELLOW,
    CHIP_ANSI_BLUE,
    CHIP_ANSI_MAGENTA,
    CHIP_ANSI_CYAN,
    CHIP_ANSI_WHITE,
    CHIP_ANSI_BG_BLACK,
    CHIP_ANSI_BG_RED,
    CHIP_ANSI_BG_GREEN,
    CHIP_ANSI_BG_YELLOW,
    CHIP_ANSI_BG_BLUE,
    CHIP_ANSI_BG_MAGENTA,
    CHIP_ANSI_BG_CYAN,
    CHIP_ANSI_BG_WHITE,
};

typedef struct tmb_chip {
    enum CHIP_TYPE type;
    union {
        tmb_string_view_t const_val;
        enum CHIP_ANSI ansi_val;
    };
    int just_amount;
    enum tmb_sb_just_opt just_opt;
    enum tmb_sb_truncate_opt truncate_opt;
} tmb_chip_t;

void tmb_formatter_deinit(tmb_formatter_t* formatter);
#endif
