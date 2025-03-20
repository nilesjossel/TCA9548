#include "TCA9548.h"

TCA9548::TCA9548(i2c_inst_t* i2c, uint8_t deviceAddress)
  : _i2c(i2c), _address(deviceAddress), _mask(0), _resetPin(255), _forced(false), _error(TCA9548_OK), _channels(8) {}

bool TCA9548::begin(uint8_t mask) {
  return setChannelMask(mask);
}

bool TCA9548::isConnected() {
  uint8_t data;
  int result = i2c_read_blocking(_i2c, _address, &data, 1, false);
  return result >= 0;
}

bool TCA9548::isConnected(uint8_t address) {
  uint8_t data;
  int result = i2c_read_blocking(_i2c, address, &data, 1, false);
  return result >= 0;
}

bool TCA9548::isConnected(uint8_t address, uint8_t channel) {
  selectChannel(channel);
  return isConnected(address);
}

uint8_t TCA9548::find(uint8_t address) {
  uint8_t mask = 0;
  for (uint8_t channel = 0; channel < _channels; channel++) {
    if (isConnected(address, channel)) {
      mask |= (1 << channel);
    }
  }
  return mask;
}

uint8_t TCA9548::channelCount() {
  return _channels;
}

bool TCA9548::enableChannel(uint8_t channel) {
  if (channel >= _channels) {
    _error = TCA9548_ERROR_CHANNEL;
    return false;
  }
  _mask |= (1 << channel);
  return setChannelMask(_mask);
}

bool TCA9548::disableChannel(uint8_t channel) {
  if (channel >= _channels) {
    _error = TCA9548_ERROR_CHANNEL;
    return false;
  }
  _mask &= ~(1 << channel);
  return setChannelMask(_mask);
}

bool TCA9548::selectChannel(uint8_t channel) {
  if (channel >= _channels) {
    _error = TCA9548_ERROR_CHANNEL;
    return false;
  }
  _mask = (1 << channel);
  return setChannelMask(_mask);
}

bool TCA9548::isEnabled(uint8_t channel) {
  if (channel >= _channels) {
    _error = TCA9548_ERROR_CHANNEL;
    return false;
  }
  return (_mask & (1 << channel)) != 0;
}

bool TCA9548::disableAllChannels() {
  _mask = 0;
  return setChannelMask(_mask);
}

bool TCA9548::setChannelMask(uint8_t mask) {
  _mask = mask;
  int result = i2c_write_blocking(_i2c, _address, &_mask, 1, false);
  return result >= 0;
}

uint8_t TCA9548::getChannelMask() {
  return _mask;
}

void TCA9548::setResetPin(uint8_t resetPin) {
  _resetPin = resetPin;
  gpio_init(_resetPin);
  gpio_set_dir(_resetPin, GPIO_OUT);
  gpio_put(_resetPin, 1);
}

void TCA9548::reset() {
  if (_resetPin != 255) {
    gpio_put(_resetPin, 0);
    sleep_ms(1);
    gpio_put(_resetPin, 1);
  }
}

void TCA9548::setForced(bool forced) {
  _forced = forced;
}

bool TCA9548::getForced() {
  return _forced;
}

int TCA9548::getError() {
  return _error;
}

// Derived classes implementation

PCA9548::PCA9548(i2c_inst_t* i2c, uint8_t deviceAddress)
  : TCA9548(i2c, deviceAddress) {}

PCA9546::PCA9546(i2c_inst_t* i2c, uint8_t deviceAddress)
  : TCA9548(i2c, deviceAddress) {
  _channels = 4;
}

uint8_t PCA9546::getChannelMask() {
  return _mask & 0x0F;
}

PCA9545::PCA9545(i2c_inst_t* i2c, uint8_t deviceAddress)
  : TCA9548(i2c, deviceAddress) {
  _channels = 4;
}

uint8_t PCA9545::getChannelMask() {
  return _mask & 0x0F;
}

uint8_t PCA9545::getInterruptMask() {
  // Implement interrupt mask reading if needed
  return 0;
}

PCA9543::PCA9543(i2c_inst_t* i2c, uint8_t deviceAddress)
  : TCA9548(i2c, deviceAddress) {
  _channels = 2;
}

uint8_t PCA9543::getChannelMask() {
  return _mask & 0x03;
}

uint8_t PCA9543::getInterruptMask() {
  // Implement interrupt mask reading if needed
  return 0;
}