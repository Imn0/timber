#ifndef TMB_LIB
#define TMB_LIB
#include <stdbool.h>

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef TMB_SHARED
        #ifdef TMB_EXPORTS
            #define TMB_API __declspec(dllexport)
        #else
            #define TMB_API __declspec(dllimport)
        #endif
    #else
        #define TMB_API
    #endif
#else
    #if defined(TMB_EXPORTS)
        #define TMB_API __attribute__((visibility("default")))
    #else
        #define TMB_API
    #endif
#endif

TMB_API bool tmb_init(const char* init_string);
TMB_API void tmb_log(void* logger, int log_level, const char* message, ...);

#endif
