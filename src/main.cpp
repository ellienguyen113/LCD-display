#include <Arduino.h>
#include <Wire.h>
// SETTINGS
#define AS1115_ADDR      0x00   //  AS1115 I2C address
#define BRIGHTNESS       10     // 0...15

// Digit mapping 
#define TACH_START_DIG   0      // DIG0..DIG1 -> HDSP-523E (2 digits)
#define TACH_LEN         2
#define RPM_START_DIG    2     // DIG2..DIG5 -> COM-11405 (4 digits)
#define RPM_LEN          4


// If digits shown reversed order, set to 1
#define REVERSE_ORDER    0

// AS1115 REGISTERS 
#define REG_DIGIT0        0x01
#define REG_DECODE_MODE   0x09
#define REG_INTENSITY     0x0A   //set brightness
#define REG_SCAN_LIMIT    0x0B
#define REG_SHUTDOWN      0x0C
#define REG_DISPLAY_TEST  0x0F


// I2C WRITE
static void as1115_write(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(AS1115_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

static void as1115_init(void) {
  Wire.begin();

  as1115_write(REG_DISPLAY_TEST, 0x00);
  as1115_write(REG_SHUTDOWN, 0x01);          // normal operation
  as1115_write(REG_INTENSITY, (BRIGHTNESS & 0x0F));

  // Scan DIG0..DIG6
  as1115_write(REG_SCAN_LIMIT, 5);

  // Enable decode for digits 0..7 (all digits) 
  as1115_write(REG_DECODE_MODE, 0x3F);

  // Clear all digits to blank
  for (uint8_t i = 0; i < 8; i++) {
    as1115_write(REG_DIGIT0 + i, CODEB_BLANK);
  }
}

// Print number right-aligned in [start..start+len-1] using decode mode
static void as1115_printBlock(uint16_t value, uint8_t start, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    as1115_write(REG_DIGIT0 + start + i, CODEB_BLANK);
  }

  // write from rightmost digit of the block
  // start at the rightmost digit, then move to the left
  for (int pos = (int)len - 1; pos >= 0; pos--) {
    uint8_t digit = (uint8_t)(value % 10);
    as1115_write((uint8_t)(REG_DIGIT0 + start + pos), digit);
    value /= 10;
  }
}
