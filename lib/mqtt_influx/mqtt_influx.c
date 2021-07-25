#include "mqtt_influx.h"

#define MAX_MQTT_REPORT_SIZE 256

static int concat_string(char *dest, int dest_length, char *append) {
    int dest_ptr;
    for(dest_ptr = 0; dest_ptr < dest_length; dest_ptr++) {
        if(dest[dest_ptr] == '\0') break;
    }

    int i = 0;
    while(dest_ptr + i < dest_length - 1) {
        if(append[i] != '\0') {
            dest[dest_ptr + i] = append[i];
            dest[dest_ptr + i + 1] = '\0';
            i++;
        } else
            break;
    }
}

mqtt_influx_ret_t mqtt_influx_init(mqtt_influx_t *influx) {
    return MQTT_INFLUX_OK;
}

mqtt_influx_ret_t mqtt_influx_publish_measurement(mqtt_influx_t *influx,
                                                  mqtt_influx_measure_t *meas) {

    char mqtt_buf[MAX_MQTT_REPORT_SIZE];
    char ns_ts[32];

    if(influx->cb.get_nsec_timestamp_cb(influx->user_data, ns_ts) !=
       MQTT_INFLUX_OK) {
        return MQTT_INFLUX_ERROR;
    }

    for(int i = 0; i < MAX_MQTT_REPORT_SIZE; i++) {
        mqtt_buf[i] = '\0';
    }

    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, meas->measurement);
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, ",hostname=");
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, influx->hostname);
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, " ");
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, meas->key);
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, "=");
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, meas->value);
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, " ");
    concat_string(mqtt_buf, MAX_MQTT_REPORT_SIZE, ns_ts);

    if(influx->cb.publish_message_cb(influx->user_data, mqtt_buf) != MQTT_INFLUX_OK) {
        return MQTT_INFLUX_ERROR;
    }

    return MQTT_INFLUX_OK;
}