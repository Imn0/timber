#include <stdio.h>
#define TMB_MIN_LOG_LEVEL 3
#include <tmb.h>

int main(int argc, char* argv[]) {
    Logger lg;
    tmb_logger_init_default(&lg);
    TMB_INFO(&lg, "wtf%d", 3);
    TMB_WARNING(&lg, "wtf%d", 3);
    tmb_logger_destroy(&lg);
}
