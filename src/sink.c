#include <tmb/sink.h>
#include <tmb_internal.h>

struct fd_sink_data {
    FILE* fd;
};

static void fd_sink(const char* msg, int msg_len, void* data) {
    struct fd_sink_data* sink_data = (struct fd_sink_data*)data;
    fprintf(sink_data->fd, "%.*s", msg_len, msg);
}

tmb_sink_t tmb_sink_fd_make(FILE* fd) {
    struct fd_sink_data* fd_malloced = malloc(sizeof(*fd_malloced));
    fd_malloced->fd                  = fd;
    return (tmb_sink_t) { .sink_data = (void*)fd_malloced,
                          .sink_fn   = fd_sink,
                          .free_fn   = free };
}

TMB_API void tmb_sink_deinit(tmb_sink_t* sink) {
    if (sink->free_fn != NULL) { sink->free_fn(sink->sink_data); }
}
