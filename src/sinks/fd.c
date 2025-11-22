#include <sink_internal.h>
#include <stdio.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

#ifdef TMB_UNIX
    #include <unistd.h>
#endif

struct sink_fd_data {
#ifdef TMB_WINDOWS
    FILE* fd;
#else
    int fd;
#endif
};

static void fd_sink(const char* msg, int msg_len, void* data) {
    struct sink_fd_data* sink_data = (struct sink_fd_data*)data;
#ifdef TMB_WINDOWS
    if (sink_data->fd) {
        fprintf(sink_data->fd, "%.*s", msg_len, msg);
        fflush(sink_data->fd);
    }
#else
    if (sink_data->fd) {
        int written = (int)write(sink_data->fd, msg, (size_t)msg_len);
        if(written == 0){
            // error writing to fd 
        }
    }
#endif
}

tmb_sink_t* tmb_sink_fd_create(FILE* fd) {
    tmb_sink_t* sink                 = malloc(sizeof(*sink));
    struct sink_fd_data* fd_malloced = malloc(sizeof(*fd_malloced));
#ifdef TMB_WINDOWS
    fd_malloced->fd = fd;
#else
    fd_malloced->fd = fileno(fd);
#endif

    sink->sink_data    = (void*)fd_malloced;
    sink->sink_fn      = fd_sink;
    sink->data_free_fn = free;
    sink->ref_count    = 0;
    return sink;
}
