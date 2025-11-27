#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Xiaomi8266";
const char* password = "123456789";

const char* versionClient = "http://192.168.212.161:8000/version.txt";
const char* binUrl = "http://192.168.212.161:8000/app.bin";

void readAppFirst2Bytes()
{
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, binUrl);

    // 请求前 2 字节：Range: bytes=0-1
    http.addHeader("Range", "bytes=1024-2047");

    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_PARTIAL_CONTENT || httpCode == HTTP_CODE_OK)
    {
      WiFiClient *stream = http.getStreamPtr();
      uint8_t buf[1024];
      int len = stream->readBytes(buf, 1024);

      if(len == 1024)
      {
        for(int i = 0; i < 1024; i++)
        {
          Serial.printf("0x%02X ",buf[i]);
        }
      }
      else
      {
        Serial.println("Read failed!");
      }
    }
    else
    {
      Serial.printf("HTTP GET failed: %d\n", httpCode);
    }
    http.end();
  }
}

void accessVersion()
{
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
    WiFiClient client;  // 新增

    http.begin(client, versionClient);

    int httpCode = http.GET();
    if(httpCode == 200)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.println("[HTTP] GET... failed, error");
    }
    http.end();
  }
}

void setup() {
  Serial.begin(460800);
  delay(1000);

  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
  }
  
  accessVersion();
  readAppFirst2Bytes();
}

void loop() {
  
}

