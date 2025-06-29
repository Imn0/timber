#include <tmb_lib.h>

void sb_appendf(StringBuilder* sb, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    da_reserve(sb, sb->size + n + 1);

    va_start(args, fmt);
    vsnprintf(sb->items + sb->size, (unsigned long)n + 1, fmt, args);
    va_end(args);

    sb->size += n;
}
