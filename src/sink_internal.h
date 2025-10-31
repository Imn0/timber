#ifndef TMB_SINK_INTERNAL_H_
#define TMB_SINK_INTERNAL_H_

#include <tmb_internal.h>

struct sink_buffer_data {
    int buffer_size;
    int length;
    char items[];
};
tmb_sink_t* tmb_sink_buffer_create(int buff_size);

#endif
