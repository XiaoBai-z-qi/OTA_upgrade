#ifndef __OTA_H__
#define __OTA_H__
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


void WiFiInit();
void readconfig();
#endif