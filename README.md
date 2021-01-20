# SipMachine
Arduino SIP Client library

Arduino sip客户端，测试了esp32，使用tcp连接，已完成接听拨打电话，收发消息等功能

使用i2s连接麦克风和喇叭，目前实现了RTP G.711A(PCMA)音频协议，音频8k采样率，16bit

## 尚未解决的问题

esp32 i2s两个通道不能同时使用，只能录音或者播放声音，目前解决办法是默认播放声音，按下按键进入录音模式，说完松开按键切到播放


# 感谢
https://github.com/RetepRelleum/SipMachine 库基于RetepRelleum SipMachine库修改