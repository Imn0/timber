#include <stdlib.h>
#include <tmb/sink.h>

TMB_API void tmb_sink_destroy(tmb_sink_t* sink) {
    sink->ref_count--;
    if (sink->ref_count > 0) { return; }
    if (sink->data_free_fn != NULL) { sink->data_free_fn(sink->sink_data); }
    free(sink);
}
