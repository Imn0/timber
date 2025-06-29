#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <internal.h>
#include <tmb.h>

/*
[formats]
format_name="[ %date ] %message"
other_format="[ %date ] %message"
*/




char* default_fmt[] = { "[ ", NULL, " ] ", NULL };



const Logger default_logger = {
    .consts = default_fmt
};


void __tmb_log(void* logger,
               tmb_log_level log_level,
               LogCtx ctx,
               const char* message,
               ...) {
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}

void tmb_test() {
    printf("helloge\n");
}
