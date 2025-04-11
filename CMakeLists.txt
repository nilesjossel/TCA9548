add_library(TCA9548
    TCA9548.cpp
)

target_include_directories(TCA9548 PUBLIC 
    include
)

target_link_libraries(TCA9548
    pico_stdlib
    hardware_i2c
)

