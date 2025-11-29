#include <Arduino.h>
#include "ota.h"

void setup() {
  
  Serial.begin(460800);
  WiFiInit();

}

void loop() {
  if(Serial.available() > 0)
  {
    uint8_t recvbyte = Serial.read();
    switch(recvbyte)
    {
      case 0x80:
        uint8_t version[2] = {0};
        uint8_t kb[2] = {0};
        readconfig(&version[0], &version[1], &kb[0], &kb[1]);
        Serial.write(version,2);
        Serial.write(kb,2);
      break;
    }
  }
}

