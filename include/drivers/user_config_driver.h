#ifndef __USER_CONFIG_DRIVER_H
#define __USER_CONFIG_DRIVER_H

#include <libconfig.h>
#include <stdbool.h>

typedef struct {
    config_t *libconfig_cfg;
} user_config_t;

int user_config_init(user_config_t *config, char *config_file);
char *user_config_lookup_string(user_config_t *config, char *path);
int user_config_lookup_int(user_config_t *config, char *path, int *value);
int user_config_lookup_double(user_config_t *config, char *path, double *value);
int user_config_lookup_bool(user_config_t *config, char *path, bool *value);
int user_config_deinit(user_config_t *config);

#endif
