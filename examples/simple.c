#include <lib.h>
#include <stdio.h>

int main() {
    int a = 3;
    int c = a + VERY_SPECIAL_FUNCTION_THAT_IS_IMPLEMENTED_IN_LIB_C(a);
    printf("%d\n", c);
}
