add_library(TCA9548 STATIC
    TCA9548.cpp
)

target_link_libraries(TCA9548
    pico_stdlib
    hardware_i2c
)

target_include_directories(TCA9548 PUBLIC 
    include
)