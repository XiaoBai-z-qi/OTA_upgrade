#include <Arduino.h>
#include "ota.h"

void setup() {
  Serial.begin(460800);
  delay(1000);

  WiFiInit();
  
  readconfig();
}

void loop() {
  
}

