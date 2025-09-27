#include <assert.h>
#include <tmb_lib.h>

typedef struct {
    _da_header_(int*);
} DynamicArray;

int main(void) {
    DynamicArray da = { 0 };
    da_reserve(&da, 2);

    da_append(&da, 1);
    ASSERT(da.items[0] == 1);
    ASSERT(da.length == 1);

    da_append(&da, 2);
    ASSERT(da.items[1] == 2);
    ASSERT(da.length == 2);

    da_append(&da, 3);
    ASSERT(da.items[2] == 3);
    ASSERT(da.length == 3); //

    da_append(&da, 4);
    ASSERT(da.items[3] == 4);
    ASSERT(da.length == 4);

    int a[] = { 1, 2, 3, 4 };
    da_appendn(&da, 4, a);
    ASSERT(da.length == 8);
    ASSERT(da.items[4] == 1);
    ASSERT(da.items[5] == 2);
    ASSERT(da.items[6] == 3);
    ASSERT(da.items[7] == 4);

    DynamicArray da2 = { 0 };
    da_appendn(&da2, 4, a);
    ASSERT(da2.length == 4);
    ASSERT(da2.items[0] == 1);
    ASSERT(da2.items[1] == 2);
    ASSERT(da2.items[2] == 3);
    ASSERT(da2.items[3] == 4);

    da_remove(&da2, 1);
    ASSERT(da2.length == 3);
    ASSERT(da2.items[0] == 1);
    ASSERT(da2.items[1] == 3);
    ASSERT(da2.items[2] == 4);

}
