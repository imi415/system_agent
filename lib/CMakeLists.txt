# Note: LittleVGL officially supports Zephyr and ESP-IDF, we are neither.
file(GLOB_RECURSE LVGL_SOURCES 
    "LittleVGL/src/*.c"
)

set(LVGL_INCLUDES
    "../include"
)

add_library(lvgl STATIC ${LVGL_SOURCES})
target_include_directories(lvgl PRIVATE ${LVGL_INCLUDES})
target_compile_definitions(lvgl PRIVATE ${LVGL_DEFINES})


set(ST7789_LCD_SOURCES
    "st7789_lcd/st7789_lcd.c"
)

add_library(st7789 STATIC ${ST7789_LCD_SOURCES})
target_compile_definitions(st7789 PRIVATE ${ST7789_LCD_DEFINES})

set(BME280_DHT_SOURCES
    "bme280_dht/bme280_dht.c"
)

add_library(bme280 STATIC ${BME280_DHT_SOURCES})
target_compile_definitions(bme280 PRIVATE ${BME280_DHT_DEFINES})
