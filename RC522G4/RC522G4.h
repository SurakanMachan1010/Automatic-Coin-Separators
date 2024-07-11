#ifndef RC522G4_h
#define RC522G4_h

#include <Arduino.h>
#include <SPI.h>

class RC522G4 {
public:
  RC522G4(uint8_t chipSelectPin, uint8_t resetPin);
  void begin();
  void readUID();
  bool checkCardUID(byte byte0, byte byte1, byte byte2, byte byte3);
private:
  uint8_t _chipSelectPin;
  uint8_t _resetPin;
};

#endif
