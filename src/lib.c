#include <tmb.h>
#include <stdio.h>

bool tmb_init(const char* init_string) {
    if (init_string == NULL) { return false; }

    int idx = 0;
    while (init_string[idx] != '\0') {
        if (init_string[idx] == '%') {
            switch (init_string[idx + 1]) {
            case 'd':
                printf("\tDATE\n");
                break;
            case 'm':
                printf("\tMSG\n");
                break;
            case '\0':
                printf("found end of input, expected logging specifier\n");
                return false;
            default:
                printf("invalid %% specifier: %c\n", init_string[idx + 1]);
                return false;
            }
            idx++;
        } else {
            printf("\t%c\n", init_string[idx]);
        }
        idx++;
    }
    return true;
}
