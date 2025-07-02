#include <assert.h>
#include <tmb_lib.h>

typedef struct {
    int* items;
    int size;
    int capacity;
} DynamicArray;

int main(void) {
    DynamicArray da = { 0 };
    da_reserve(&da, 2);

    da_append(&da, 1);
    assert(da.items[0] == 1);
    assert(da.size == 1);

    da_append(&da, 2);
    assert(da.items[1] == 2);
    assert(da.size == 2);

    da_append(&da, 3);
    assert(da.items[2] == 3);
    assert(da.size == 3);

    da_append(&da, 4);
    assert(da.items[3] == 4);
    assert(da.size == 4);

    int a[] = { 1, 2, 3, 4 };
    da_appendn(&da, a, 4);
    assert(da.size == 8);
    assert(da.items[4] == 1);
    assert(da.items[5] == 2);
    assert(da.items[6] == 3);
    assert(da.items[7] == 4);

    da_for_each(int, it, &da){
        printf("%d\n", *it);
    }
}
