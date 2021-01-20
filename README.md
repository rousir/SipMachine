# SipMachine
Arduino SIP Client library

Arduino sip客户端，测试了esp32，使用tcp连接，已完成接听拨打电话，收发消息等功能

使用i2s连接麦克风和喇叭，目前实现了RTP G.711A(PCMA)音频协议，音频8k采样率，16bit

## 使用方法
1.需要一个i2s麦克风和一个i2s喇叭

2.安装一个sip server，我使用的是[miniSIPServer](https://www.myvoipapp.com/)

3.安装sip server后，添加几个分机，设置不同的号码


4.电脑或者手机安装sip客户端，我使用的是[linphone](http://www.linphone.org/)

5.修改代码中的sip分机号码
```
String telNr = "10013";
String serverIp = "192.168.31.21";

String user = "10013";
String pwd  = "1234";

SipMachine sipMachine = SipMachine(user, pwd, telNr, serverIp + String(":5060"), serverIp);
```
6.使用linphone拨打esp32 sip


## 尚未解决的问题

esp32 i2s两个通道不能同时使用，只能录音或者播放声音，目前解决办法是默认播放声音，按下按键进入录音模式，说完松开按键切到播放


# 感谢
https://github.com/RetepRelleum/SipMachine 库基于RetepRelleum SipMachine库修改