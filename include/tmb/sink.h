#ifndef TMB_SINK_H_
#define TMB_SINK_H_

#include "tmb.h"
#include <stdio.h>

struct tmb_sink_opts {
    tmb_log_level min_log_level;
};

#define TMB_SINK_DEFAULT_OPTS .min_log_level = TMB_LOG_LEVEL_DEBUG

TMB_API tmb_sink_t* tmb_sink_fd_create(struct tmb_sink_opts opts, FILE* fd);
#define TMB_SINK_STDOUT(...)                                                   \
    tmb_sink_fd_create(                                                        \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            stdout)
#define TMB_SINK_STDERR(...)                                                   \
    tmb_sink_fd_create(                                                        \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            stderr)
#define TMB_SINK_FD(fd, ...)                                                   \
    tmb_sink_fd_create(                                                        \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ }, fd)

TMB_API tmb_sink_t* tmb_sink_file_create(struct tmb_sink_opts opts,
                                         const char* filename);
#define TMB_SINK_FILE(filename, ...)                                           \
    tmb_sink_file_create(                                                      \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            filename)

TMB_API tmb_sink_t* tmb_sink_rotating_file_create(struct tmb_sink_opts opts,
                                                  const char* filename,
                                                  int max_files,
                                                  int max_file_size_B);
#define TMB_SINK_ROTATING_FILE(filename, max_files, max_file_size_B, ...)      \
    tmb_sink_rotating_file_create(                                             \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            filename,                                                          \
            max_files,                                                         \
            max_file_size_B)

TMB_API tmb_sink_t* tmb_sink_null_create();
#define TMB_SINK_NULL() tmb_sink_null_create()

TMB_API tmb_sink_t* tmb_sink_graylog_create(struct tmb_sink_opts opts,
                                            const char* graylog_host,
                                            int port);
#define TMB_SINK_GRAYLOG(graylog_host, port, ...)                              \
    tmb_sink_graylog_create(                                                   \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            graylog_host,                                                      \
            port)

TMB_API void tmb_sink_destroy(tmb_sink_t* sink);
#endif
