#ifndef TMB_LIB
#define TMB_LIB
#include <stdbool.h>

bool tmb_init(const char* init_string);
void tmb_log(void* logger, int log_level, const char* message, ...);

#endif
