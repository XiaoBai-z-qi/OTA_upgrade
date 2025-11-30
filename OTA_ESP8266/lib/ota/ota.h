#ifndef __OTA_H__
#define __OTA_H__
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define BIN_CHUNK_SIZE 1024
void WiFiInit();
void readconfig(uint8_t* hversion, uint8_t* lversion, uint8_t* h_kb, uint8_t* l_kb);
void readbin(uint8_t *buff, uint16_t count);
#endif