/*!
 *  @file Adafruit_DACX578.h
 *  @brief Library for DACX578 series DACs
 */

#ifndef _ADAFRUIT_DACX578_H_
#define _ADAFRUIT_DACX578_H_

#include <Adafruit_I2CDevice.h>
#include <Arduino.h>

/*!< Pre-shifted command defines (upper nibble) */
#define DACX578_CMD_WRITE (0x0 << 4)  ///< Write command
#define DACX578_CMD_UPDATE (0x1 << 4) ///< Update command
#define DACX578_CMD_WRITE_GLOBAL_UPDATE                                        \
  (0x2 << 4)                                ///< Write and global update command
#define DACX578_CMD_WRITE_UPDATE (0x3 << 4) ///< Write and update command
#define DACX578_CMD_POWERDOWN (0x4 << 4)    ///< Power-down command
#define DACX578_CMD_RESET (0x5 << 4)        ///< Reset command
#define DACX578_CMD_LDAC_MASK (0x6 << 4)    ///< LDAC mask command
#define DACX578_CMD_INTERNAL_REF                                               \
  (0x7 << 4) ///< Internal reference enable command

/*!< Broadcast channel for DACX578 */
#define DACX578_CHANNEL_BROADCAST 0xF ///< Broadcast to all channels

/*!< Default I2C address for DACX578 */
#define DACX578_DEFAULT_I2C_ADDR 0x47 ///< Default I2C address

/*!
 * @brief Enum for DAC clear code register options.
 */
typedef enum {
  DACX578_CLEAR_CODE_ZERO = 0, ///< Clear to 0x000
  DACX578_CLEAR_CODE_MID = 1,  ///< Clear to midscale (0x800 for 12-bit)
  DACX578_CLEAR_CODE_FULL = 2, ///< Clear to full scale (0xFFF for 12-bit)
  DACX578_CLEAR_CODE_NOP = 3   ///< No operation (retain current value)
} dacx578_clear_code_t;

/*!
 *  @brief  Class that stores state and functions for interacting with DACX578
 */
class Adafruit_DACX578 {
public:
  Adafruit_DACX578(uint8_t resolution = 8); // Add resolution parameter
  ~Adafruit_DACX578();

  bool begin(uint8_t i2c_addr = DACX578_DEFAULT_I2C_ADDR,
             TwoWire *wire = &Wire);
  bool reset(void);

  bool writeChannelValue(uint8_t channel, uint16_t value);
  bool updateChannel(uint8_t channel);
  bool writeAndUpdateChannelValue(uint8_t channel, uint16_t value);
  bool writeAndGlobalUpdateChannelValue(uint8_t channel, uint16_t value);
  bool readInputChannelValue(uint8_t channel, uint16_t *value);
  bool readChannelValue(uint8_t channel, uint16_t *value);
  bool writeLDAC(uint8_t ldac_mask);
  uint8_t readLDAC(void);
  bool setClearCode(dacx578_clear_code_t clear_code);
  dacx578_clear_code_t getClearCode(void);

private:
  bool commandWrite(uint8_t command, uint16_t value);
  bool commandRead(uint8_t command, uint16_t *value);
  Adafruit_I2CDevice *_i2c_dev = nullptr;
  uint8_t _i2c_addr;
  TwoWire *_wire;
  uint8_t _resolution_bits;
};

#endif // _ADAFRUIT_DACX578_H_
