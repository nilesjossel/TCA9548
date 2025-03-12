#ifndef SSCSRNN015PA3A3_H
#define SSCSRNN015PA3A3_H

#include "hardware/i2c.h"

class SSCSRNN015PA3A3 {
public:
    SSCSRNN015PA3A3(i2c_inst_t* i2c, uint8_t address);
    bool begin();
    float readPressure();

private:
    i2c_inst_t* _i2c;
    uint8_t _address;
    uint16_t readRawData();
};

#endif // SSCSRNN015PA3A3_H