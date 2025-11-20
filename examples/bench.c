#ifndef _WIN32
    #ifndef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 200809L
    #endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>
    #include <tmb/sink.h>
    #include <tmb/tmb.h>
    #include <unistd.h>

static inline void measure_start(struct timespec* c) {
    clock_gettime(CLOCK_MONOTONIC, c);
}

static inline double measure_end(struct timespec* start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return ((double)end.tv_sec - (double)start->tv_sec) +
           ((double)end.tv_nsec - (double)start->tv_nsec) / 1e9;
}

    #ifndef BENCH_T
        #define BENCH_T 1000000
    #endif

int main(void) {
    tmb_logger_t* logger = TMB_LOGGER("bench_timber");
    tmb_logger_assign_format(
            logger,
            tmb_logger_add_sink(logger, TMB_SINK_STDERR()),
            tmb_logger_add_format(
                    logger,
                    "[{^7:l}] - {$BLACK:$ANSI_BOLD:}{>]20:@}:{<4:#}{$RESET:} - "
                    "{$}\n"));

    struct timespec start;
    {
        measure_start(&start);

        for (int i = 0; i < BENCH_T; i++) {
            fprintf(stderr,
                    "Benchmarking Timber logging library: message number %d",
                    i);
        }
        printf("printf %f\n", measure_end(&start));
    }
    {
        measure_start(&start);

        for (int i = 0; i < BENCH_T; i++) {
            LOG_INFO(logger,
                     "Benchmarking Timber logging library: message number %d",
                     i);
        }
        printf("timber %f\n", measure_end(&start));
    }

    {
        measure_start(&start);

        for (int i = 0; i < BENCH_T; i++) {
            char* buff = malloc(1024);
            int w      = snprintf(
                    buff,
                    1024,
                    "Benchmarking Timber logging library: message number %d",
                    i);
            int a = write(2, buff, (size_t)w);
            (void)a;
            free(buff);
        }
        printf("write + malloc %f\n", measure_end(&start));
    }

    {
        char* buff = malloc(1024);
        measure_start(&start);

        for (int i = 0; i < BENCH_T; i++) {
            int w = snprintf(
                    buff,
                    1024,
                    "Benchmarking Timber logging library: message number %d",
                    i);
            int a = write(2, buff, (size_t)w);
            (void)a;
        }
        free(buff);
        printf("write + static %f\n", measure_end(&start));
    }

    return 0;
}
#else
int main() {
    return 0;
}
#endif
