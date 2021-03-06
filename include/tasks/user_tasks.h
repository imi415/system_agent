#ifndef USER_TASKS_H
#define USER_TASKS_H

int user_lvgl_task_init(void);
int user_lvgl_task_deinit(void);

int user_clock_task_init(void);
int user_clock_task_deinit(void);

int user_dht_task_init(void);
int user_dht_task_deinit(void);

int user_base_task_init(void);
int user_base_task_deinit(void);

int user_mqtt_task_init(void);
int user_mqtt_task_deinit(void);

int user_tvoc_task_init(void);
int user_tvoc_task_deinit(void);

#endif
