#include <tmb_lib.h>

tmb_time_stamp_t tmb_timestamp() {
    // extern inline tmb_time_stamp_t tmb_timestamp();
    long long sec  = 0;
    long long nsec = 0;

#if defined(TMB_UNIX)
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    sec  = ts.tv_sec;
    nsec = ts.tv_nsec;
#elif defined(TMB_WINDOWS)

    sec = time(NULL);
#else
    sec = time(NULL);
#endif

    return (tmb_time_stamp_t) { .sec = sec, .nsec = nsec };
}
