#include <Adafruit_DACX578.h>

Adafruit_DACX578 dac(12); // 12-bit resolution

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit DACX578 Sawtooth Wave Test");

  if (!dac.begin()) {
    Serial.println("Failed to find DAC7578 chip");
    while (1)
      delay(10);
  }

  Serial.println("DAC7578 initialized");

  // Set I2C frequency to 800 kHz for faster updates
  Wire.setClock(800000);
}

void loop() {
  for (uint16_t value = 0; value < 4096; value++) { // 12-bit range: 0 to 4095
    dac.writeAndUpdateChannelValue(0, value);       // Write to channel 0
  }
}
