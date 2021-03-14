#ifndef __USER_CONFIG_DRIVER_H
#define __USER_CONFIG_DRIVER_H

#include <libconfig.h>

typedef enum {
    USER_CONFIG_OK,
    USER_CONFIG_ERROR
} user_config_ret_t;

typedef struct {
    config_t libconfig_cfg;
} user_config_t;

user_config_ret_t user_config_init(user_config_t *config, char *config_file);
user_config_ret_t user_config_deinit(user_config_t *config);

#endif
