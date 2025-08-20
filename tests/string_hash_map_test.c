#include <stddef.h>
#include <tmb_lib.h>

typedef struct {
    _hm_header_(char*, int);
} hash_map;

int main() {
    hash_map hm = { 0 };
    char* p1 = strdup("a very long string so its easy to see in the debbuger");
    char* p2 = strdup("a very long string so its easy to see in the debbuger");
    hm_put(&hm, p1, 12);
    int a = hm_get(&hm, p2);
    assert(a == 12);
}
