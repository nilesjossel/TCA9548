// Author: Niles Roxas
// Library for the SSCSRNN 15PSI and 1.6Bar pressure sensor

#include "SSCSRNN015PA3A3.h"
#include "pico/stdlib.h"

SSCSRNN015PA3A3::SSCSRNN015PA3A3(i2c_inst_t* i2c, uint8_t address) : _i2c(i2c), _address(address) {}

bool SSCSRNN015PA3A3::begin() {
    return true;
}

uint16_t SSCSRNN015PA3A3::readRawData() {
    uint8_t buffer[2];
    int result = i2c_read_blocking(_i2c, _address, buffer, 2, false);
    if (result != 2) {
        return 0;       // Handles error
    }
    return (buffer[0] << 8) | buffer[1];
}

float SSCSRNN015PA3A3::readPressure() {
    uint16_t rawData = readRawData();
    uint16_t pressureCounts = rawData & 0x3FFF;     // 14-bit pressure value from raw data

    float pressure;
    // 2^14 = 16384 where Pmax(90%) = 14745 and Pmin(10%)= 1638
    // Pressure equation: Output(% of 2^14 counts) = (0.8/(Pmax-Pmin))*(P-Pmin) + 0.1
    // See Reference: SSC series DS datasheet
    if (_address == 0x2C) { // SSCSRNN1.6BA7A3 sensor (0 to 1.6 bar)
        pressure = (pressureCounts - 1638) * (1.6 / (14745 - 1638));
    } 
    else { //Default sensor SSCSRNN015PA3A3 (0 to 15 psi)
        pressure = (pressureCounts - 1638) * (15.0 / (14745 - 1638));
    }

    return pressure;
}