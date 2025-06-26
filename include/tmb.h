#ifndef TMB_LIB
#define TMB_LIB
#include <stdbool.h>

extern const char* GIT_REV;
extern const char* GIT_TAG;
extern const char* GIT_BRANCH;
extern const char* GIT_PROJECT_VERSION;
extern const char* PROJECT_VERSION;
extern const char* SOVERSION;

bool tmb_init(const char* init_string);
void tmb_log(void* logger, int log_level, const char* message, ...);

#endif
