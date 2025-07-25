#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <config/lex.h>
#include <format.h>
#include <sink.h>
#include <tmb_internal.h>
#include <tmb_lib.h>

const char* const tmb_log_level_str[] = {
    [TMB_LEVEL_FATAL] = "FATAL",     [TMB_LEVEL_ERROR] = "ERROR",
    [TMB_LEVEL_WARNING] = "WARNING", [TMB_LEVEL_INFO] = "INFO",
    [TMB_LEVEL_DEBUG] = "DEBUG",     [TMB_LEVEL_TRACE] = "TRACE",
};

const int tmb_log_level_str_len[] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE("FATAL"),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE("ERROR"),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE("WARNING"),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE("INFO"),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE("DEBUG"),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE("TRACE"),
};

void tmb_log(LogCtx ctx, const Logger* logger, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_impl(ctx, logger, message, args);
    va_end(args);
}

void tmb_log_default(LogCtx ctx, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_impl(ctx, tmb_get_default_logger(), message, args);
    va_end(args);
}

void tmb_print_version(void) {
    printf("%s.%s.%s @ %s\nSO Version: %s\n",
           TMB_PATCH_V,
           TMB_MINOR_V,
           TMB_MAJOR_V,
           GIT_REV,
           TMB_SO_V);
}
