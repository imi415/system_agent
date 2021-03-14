# Note: LittleVGL officially supports Zephyr and ESP-IDF, we are neither.
file(GLOB_RECURSE LVGL_SOURCES 
    "LittleVGL/src/*.c"
)

set(LVGL_INCLUDES
    "../include"
)

add_library(lvgl STATIC ${LVGL_SOURCES})
target_include_directories(lvgl PRIVATE ${LVGL_INCLUDES})
target_compile_definitions(lvgl PRIVATE "LV_CONF_INCLUDE_SIMPLE")


set(ST7789_LCD_SOURCES
    "st7789_lcd/st7789_lcd.c"
)

add_library(st7789 STATIC ${ST7789_LCD_SOURCES})