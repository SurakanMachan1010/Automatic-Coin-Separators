#include "RC522G4.h"
#include <MFRC522.h>

MFRC522 mfrc522;

RC522G4::RC522G4(uint8_t chipSelectPin, uint8_t resetPin) {
  _chipSelectPin = chipSelectPin;
  _resetPin = resetPin;
}

void RC522G4::begin() {
  SPI.begin();
  mfrc522.PCD_Init(_chipSelectPin, _resetPin);
}

void RC522G4::readUID() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Read the UID of the card
    Serial.print("UID Length: ");Serial.print(mfrc522.uid.size);
    Serial.println(" ");
    Serial.print("UID Value: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    mfrc522.PICC_HaltA();
  }
}

bool RC522G4::checkCardUID(byte byte0, byte byte1, byte byte2, byte byte3) {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // อ่าน UID ของการ์ด
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] != (i == 0 ? byte0 : i == 1 ? byte1 : i == 2 ? byte2 : byte3)) {
        // ถ้า UID ไม่ตรงกับที่กำหนด ให้หยุดการสื่อสารกับการ์ดและคืนค่า false
        mfrc522.PICC_HaltA();
        return false;
      }
    }
    // ถ้า UID ตรงกับที่กำหนด ให้หยุดการสื่อสารกับการ์ดและคืนค่า true
    mfrc522.PICC_HaltA();
    return true;
  }
  // ถ้าไม่มีการ์ดใหม่หรือการอ่านไม่สำเร็จ ให้คืนค่า false
  return false;
}



