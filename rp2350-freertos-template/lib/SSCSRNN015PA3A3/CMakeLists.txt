add_library(SSCSRNN015PA3A3
    SSCSRNN015PA3A3.cpp
)

target_include_directories(SSCSRNN015PA3A3 PUBLIC 
    include
)

target_link_libraries(SSCSRNN015PA3A3
    pico_stdlib
    hardware_i2c
)