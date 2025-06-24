#ifndef TMB_LIB
#define TMB_LIB

#ifdef _WIN32
  #ifdef TMB_EXPORTS
    #define TMB_API __declspec(dllexport)
  #else
    #define TMB_API __declspec(dllimport)
  #endif
#else
  #define TMB_API
#endif

TMB_API int VERY_SPECIAL_FUNCTION_THAT_IS_IMPLEMENTED_IN_LIB_C(int b);

#endif
