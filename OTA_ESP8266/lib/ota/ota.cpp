#include "ota.h"
const char* ssid = "Xiaomi8266";
const char* password = "123456789";

const char* configUrl = "http://192.168.212.161:8000/config.json";
const char* binUrl    = "http://192.168.212.161:8000/app.bin";
const char* crcUrl    = "http://192.168.212.161:8000/crc.bin";


void WiFiInit()
{
    WiFi.begin(ssid, password);
    while (WiFi.status()!= WL_CONNECTED) 
    {
    delay(200);
    }
}


void readconfig(uint8_t* hversion, uint8_t* lversion, uint8_t* h_kb, uint8_t* l_kb)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;      //创建WiFi客户端
        HTTPClient http;        //创建HTTP请求对象

        http.begin(client, configUrl);   //开始HTTP请求
        int httpCode = http.GET();       //发送HTTP GET请求

        if(httpCode == HTTP_CODE_OK)     //请求成功
        {
            String payload = http.getString();   //获取HTTP响应内容
            StaticJsonDocument<128> doc;

            DeserializationError error = deserializeJson(doc, payload);   //解析JSON数据
            if(!error)
            {
                const char* version = doc["version"];
                int kb = doc["kb"];

                uint8_t major = 0;
                uint8_t minor = 0;

                // 使用 sscanf 拆分数字
                sscanf(version, "%d.%d", &major, &minor);
                *hversion = major;
                *lversion = minor;
                *h_kb = (kb >> 8) & 0xff;
                *l_kb = kb & 0xff;
            }else{
                *hversion = 0;
                *lversion = 0;
                *h_kb = 0;
                *l_kb = 0;
            }
        }

        http.end();   //关闭HTTP请求
    }
}