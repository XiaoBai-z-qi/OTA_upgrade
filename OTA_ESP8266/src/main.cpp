#include <Arduino.h>
#include "ota.h"


uint8_t binBuff[BIN_CHUNK_SIZE] = {0};
void setup() {
  
  Serial.begin(460800);
  WiFiInit();

}
uint8_t kb[2] = {0};
uint8_t version[2] = {0};
void loop() {

  // Serial.println("Waiting for command");
  // readconfig(&version[0], &version[1], &kb[0], &kb[1]);
  // Serial.printf("version: %d.%d kb\n", version[0], version[1]);

  // delay(5000);

  if(Serial.available() > 0)
  {
    static uint16_t count = 0;
    uint8_t recvbyte = Serial.read();
    uint8_t version[2] = {0};
    uint8_t kb[2] = {0};
    switch(recvbyte)
    {
      case 0x80:
        
        readconfig(&version[0], &version[1], &kb[0], &kb[1]);
        Serial.write(version,2);
        Serial.write(kb,2);
      break;

      case 0x79:
        readbin(binBuff, count);
        Serial.write(binBuff, BIN_CHUNK_SIZE);
        count++;
      break;

      case 0x6f:
        count = 0;
      break;
    }
    
  }
}


// void printHexFormat(uint8_t* data, uint16_t len)
// {
//     for(uint16_t i = 0; i < len; i++)
//     {
//         Serial.printf("%02X ", data[i]);
//         if((i + 1) % 16 == 0) Serial.println(); // 每16个换行
//     }
//     Serial.println();
// }