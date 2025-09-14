#include <stddef.h>
#include <tmb_lib.h>
typedef struct {
    _hm_header_(int, int);
} hash_map;

int main() {
    hash_map hm = { 0 };

    for (int i = 1; i < 10000; i++) {
        hm_put(&hm, i, 21);
        ASSERT(hm.occupied == i);
        ASSERT(hm_get(&hm, i) == 21);
    }
    for (int i = 1; i < 10000; i++) { ASSERT(hm_get(&hm, i) == 21); }
    ASSERT(hm_get(&hm, 3) == 21);
}
