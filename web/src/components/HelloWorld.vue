<template>
  <div>
    <div class="main-button" @click="open">OPEN</div>
  </div>
</template>

<script>
import mqtt from 'mqtt'
export default {
  name: 'HelloWorld',
  props: {
    msg: String
  },
  created(){
    this.initMqtt()
  },
  methods: {
    initMqtt(){
      // 连接选项
      const options = {
            clean: true, // 保留回话
            connectTimeout: 4000, // 超时时间
            keepalive: 60,
            maximumPacketSize: 1024,
            // 认证信息
            clientId: '0D3FCD|securemode=3,signmethod=hmacsha1|',
            username: 'esp8266_switch_door&a1fwIZlerKO',
            password: '80514E038C8490DD57F6BFA6FB7F8558D6608CBE',
      }

      // 连接字符串, 通过协议指定使用的连接方式
      // ws 未加密 WebSocket 连接
      // wss 加密 WebSocket 连接
      // mqtt 未加密 TCP 连接
      // mqtts 加密 TCP 连接
      // wxs 微信小程序连接
      // alis 支付宝小程序连接
      const connectUrl = 'wss://a1fwIZlerKO.iot-as-mqtt.cn-shanghai.aliyuncs.com/sys/a1fwIZlerKO/esp8266_switch_door/thing/event/property/post'
      const client = mqtt.connect(connectUrl, options)

      // client.on('reconnect', (error) => {
      //     console.log('正在重连:', error)
      // })

      client.on('error', (error) => {
          console.log('连接失败:', error)
      })

      client.on('connect', function () {
        client.subscribe('presence', function (err) {
            if (!err) {
              client.publish('presence', 'Hello mqtt')
            }
          })
        })

      client.on('message', (topic, message) => {
        console.log('收到消息：', topic, message.toString())
      })
    },
    open(){
      client.publish('presence', 'Hello mqtt')
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.main-button{
  width: 50px;
  height: 50px;
  line-height: 50px;
  margin: 0 auto;
  cursor: pointer;
  border: 1px solid #333;
  border-radius: 50%;
  box-shadow: 0px 0px 8px 1px #888888;
}
.main-button:hover{
  background: rgb(170, 170, 170);
  box-shadow: 0px 0px 10px 1px #888888;
}
</style>
