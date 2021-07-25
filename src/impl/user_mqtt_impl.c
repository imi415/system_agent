#include <stdio.h>
#include <string.h>

#include "impl/user_mqtt_impl.h"

#include "utils/user_log_util.h"

mqtt_influx_ret_t user_mqtt_get_nsec_timestamp_cb(user_mqtt_impl_t *handle, char *timestamp_string) {
    sprintf(timestamp_string, "00000000");
    return MQTT_INFLUX_OK;
}

mqtt_influx_ret_t user_mqtt_publish_message_cb(user_mqtt_impl_t *handle, char *data) {
    USER_LOG(USER_LOG_INFO, "INFLUX: %s", data);

    return MQTT_INFLUX_OK;
}