#include <stdlib.h>

#include "drivers/user_config_driver.h"
#include "utils/user_log_util.h"

int user_config_init(user_config_t *config, char *config_file) {
    USER_LOG(USER_LOG_INFO, "Config init.");

    config->libconfig_cfg = malloc(sizeof(config_t));
    if(config->libconfig_cfg == NULL) return -1;

    config_init(config->libconfig_cfg);

    if(!config_read_file(config->libconfig_cfg, config_file)) {
        USER_LOG(USER_LOG_ERROR,
                 "Failed to read config file %s: %s in file %s at line %d.",
                 config_file, config_error_text(config->libconfig_cfg),
                 config_error_file(config->libconfig_cfg),
                 config_error_line(config->libconfig_cfg));
        return -2;
    }

    return 0;
}

char *user_config_lookup_string(user_config_t *config, char *path) {
    const char *value;

    // This string is auto-freeed by libconfig.
    if(config_lookup_string(config->libconfig_cfg, path, &value) == 0) {
        return NULL;
    }
    return (char *)value;
}

int user_config_lookup_int(user_config_t *config, char *path, int *value) {
    if(config_lookup_int(config->libconfig_cfg, path, value) == 0) {
        return -1;
    }
    return 0;
}

int user_config_lookup_double(user_config_t *config, char *path,
                              double *value) {
    if(config_lookup_float(config->libconfig_cfg, path, value) == 0) {
        return -1;
    }
    return 0;
}

int user_config_lookup_bool(user_config_t *config, char *path, bool *value) {
    if(config_lookup_bool(config->libconfig_cfg, path, (int *)value) == 0) {
        return -1;
    }
    return 0;
}

int user_config_deinit(user_config_t *config) {
    config_destroy(config->libconfig_cfg);

    if(config->libconfig_cfg) {
        free(config->libconfig_cfg);
    }

    return 0;
}
