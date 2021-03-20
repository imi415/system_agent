cmake_minimum_required(VERSION 3.10)

project(SystemAgent)

set(ST7789_LCD_DEFINES "ST7789_PANEL_ZJY_240")
set(LVGL_DEFINES "LV_CONF_INCLUDE_SIMPLE")

set(C_SOURCES
    "src/main.c"
    "src/drivers/user_gpio_driver.c"
    "src/drivers/user_spi_driver.c"
    "src/drivers/user_config_driver.c"
    "src/impl/user_st7789_impl.c"
    "src/impl/user_lvgl_impl.c"
    "src/tasks/user_lvgl_task.c"
    "src/utils/user_log_util.c"
)

set(C_INCLUDES
    "lib/LittleVGL"
    "lib/st7789_lcd"
    "include"
)

set(C_DEFINES
    "_GNU_SOURCE"
    ${ST7789_LCD_DEFINES}
    ${LVGL_DEFINES}
)

set(C_LIBRARIES
    "gpiod"
    "iio"
    "mosquitto"
    "pthread"
    "config"
    "lvgl"
    "st7789"
)

add_subdirectory(lib)

# Main executable target
add_executable(${CMAKE_PROJECT_NAME} ${C_SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${C_LIBRARIES})
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ${C_DEFINES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${C_INCLUDES})
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "-pthread")