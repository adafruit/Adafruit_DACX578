#include "Adafruit_DACX578.h"

/*!
 * @brief Constructor for Adafruit_DACX578
 * @param resolution The DAC resolution in bits (8, 10, or 12). Defaults to 8 if
 * invalid.
 */
Adafruit_DACX578::Adafruit_DACX578(uint8_t resolution) {
  if (resolution == 10 || resolution == 12) {
    _resolution_bits = resolution;
  } else {
    _resolution_bits = 8; // Default to 8 bits if invalid resolution is provided
  }
}

/*!
 * @brief Destructor for Adafruit_DACX578
 */
Adafruit_DACX578::~Adafruit_DACX578() {
  if (_i2c_dev) {
    delete _i2c_dev;
    _i2c_dev = nullptr;
  }
}

/*!
 * @brief Initializes the DAC7578 and checks if the device is connected.
 * @param i2c_addr The I2C address of the DAC (default: 0x47).
 * @param wire Pointer to the I2C bus instance (default: &Wire).
 * @return True if initialization was successful, otherwise false.
 */
bool Adafruit_DACX578::begin(uint8_t i2c_addr, TwoWire *wire) {
  if (_i2c_dev) {
    delete _i2c_dev; // Clean up if already allocated
    _i2c_dev = nullptr;
  }

  _i2c_addr = i2c_addr;
  _wire = wire;

  _i2c_dev = new Adafruit_I2CDevice(_i2c_addr, _wire);
  if (!_i2c_dev->begin()) {
    return false;
  }
  return true;
}

/*!
 * @brief Resets the DAC using the software reset command.
 * @return True if the command was successfully sent, otherwise false.
 */
bool Adafruit_DACX578::reset(void) {
  uint8_t reset_cmd = DACX578_CMD_RESET;
  return _i2c_dev->write(&reset_cmd, 1);
}

/*!
 * @brief Writes a value to the input register of a specific DAC channel.
 * @param channel The DAC channel to write to (0-7 or
 * DACX578_CHANNEL_BROADCAST).
 * @param value The 8/10/12-bit value to write to the DAC.
 * @return True if the write was successful, otherwise false.
 */
bool Adafruit_DACX578::writeChannelValue(uint8_t channel, uint16_t value) {
  if (channel > 7 && channel != DACX578_CHANNEL_BROADCAST) {
    return false; // Invalid channel
  }
  return commandWrite(DACX578_CMD_WRITE | (channel & 0xF), value);
}

/*!
 * @brief Updates the DAC output with the written register value from before.
 * @param channel The DAC channel to update (0-7 or DACX578_CHANNEL_BROADCAST).
 * @return True if the update was successful, otherwise false.
 */
bool Adafruit_DACX578::updateChannel(uint8_t channel) {
  if (channel > 7 && channel != DACX578_CHANNEL_BROADCAST) {
    return false; // Invalid channel
  }
  return commandWrite(DACX578_CMD_UPDATE | (channel & 0xF), 0x0);
}

/*!
 * @brief Writes a value to the input register and updates the DAC output for a
 * specific channel.
 * @param channel The DAC channel to write and update (0-7 or
 * DACX578_CHANNEL_BROADCAST).
 * @param value The 8/10/12-bit value to write and update the DAC with.
 * @return True if the operation was successful, otherwise false.
 */
bool Adafruit_DACX578::writeAndUpdateChannelValue(uint8_t channel,
                                                  uint16_t value) {
  if (channel > 7 && channel != DACX578_CHANNEL_BROADCAST) {
    return false; // Invalid channel
  }
  return commandWrite(DACX578_CMD_WRITE_UPDATE | (channel & 0xF), value);
}

/*!
 * @brief Writes a value to the input register and performs a global update of
 * all DAC outputs.
 * @param channel The DAC channel to write to (0-7 or
 * DACX578_CHANNEL_BROADCAST).
 * @param value The 8/10/12-bit value to write before the global update.
 * @return True if the operation was successful, otherwise false.
 */
bool Adafruit_DACX578::writeAndGlobalUpdateChannelValue(uint8_t channel,
                                                        uint16_t value) {
  if (channel > 7 && channel != DACX578_CHANNEL_BROADCAST) {
    return false; // Invalid channel
  }
  return commandWrite(DACX578_CMD_WRITE_GLOBAL_UPDATE | (channel & 0xF), value);
}

/*!
 * @brief Reads the value from the input register of a specific DAC channel.
 * @param channel The DAC channel to read from (0-7).
 * @param value Pointer to store the read 8/10/12-bit value.
 * @return True if the read was successful, otherwise false.
 */
bool Adafruit_DACX578::readInputChannelValue(uint8_t channel, uint16_t *value) {
  if (channel > 7) {
    return false; // Invalid channel
  }
  return commandRead(DACX578_CMD_WRITE | (channel & 0xF), value);
}

/*!
 * @brief Reads the value from the DAC register (current output) of a specific
 * DAC channel.
 * @param channel The DAC channel to read from (0-7).
 * @param value Pointer to store the read 8/10/12-bit value.
 * @return True if the read was successful, otherwise false.
 */
bool Adafruit_DACX578::readChannelValue(uint8_t channel, uint16_t *value) {
  if (channel > 7) {
    return false; // Invalid channel
  }
  return commandRead(DACX578_CMD_UPDATE | (channel & 0xF), value);
}

/*!
 * @brief Writes the LDAC mask to the DAC to control which channels respond to
 * LDAC.
 * @param ldac_mask The 8-bit LDAC mask, where each bit corresponds to a channel
 * (0-7).
 * @return True if the operation was successful, otherwise false.
 */
bool Adafruit_DACX578::writeLDAC(uint8_t ldac_mask) {
  uint8_t buffer[3];
  buffer[0] = DACX578_CMD_LDAC_MASK;
  buffer[1] = ldac_mask;
  buffer[2] = 0;
  return _i2c_dev->write(buffer, 3);
}

/*!
 * @brief Reads the current LDAC mask from the DAC.
 * @return The 8-bit LDAC mask, where each bit corresponds to a channel (0-7).
 */
uint8_t Adafruit_DACX578::readLDAC(void) {
  uint8_t buffer[2] = {0, 0};
  uint8_t command = DACX578_CMD_LDAC_MASK;
  if (!_i2c_dev->write_then_read(&command, 1, buffer, 2)) {
    return 0;
  }

  return buffer[1];
}

/*!
 * @brief Sets the clear code register to determine what value the DAC outputs
 * on a clear event.
 * @param clear_code The desired clear code (zero, midscale, full scale, or no
 * operation).
 * @return True if the operation was successful, otherwise false.
 */
bool Adafruit_DACX578::setClearCode(dacx578_clear_code_t clear_code) {
  if (clear_code > DACX578_CLEAR_CODE_NOP) {
    return false; // Invalid clear code
  }

  uint8_t buffer[3];
  buffer[0] =
      (DACX578_CMD_RESET);       // Command byte (0x5 << 4 already pre-shifted)
  buffer[1] = 0x00;              // Upper 8 bits of DNC
  buffer[2] = clear_code & 0x03; // Lower 2 bits of the clear code

  return _i2c_dev->write(buffer, 3);
}

/*!
 * @brief Reads the current clear code register value.
 * @return The 2-bit clear code (zero, midscale, full scale, or no operation).
 *         Returns DACX578_CLEAR_CODE_NOP on failure.
 */
dacx578_clear_code_t Adafruit_DACX578::getClearCode(void) {
  uint8_t cmd = DACX578_CMD_RESET; // Command byte to read clear code register
  uint8_t buffer[3];

  if (!_i2c_dev->write_then_read(&cmd, 1, buffer, 3)) {
    return DACX578_CLEAR_CODE_NOP; // Return NOP on failure
  }

  return (dacx578_clear_code_t)(buffer[2] &
                                0x03); // Extract and return lower 2 bits
}

/*!
 * @brief Writes a command and a 16-bit value to the DAC.
 * @param command The command to write (pre-shifted upper nibble with channel
 * info).
 * @param value The 8/10/12-bit value to write, shifted based on the resolution.
 * @return True if the write was successful, otherwise false.
 */
bool Adafruit_DACX578::commandWrite(uint8_t command, uint16_t value) {
  uint8_t shift_amount = 16 - _resolution_bits;
  value <<= shift_amount;

  uint8_t buffer[3];
  buffer[0] = command;
  buffer[1] = (value >> 8) & 0xFF;
  buffer[2] = value & 0xFF;
  return _i2c_dev->write(buffer, 3);
}

/*!
 * @brief Reads a 16-bit value from the DAC after issuing a command.
 * @param command The command to send (pre-shifted upper nibble with channel
 * info).
 * @param value Pointer to store the read value, shifted down based on the
 * resolution.
 * @return True if the read was successful, otherwise false.
 */
bool Adafruit_DACX578::commandRead(uint8_t command, uint16_t *value) {
  uint8_t buffer[2];

  if (!_i2c_dev->write_then_read(&command, 1, buffer, 2)) {
    return false;
  }

  *value = (((uint16_t)buffer[0] << 8) | (uint16_t)buffer[1]) >>
           (16 - _resolution_bits);
  return true;
}
