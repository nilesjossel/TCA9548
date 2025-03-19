add_library(Pico_BME280_Driver STATIC 
"bme280_driver.c"
)

target_link_libraries(Pico_BME280_Driver
        hardware_i2c
)

target_include_directories(Pico_BME280_Driver PUBLIC
    ${CMAKE_CURRENT_LIST_DIR} 
)