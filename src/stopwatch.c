#include <tmb_lib.h>

#if defined(TMB_UNIX)
    #include <time.h>
    #include <unistd.h>
#elif defined(TMB_WINDOWS)
    #include <windows.h>
#endif

enum CLOCK_TYPE {
    CLOCK_TYPE_WALL,
    CLOCK_TYPE_MONOTINIC,
};

static tmb_time_stamp_t get_ts(enum CLOCK_TYPE clock_type) {
    // extern inline tmb_time_stamp_t tmb_timestamp();
    i64 sec  = 0;
    i64 nsec = 0;

#if defined(TMB_UNIX)
    struct timespec ts = { 0 };
    if (clock_type == CLOCK_TYPE_WALL) {
        clock_gettime(CLOCK_REALTIME, &ts);
    } else {
        clock_gettime(CLOCK_MONOTONIC, &ts);
    }
    sec  = ts.tv_sec;
    nsec = ts.tv_nsec;
#elif defined(TMB_WINDOWS)

    if (clock_type == CLOCK_TYPE_WALL) {
        FILETIME ft;
        ULARGE_INTEGER uli;

    // we need at least win8 to use GetSystemTimePreciseAsFileTime
    // https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimepreciseasfiletime
    // https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
    #if (_WIN32_WINNT >= 0x0602)
        GetSystemTimePreciseAsFileTime(&ft);
    #else
        GetSystemTimeAsFileTime(&ft);
    #endif

        uli.LowPart  = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;

        unsigned long long ns100 = uli.QuadPart;

        // seconds between 1601 and 1970
        unsigned long long EPOCH_DIFF = 11644473600ULL;

        sec  = (long long)(ns100 / 10000000ULL - EPOCH_DIFF);
        nsec = (long long)((ns100 % 10000000ULL) * 100);
    } else { // CLOCK_TYPE_MONOTONIC
        static LARGE_INTEGER freq;
        static int freq_init = 0;

        if (!freq_init) {
            QueryPerformanceFrequency(&freq);
            freq_init = 1;
        }

        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        sec  = (long long)(counter.QuadPart / freq.QuadPart);
        nsec = (long long)(((counter.QuadPart % freq.QuadPart) * 1000000000LL) /
                           freq.QuadPart);
    }
#else
    sec = time(NULL);
#endif

    return (tmb_time_stamp_t) { .sec = sec, .nsec = nsec };
}

tmb_time_stamp_t tmb_time_stamp() {
    return get_ts(CLOCK_TYPE_WALL);
}

tmb_time_stamp_t tmb_time_stopwatch() {
    return get_ts(CLOCK_TYPE_MONOTINIC);
}
