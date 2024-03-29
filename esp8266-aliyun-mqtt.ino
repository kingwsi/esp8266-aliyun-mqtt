#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "aliyun_mqtt.h"

// hashlib, ArduinoJson, ArduinoJson
// 本案例使用esp-01s，对饮引脚为 0 2
#define OUTPUT_PIN 0
#define STATUS_LED_PIN 2

#define WIFI_SSID ""  //替换自己的WIFI
#define WIFI_PASSWD "" //替换自己的WIFI

#define PRODUCT_KEY ""   //替换自己的PRODUCT_KEY
#define DEVICE_NAME ""   //替换自己的DEVICE_NAME
#define DEVICE_SECRET "" //替换自己的DEVICE_SECRET

#define DEV_VERSION "S-TH-WIFI-v1.0-34487" //固件版本信息

#define ALINK_BODY_FORMAT "{\"id\":\"123\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_PROP_POSTRSP "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST "thing.event.property.post"
#define ALINK_TOPIC_DEV_INFO "/ota/device/inform/" PRODUCT_KEY "/" DEVICE_NAME ""
#define ALINK_VERSION_FROMA "{\"id\": 123,\"params\": {\"version\": \"%s\"}}"
unsigned long lastMs = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void init_wifi(const char *ssid, const char *password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi does not connect, try again ...");
        delay(500);
    }

    Serial.println("Wifi is connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// 监听订阅消息
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    payload[length] = '\0';
    Serial.println((char *)payload);
    if (strstr(topic, ALINK_TOPIC_PROP_SET))
    {
        StaticJsonBuffer<100> jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(payload);
        if (!root.success())
        {
            Serial.println("parseObject() failed");
            return;
        }
        int powerSwitch = root["params"]["PowerSwitch"];
        digitalWrite(STATUS_LED_PIN, powerSwitch);
        digitalWrite(OUTPUT_PIN, powerSwitch);
    }
}
// 版本信息上传
void mqtt_version_post()
{
    char param[512];
    char jsonBuf[1024];

    sprintf(param, "{\"id\": 123,\"params\": {\"version\": \"%s\"}}", DEV_VERSION);
    Serial.println(param);
    mqttClient.publish(ALINK_TOPIC_DEV_INFO, param);
}

// mqtt订阅检测
void mqtt_check_connect()
{
    while (!mqttClient.connected())
    {
        while (connect_aliyun_mqtt(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
        {
            Serial.println("MQTT connect succeed!");
            // 订阅
            mqttClient.subscribe(ALINK_TOPIC_PROP_SET);
            Serial.println("subscribe done");
            mqtt_version_post();
        }
    }
}

// 推送mqtt消息 上报设备状态 json
void mqtt_interval_post()
{
    char param[512];
    char jsonBuf[1024];
    // sprintf(param, "{\"CurrentHumidity\":%d,\"CurrentTemperature\":12,\"GeoLocation\":{\"CoordinateSystem\":2,\"Latitude\":2,\"Longitude\":123,\"Altitude\":1}}", digitalRead(13));
    sprintf(param, "{\"PowerSwitch\":%d}", digitalRead(OUTPUT_PIN));
    sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
    Serial.println(jsonBuf);
    mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
}

void setup()
{
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(OUTPUT_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH);
    digitalWrite(OUTPUT_PIN, HIGH);
    /* initialize serial for debugging */
    Serial.begin(115200);

    Serial.println("Start...");

    init_wifi(WIFI_SSID, WIFI_PASSWD);

    mqttClient.setCallback(mqtt_callback);
}

// the loop function runs over and over again forever
void loop()
{
    // 20秒检查一次
    if (millis() - lastMs >= 20000)
    {
        lastMs = millis();
        mqtt_check_connect();
        // mqtt_interval_post();
    }

    mqttClient.loop();

    unsigned int WAIT_MS = 1000;
    delay(WAIT_MS); // ms
    Serial.println(millis() / WAIT_MS);
}
