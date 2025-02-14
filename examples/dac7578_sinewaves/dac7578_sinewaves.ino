#include <Adafruit_DACX578.h>
#include <math.h>

Adafruit_DACX578 dac(12); // Assuming 12-bit resolution

const float frequencies[8] = {10, 20, 40, 80, 160, 320, 640, 1280}; // Hz
const uint16_t amplitude =
    2048;                     // Half of full scale for 12-bit DAC (0 to 4095)
const uint16_t offset = 2048; // DC offset to keep sine wave positive
const uint32_t sampleRate = 10000; // 10 kHz sample rate for smooth output
const uint32_t period =
    1000000 / sampleRate; // Period in microseconds per sample

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit DACX578 Sine Wave Test");

  if (!dac.begin()) {
    Serial.println("Failed to find DAC7578 chip");
    while (1)
      delay(10);
  }

  Serial.println("DAC7578 initialized");

  // Set I2C frequency to 800 kHz for faster communication
  Wire.setClock(800000);
}

void loop() {
  static uint32_t lastTime = micros();
  static float phase[8] = {0};

  if (micros() - lastTime >= period) {
    lastTime += period;

    for (uint8_t channel = 0; channel < 8; channel++) {
      // Compute the sine wave value
      float sineValue = sin(phase[channel]) * amplitude + offset;
      dac.writeAndUpdateChannelValue(channel, (uint16_t)sineValue);

      // Update phase for the next sample
      phase[channel] += 2 * M_PI * frequencies[channel] / sampleRate;
      if (phase[channel] >= 2 * M_PI) {
        phase[channel] -= 2 * M_PI;
      }
    }
  }
}
