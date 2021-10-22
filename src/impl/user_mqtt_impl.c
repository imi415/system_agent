#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "drivers/user_config_driver.h"
#include "impl/user_mqtt_impl.h"
#include "user_common.h"
#include "utils/user_log_util.h"

#define USER_MQTT_CONFIG_COMMON "agent.libraries.mqtt."

// TODO: Add callbacks here as static functions.

extern user_config_t g_config;

/**
 * @brief Mosquitto connect callback function.
 * 
 * @param mosq mosquitto object pointer
 * @param obj user pointer passed to mosquitto_new()
 * @param rc connect return value, see section 3.2.2.3 of 
 * http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.html
 */
static void user_moqtt_impl_cb_on_connect(struct mosquitto *mosq, void *obj, int rc) {
    switch(rc) {
        case 0:
        USER_LOG(USER_LOG_INFO, "MQTT connection accepted.");
        break;
        case 1:
        USER_LOG(USER_LOG_WARN, "MQTT connection refused, unacceptable protocol version.");
        break;
        case 2:
        USER_LOG(USER_LOG_WARN, "MQTT connection refused, identifier rejected.");
        break;
        case 3:
        USER_LOG(USER_LOG_WARN, "MQTT connection refused, server unavailable.");
        break;
        case 4:
        USER_LOG(USER_LOG_WARN, "MQTT connection refused, bad username or password.");
        break;
        case 5:
        USER_LOG(USER_LOG_WARN, "MQTT connection refused, not authorized.");
        break;
        default:
        USER_LOG(USER_LOG_WARN, "MQTT connection callback: unknown rc (%d).", rc);
        break;
    }

    if(rc == 0) {
        user_mqtt_impl_t *handle = obj;
        handle->ready = true;
        // If we are about to subscribe some topics, do it here.
    }
}

/**
 * @brief Mosquitto disconnect event callback function,
 * used to determine whether this disconnect is expected or not.
 *
 * @param mosq mosquitto object pointer
 * @param obj user pointer passed to mosquitto_new()
 * @param rc disconnect reason, 0 is expected.
 */
static void user_mqtt_impl_cb_on_disconnect(struct mosquitto *mosq, void *obj, int rc) {
    if(rc != 0) {
        USER_LOG(USER_LOG_WARN, "MQTT received unexpected disconnect event (%d).", rc);
    } else {
        USER_LOG(USER_LOG_INFO, "Disconnected successfully from MQTT broker.");
    }
}

/**
 * @brief Mosquitto logging callback function.
 * 
 * @param mosq mosquitto object pointer
 * @param obj user pointer passed to mosquitto_new()
 * @param level log level
 * @param str log message
 */
static void user_mqtt_impl_cb_log(struct mosquitto *mosq, void *obj, int level, const char *str) {
    user_log_level_t log_level;

    switch(level) {
    case MOSQ_LOG_DEBUG:
        log_level = USER_LOG_DEBUG;
        break;
    case MOSQ_LOG_INFO:
    case MOSQ_LOG_NOTICE:
        log_level = USER_LOG_INFO;
        break;
    case MOSQ_LOG_WARNING:
        log_level = USER_LOG_WARN;
        break;
    case MOSQ_LOG_ERR:
        log_level = USER_LOG_ERROR;
        break;
    default:
        log_level = USER_LOG_DEBUG;
        break;
    }

    USER_LOG(log_level, "mosquitto: %s", str);
}

/**
 * @brief Mosquitto config (mainly TLS related)
 * 
 * @param handle user_mqtt_impl_t handle pointer
 * @return 0 if success, negative value if error.
 */
static inline int user_mqtt_impl_config(user_mqtt_impl_t *handle) {
    char *server_host = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "server.host");
    if(!server_host) {
        USER_LOG(USER_LOG_ERROR, "failed to lookup MQTT host string.");
        return -1;
    }

    int server_port = 0;
    if(user_config_lookup_int(&g_config, USER_MQTT_CONFIG_COMMON "server.port", &server_port) != 0) {
        USER_LOG(USER_LOG_ERROR, "failed to lookup MQTT port.");
        return -1;
    }

    bool use_tls = false;
    if(user_config_lookup_bool(&g_config, USER_MQTT_CONFIG_COMMON "tls.enabled", &use_tls) != 0) {
        USER_LOG(USER_LOG_ERROR, "failed to get TLS config.");
        return -1;
    }

    if(use_tls) {
        char *psk = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "tls.psk");
        if(!psk || (psk[0] == '\0')) {
            // Use client certificate
            char *ca_path = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "tls.ca_path");
            char *ca_file = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "tls.ca_file");
            if(!ca_path || (ca_path[0] == '\0')) ca_path = NULL;
            if(!ca_file || (ca_file[0] == '\0')) ca_file = NULL;
            if(!ca_file && !ca_path) {
                USER_LOG(USER_LOG_ERROR, "failed to set CA path or file.");
                return -2;
            }

            char *client_cert = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "tls.client_cert_file");
            char *client_key = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "tls.client_key_file");
            if(!client_cert || (client_cert[0] == '\0')) client_cert = NULL;
            if(!client_key || (client_key[0] == '\0')) client_key = NULL;
            if(client_cert == NULL && client_key != NULL) {
                USER_LOG(USER_LOG_ERROR, "must provide both certificate and private key.");
                return -2;
            } else if(client_cert != NULL && client_key == NULL) {
                USER_LOG(USER_LOG_ERROR, "must provide both certificate and private key.");
                return -2;
            }

            if(mosquitto_tls_set(handle->mosq, ca_file, ca_path, client_cert, client_key, NULL) != MOSQ_ERR_SUCCESS) {
                return -2;
            }
        } else {
            if(mosquitto_tls_psk_set(handle->mosq, psk, "IDENTITY", NULL) != MOSQ_ERR_SUCCESS) {
                return -2;
            }
        }
    }

    return 0;
}

/**
 * @brief Set mosquitto username and password, connect
 * to server.
 * 
 * @param handle user_mqtt_impl_t handle
 * @return 0 if success, negative value if error.
 */
static inline int user_mqtt_connect(user_mqtt_impl_t *handle) {
    char *mqtt_user = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "client.username");
    char *mqtt_pass = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "client.password");
    if(mqtt_user && (mqtt_user[0] != '\0')) {
        if(!mqtt_pass || (mqtt_pass[0] == '\0')) mqtt_pass = NULL;
        mosquitto_username_pw_set(handle->mosq, mqtt_user, mqtt_pass);
        USER_LOG(USER_LOG_DEBUG, "set MQTT username and password.");
    }

    char *mqtt_host = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "server.host");
    if(!mqtt_host) {
        USER_LOG(USER_LOG_ERROR, "failed to get MQTT server.");
        return -1;
    }
    int mqtt_port = 0;
    if(user_config_lookup_int(&g_config, USER_MQTT_CONFIG_COMMON "server.port", &mqtt_port) != 0) {
        USER_LOG(USER_LOG_ERROR, "failed to get MQTT port.");
        return -1;
    }
    int mqtt_keepalive = 0;
    if(user_config_lookup_int(&g_config, USER_MQTT_CONFIG_COMMON "server.keepalive", &mqtt_keepalive) != 0) {
        USER_LOG(USER_LOG_INFO, "No MQTT keepalive defined, use default.");
        mqtt_keepalive = 30;
    }

    mosquitto_log_callback_set(handle->mosq, user_mqtt_impl_cb_log);
    mosquitto_connect_callback_set(handle->mosq, user_moqtt_impl_cb_on_connect);
    mosquitto_disconnect_callback_set(handle->mosq, user_mqtt_impl_cb_on_disconnect);

    int result = mosquitto_connect(handle->mosq, mqtt_host, mqtt_port, mqtt_keepalive);

    if(result != MOSQ_ERR_SUCCESS) {
        USER_LOG(USER_LOG_ERROR, "mosquitto connect() failed, result is %s.", mosquitto_strerror(result));
        return -2;
    }

    return 0;
}

/**
 * @brief Init MQTT implementation
 * 
 * @param handle user_mqtt_impl_t handle pointer
 * @return 0 if success, negative value if error.
 */
int user_mqtt_impl_init(user_mqtt_impl_t *handle) {
    int mosq_major, mosq_minor, mosq_revision;
    mosquitto_lib_init();

    mosquitto_lib_version(&mosq_major, &mosq_minor, &mosq_revision);
    USER_LOG(USER_LOG_INFO, "libmosquitto library version %d.%d rev. %d.", mosq_major, mosq_minor, mosq_revision);

    char *topic = user_config_lookup_string(&g_config, USER_MQTT_CONFIG_COMMON "server.topic");
    if(!topic || (topic[0] == '\0')) {
        USER_LOG(USER_LOG_ERROR, "MQTT topic undefined.");
        return -1;
    }

    int topic_len = strlen(topic);
    if(topic_len > USER_MQTT_IMPL_TOPIC_MAX_LEN) {
        USER_LOG(USER_LOG_WARN, "MQTT topic too long, cap to %d bytes.", USER_MQTT_IMPL_TOPIC_MAX_LEN);
        topic_len = USER_MQTT_IMPL_TOPIC_MAX_LEN;
    }

    strncpy(handle->topic, topic, topic_len);

    // Init mosquitto instance.
    handle->mosq = mosquitto_new("ID", false, handle);
    if(handle->mosq == NULL) {
        int err = errno;
        USER_LOG(USER_LOG_ERROR, "mosquitto client creation failed, reason: (%s).", strerror(err));
        return -1;
    }

    // Enable multi-thread support.
    mosquitto_threaded_set(handle->mosq, true);

    if(user_mqtt_impl_config(handle) != 0) {
        USER_LOG(USER_LOG_ERROR, "MQTT config failed.");
        return -2;
    }

    if(user_mqtt_connect(handle) != 0) {
        USER_LOG(USER_LOG_ERROR, "MQTT connect failed.");
        return -3;
    }

    return 0;
}

/**
 * @brief De-initialize MQTT implementation
 * 
 * @param handle user_mqtt_impl_t handle pointer
 * @return 0 if success, negative value if error.
 */
int user_mqtt_impl_deinit(user_mqtt_impl_t *handle) {
    mosquitto_destroy(handle->mosq);
    mosquitto_lib_cleanup();

    return 0;
}

/**
 * @brief MQTT implementation main network loop, run this forever.
 * 
 * @param handle user_mqtt_impl_t handle pointer
 * @return 0 if success, negative value if error.
 */
int user_mqtt_network_loop(user_mqtt_impl_t *handle) {
    int status = mosquitto_loop(handle->mosq, 1000, 1);
    if(status == MOSQ_ERR_SUCCESS) {
        return 0;
    } else if(status == MOSQ_ERR_NO_CONN) {
        USER_LOG(USER_LOG_DEBUG, "Mosquitto loop not connected");
        return 0;
    } else {
        USER_LOG(USER_LOG_ERROR, "Mosquitto loop returned error: %s.", mosquitto_strerror(status));
        return -1;
    }
}

mqtt_influx_ret_t user_mqtt_get_nsec_timestamp_cb(user_mqtt_impl_t *handle, char *timestamp_string) {
    UNUSED(handle);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t ts_int = ts.tv_sec * 1e9 + ts.tv_nsec;
    sprintf(timestamp_string, "%lu", ts_int);
    return MQTT_INFLUX_OK;
}

mqtt_influx_ret_t user_mqtt_publish_message_cb(user_mqtt_impl_t *handle, char *data) {
    USER_LOG(USER_LOG_DEBUG, "MQTT message: %s", data);

    // TODO: Exception handling required.
    mosquitto_publish(handle->mosq, NULL, handle->topic, strlen(data), data, 1, false);

    return MQTT_INFLUX_OK;
}

bool user_mqtt_ready_cb(user_mqtt_impl_t *handle) {
    return handle->ready;
}