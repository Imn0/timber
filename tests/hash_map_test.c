#include <stddef.h>
#include <tmb_lib.h>

typedef struct {
    _hm_header_(int, int);
} hash_map;

int main() {
    hash_map hm = { 0 };

    hm_put(&hm, 5, 123);
    hm_put(&hm, 3, 12);
    hm_put(&hm, 5, 33);
    int a  = hm_get(&hm, 3);
    bool e = hm_exists(&hm, 6);
    assert(a == 12);
    assert(e == false);
}
