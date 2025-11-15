#include <tmb_lib.h>

int main() {

    ASSERT(tmb_cmp_flt(
            tmb_timestamp_diff((tmb_timestamp_t) { 1, MSEC_TO_NSEC(300) },
                          (tmb_timestamp_t) { 1, MSEC_TO_NSEC(500) }),
            -0.2f));

    ASSERT(tmb_cmp_flt(
            tmb_timestamp_diff((tmb_timestamp_t) { 2, MSEC_TO_NSEC(300) },
                          (tmb_timestamp_t) { 1, MSEC_TO_NSEC(100) }),
            1.2f));

    ASSERT(tmb_cmp_flt(
            tmb_timestamp_diff((tmb_timestamp_t) { 2, MSEC_TO_NSEC(100) },
                          (tmb_timestamp_t) { 1, MSEC_TO_NSEC(500) }),
            0.6f));
}
