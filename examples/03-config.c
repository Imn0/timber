#include <stdio.h>
#include <tmb/tmb.h>

int main(int argc, char* argv[]) {
    char* config_file;
    if (argc != 2) {
        printf("provide config file\n");
        config_file = "/home/imno/programming/timber/examples/03-config.toml";
    } else {
        config_file = argv[1];
    }
    tmb_load_config(config_file);
}
