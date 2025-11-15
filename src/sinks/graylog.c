#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

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

TMB_API tmb_sink_t* tmb_sink_graylog_create(const char* graylog_host,
                                            int port) {
    struct sink_graylog_data* gdata = malloc(sizeof(*gdata));
    strncpy(gdata->address, graylog_host, sizeof(gdata->address) - 1);
    gdata->address[sizeof(gdata->address) - 1] = '\0';
    gdata->port                                = port;
    if (!graylog_connect(gdata)) {
        free(gdata);
        return NULL;
    }
    tmb_sink_t* sink = malloc(sizeof(*sink));

    sink->sink_data = (void*)gdata;
    sink->sink_fn   = graylog_log;
    sink->data_free_fn   = free;
    sink->ref_count = 0;
    return sink;
}
#endif
