#define TMB_MIN_LOG_LEVEL 3
#include <tmb.h>

int main(void) {
    Logger lgr;
    tmb_logger_init_default(&lgr);
    TMB_INFO(&lgr, "wtf%d", 3);
    TMB_WARNING(&lgr, "wtf%d", 3);
    tmb_logger_destroy(&lgr);
}
