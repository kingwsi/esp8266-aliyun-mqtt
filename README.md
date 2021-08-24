# esp8266-aliyun-mqtt
ESP8266 MQTT(aliyun)通信，使用ESP-01S验证过
搬运整合的
## 使用&&相关资源
阿里云iot控制台 https://iot.console.aliyun.com/
测试使用 一键生成接入信息 https://x223222981.gitee.io/aliyun_mqttpassword_get/

#### 需要安装的库
PubSubClient
ArduinoJson
#### 需要修改的地方
找到`PubSubClient.h`文件
- 将`MQTT_MAX_PACKET_SIZE` 改为 `MQTT_MAX_PACKET_SIZE 1024`
- 将`MQTT_KEEPALIVE` 改为 `MQTT_KEEPALIVE 60`

