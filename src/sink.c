#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

struct sink_fd_data {
    FILE* fd;
};

static void fd_sink(const char* msg, int msg_len, void* data) {
    struct sink_fd_data* sink_data = (struct sink_fd_data*)data;
    fprintf(sink_data->fd, "%.*s", msg_len, msg);
}

tmb_sink_t tmb_sink_fd_make(FILE* fd) {
    struct sink_fd_data* fd_malloced = malloc(sizeof(*fd_malloced));
    fd_malloced->fd                  = fd;
    return (tmb_sink_t) { .sink_data = (void*)fd_malloced,
                          .sink_fn   = fd_sink,
                          .free_fn   = free };
}

static void buffer_sink(const char* msg, int msg_len, void* data) {
    struct sink_buffer_data* buff_data = data;
    int copy_len                       = 0;
    if (msg_len > buff_data->buffer_size)
        copy_len = buff_data->buffer_size;
    else
        copy_len = msg_len;

    for (int i = 0; i < copy_len; i++) { buff_data->items[i] = msg[i]; }
    buff_data->length = copy_len;
}

tmb_sink_t tmb_sink_buffer_make(int buff_size) {
    struct sink_buffer_data* buff_data = malloc(sizeof(*buff_data) +
                                                (size_t)buff_size);
    buff_data->buffer_size             = buff_size;
    return (tmb_sink_t) { .sink_data = (void*)buff_data,
                          .sink_fn   = buffer_sink,
                          .free_fn   = free };
}

static void close_and_free_fd(void* data) {
    struct sink_fd_data* sink_data = data;
    (void)fclose(sink_data->fd);
    free(data);
}

TMB_API tmb_sink_t tmb_sink_file_make(const char* filename) {
    FILE* fd = fopen(filename, "a");
    if (fd == NULL) {}
    tmb_sink_t sink = tmb_sink_fd_make(fd);
    sink.free_fn    = close_and_free_fd;
    return sink;
}

struct rotating_file_data {
    FILE* fd;
    tmb_mutex_t sink_mtx;
    tmb_string_view_t base_filename;
    int max_num_files;
    int max_file_size_B;
    int current_written_B;
};

static void rotating_file_sink(const char* msg, int msg_len, void* data) {
    struct rotating_file_data* sink_data = data;

    int new_size = sink_data->current_written_B + msg_len;
    if (new_size > sink_data->max_file_size_B) {
        // rotate
    }
    printf("%*.s", msg_len, msg);
}

TMB_API tmb_sink_t tmb_sink_rotating_file_make(const char* filename) {
    struct rotating_file_data* rot_file_data = malloc(sizeof(*rot_file_data));

    tmb_string_builder_t sb = { 0 };

    sb_append_cstr(&sb, filename);
    tmb_string_view_t sv = sv_from_sb(&sb);

    rot_file_data->base_filename = sv;
    rot_file_data->fd            = NULL;

    return (tmb_sink_t) { .sink_data = (void*)rot_file_data,
                          .sink_fn   = rotating_file_sink,
                          .free_fn   = free };
}

#ifdef TMB_UNIX
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <unistd.h>

struct sink_graylog_data {
    char address[16];
    int port;
    int sockfd;
    struct sockaddr_in server_addr;
};

static bool graylog_connect(struct sink_graylog_data* gdata) {
    gdata->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (gdata->sockfd < 0) { return false; }

    memset(&gdata->server_addr, 0, sizeof(gdata->server_addr));
    gdata->server_addr.sin_family = AF_INET;
    gdata->server_addr.sin_port   = htons((uint16_t)gdata->port);
    if (inet_pton(AF_INET, gdata->address, &gdata->server_addr.sin_addr) <= 0) {
        close(gdata->sockfd);
        return false;
    }
    return true;
}

static void graylog_log(const char* msg, int msg_len, void* data) {
    struct sink_graylog_data* gdata = data;
    ssize_t sent_bytes              = sendto(gdata->sockfd,
                                msg,
                                (size_t)msg_len,
                                0,
                                (struct sockaddr*)&gdata->server_addr,
                                sizeof(struct sockaddr_in));
    if (sent_bytes < 0) {}
}

TMB_API tmb_sink_t tmb_sink_graylog_make(const char* graylog_host, int port) {
    struct sink_graylog_data* gdata = malloc(sizeof(*gdata));
    strncpy(gdata->address, graylog_host, sizeof(gdata->address) - 1);
    gdata->address[sizeof(gdata->address) - 1] = '\0';
    gdata->port                                = port;
    if (!graylog_connect(gdata)) {
        free(gdata);
        return (tmb_sink_t) { 0 };
    }
    return (tmb_sink_t) { .sink_data = (void*)gdata,
                          .sink_fn   = graylog_log,
                          .free_fn   = free };
}
#endif

TMB_API void tmb_sink_deinit(tmb_sink_t* sink) {
    if (sink->free_fn != NULL) { sink->free_fn(sink->sink_data); }
}
