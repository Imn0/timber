#ifndef TMB_LIB
#define TMB_LIB

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

TMB_API int VERY_SPECIAL_FUNCTION_THAT_IS_IMPLEMENTED_IN_LIB_C(int b);

#endif
