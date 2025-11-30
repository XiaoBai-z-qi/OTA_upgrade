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
        WiFiClient client;
        HTTPClient http;
        http.begin(client, configUrl); // ← 修复API

        int httpCode = http.GET();
        if(httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();

            StaticJsonDocument<128> doc;
            if(!deserializeJson(doc, payload))
            {
                const char* version = doc["version"];
                int kb = doc["kb"];

                int major = 0, minor = 0;
                sscanf(version, "%d.%d", &major, &minor);

                *hversion = major;
                *lversion = minor;
                *h_kb = (kb >> 8) & 0xFF;
                *l_kb = kb & 0xFF;
            }
            else
            {
                *hversion = *lversion = 0;
                *h_kb = *l_kb = 0;
            }
        }
        http.end();
    }
}


void readbin(uint8_t *buff, uint16_t count)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        HTTPClient http;

        int start = count * BIN_CHUNK_SIZE;
        int end = start + BIN_CHUNK_SIZE - 1;
        String rangeHeader = "bytes=" + String(start) + "-" + String(end);

        http.begin(client, binUrl); // ← 修复API
        http.addHeader("Range", rangeHeader);
        
        int httpCode = http.GET();
        if(httpCode > 0)
        {
            WiFiClient *stream = http.getStreamPtr();
            int len = http.getSize();
            int totalRead = 0;

            while(http.connected() && totalRead < len)
            {
                int available = stream->available();
                if(available)
                {
                    totalRead += stream->readBytes(buff + totalRead, available);
                }
                delay(1);
            }
        }
        http.end();
    }
}


// void readconfig(uint8_t* hversion, uint8_t* lversion, uint8_t* h_kb, uint8_t* l_kb)
// {
//     if(WiFi.status() == WL_CONNECTED)
//     {
//         // HTTPS请求客户端（不校验证书，直接允许访问）
//         WiFiClientSecure client;
//         client.setInsecure();

//         HTTPClient http;
//         http.begin(client, configUrl);   // 向 configUrl 发起 HTTP(S) 请求

//         int httpCode = http.GET();       // 执行GET
//         if(httpCode == HTTP_CODE_OK)     // 请求成功
//         {
//             String payload = http.getString();  // 获取 JSON 字符串数据

//             StaticJsonDocument<128> doc;
//             // 反序列化 JSON 字符串 -> JSON 对象
//             DeserializationError error = deserializeJson(doc, payload);

//             if(!error)
//             {
//                 // 从 JSON 中提取字段
//                 const char* version = doc["version"];  // 版本号格式："1.5"
//                 int kb = doc["kb"];                   // 固件大小 (单位: KB) 如 32

//                 int major = 0;
//                 int minor = 0;
//                 // 将字符串版本号拆成数字
//                 sscanf(version, "%d.%d", &major, &minor);

//                 // 赋值给输出指针
//                 *hversion = major;    // 主版本号
//                 *lversion = minor;    // 次版本号
//                 *h_kb = (kb >> 8) & 0xFF; // KB 高字节
//                 *l_kb = kb & 0xFF;        // KB 低字节
//             }
//             else
//             {
//                 // JSON解析失败则返回0
//                 *hversion = *lversion = 0;
//                 *h_kb = *l_kb = 0;
//             }
//         }
//         http.end(); // 关闭连接
//     }
// }


// void readbin(uint8_t *buff, uint16_t count)
// {
//     if(WiFi.status() == WL_CONNECTED)
//     {
//         WiFiClientSecure client;
//         client.setInsecure();   
//         HTTPClient http;

//         int start = count * BIN_CHUNK_SIZE;
//         int end   = start + BIN_CHUNK_SIZE - 1;
//         String rangeHeader = "bytes=" + String(start) + "-" + String(end);

//         http.begin(client, binUrl);
//         http.addHeader("Range", rangeHeader);

//         int httpCode = http.GET();

//         if(httpCode > 0)
//         {
//             WiFiClient *stream = http.getStreamPtr();
//             int len = http.getSize();

//             int totalRead = 0;
//             while (http.connected() && totalRead < len)
//             {
//                 int available = stream->available();
//                 if (available)
//                 {
//                     int readNow = stream->readBytes(buff + totalRead, available);
//                     totalRead += readNow;
//                 }
//                 delay(1);
//             }
//         }
//         http.end();
//     }
// }