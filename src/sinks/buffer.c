#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

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

tmb_sink_t* tmb_sink_buffer_create(int buff_size) {
    tmb_sink_t* sink                   = malloc(sizeof(*sink));
    struct sink_buffer_data* buff_data = malloc(sizeof(*buff_data) +
                                                (size_t)buff_size);
    buff_data->buffer_size             = buff_size;

    sink->sink_data = (void*)buff_data;
    sink->sink_fn   = buffer_sink;
    sink->data_free_fn   = free;
    sink->ref_count = 0;
    sink->min_log_level = TMB_LOG_LEVEL_TRACE;
    return sink;
}
