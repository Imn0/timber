#include <sink_internal.h>
#include <tmb_internal.h>

static void null_sink(const char* msg, int msg_len, void* data) {
    UNUSED msg;
    UNUSED msg_len;
    UNUSED data;
}

tmb_sink_t* tmb_sink_null_create() {
    tmb_sink_t* sink = malloc(sizeof(*sink));

    sink->sink_data    = NULL;
    sink->sink_fn      = null_sink;
    sink->data_free_fn = free;
    sink->ref_count    = 0;
    return sink;
}
