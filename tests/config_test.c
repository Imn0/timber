#include <stdio.h>
#include <tmb_internal.h>

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
    ASSERT(tmb_sv_cmp(&sv_make("{$}"), &sv_make(fmt)));
    ASSERT(tmb_lib_cfg.enable_colors == true);
    ASSERT(tmb_lib_cfg.log_level == TMB_LOG_LEVEL_DEBUG);
}
