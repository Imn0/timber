#include <tmb/tmb.h>
#include <tmb_lib.h>
#include <formatter.h>

int main() {
    const char* fmt = "{$LEVEL} {D} {{ {$} }}test\n";

    tmb_formatter_t formatter = tmb_formatter_make(fmt);
    tmb_formatter_print(&formatter);
    printf("%d\n", formatter.length);
}
