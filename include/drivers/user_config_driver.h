#ifndef __USER_CONFIG_DRIVER_H
#define __USER_CONFIG_DRIVER_H

#include <libconfig.h>

typedef struct {
    config_t *libconfig_cfg;
} user_config_t;

int user_config_init(user_config_t *config, char *config_file);
char *user_config_lookup_string(user_config_t *config, char *path);
int user_config_lookup_int(user_config_t *config, char *path, int *value);
int user_config_lookup_float(user_config_t *config, char *path, float *value);
int user_config_deinit(user_config_t *config);

#endif
