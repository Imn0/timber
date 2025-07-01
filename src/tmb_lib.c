#include <tmb_lib.h>

void sb_appendf(StringBuilder* sb, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    sb_appendv(sb, fmt, args);
    va_end(args);
}

void sb_appendv(StringBuilder* sb, const char* fmt, va_list args) {

    va_list args1;
    va_copy(args1, args);

    int n = vsnprintf(NULL, 0, fmt, args1);
    va_end(args1);

    da_reserve(sb, sb->size + n + 1);

    va_copy(args1, args);
    vsnprintf(sb->items + sb->size, (unsigned long)n + 1, fmt, args1);
    va_end(args1);

    sb->size += n;
}
