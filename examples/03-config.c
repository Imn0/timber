#include <stdio.h>
#include <tmb/tmb.h>

/*
[formats]
b = "{$}"

[global]
log_level = "DEBUG"  # 4
enable_colors = true
*/

int main(void) {
    const char* config_contents = "[formats]                \n\
                                  b = \"{$}\"               \n\
                                  [global]                  \n\
                                  log_level = \"DEBUG\" #4  \n\
                                  enable_colors = true      \n";

    struct tmb_config* cfg = tmb_config_from_string(config_contents);
    if (cfg == NULL) {
        printf("loading config failed\n");
        return 1;
    }
    const char* fmt = tmb_config_get_format(cfg, "b");
    printf("b -> %s\n", fmt);
}
