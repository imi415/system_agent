#include "drivers/user_config_driver.h"

user_config_ret_t user_config_init(user_config_t *config, char *config_file) {
    config_init(&config->libconfig_cfg);

    if(!config_read_file(&config->libconfig_cfg, config_file)) {
        return USER_CONFIG_ERROR;
    }
}

user_config_ret_t user_config_deinit(user_config_t *config) {
    //
}