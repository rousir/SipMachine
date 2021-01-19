#include <Arduino.h>
#include "WiFi.h"
#include "SipMachine.h"
#include "Debug.h"

#include <SPIFFS.h>
#include <SD.h>

#include "driver/i2s.h"

const char *ssid = "b209";            // your network SSID (name)
const char *password = "bao12345678"; // your network password (use for WPA, or use as key for WEP)

String telNr = "10013";
String serverIp = "192.168.31.21";

SipMachine sipMachine = SipMachine("10013", "1234", telNr, serverIp + String(":5060"), serverIp); //esp

unsigned long t2 = millis();
unsigned long t4 = micros();
unsigned long t5 = millis();

SipMachine::Status status;
int16_t pcmOut = 0;
int16_t pcmIn;

#define I2S_NUM_TX I2S_NUM_0
#define I2S_NUM_RX I2S_NUM_1

#define PIN_I2S_BCLK    26
#define PIN_I2S_LRC     27
#define PIN_I2S_DIN     34
#define PIN_I2S_DOUT    25

#define PIN_CARDCS        22     // Card chip select pin

//format bytes
String formatBytes(size_t bytes) {
    if (bytes < 1024) {
        return String(bytes) + "B";
    } else if (bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

void printFileList(fs::File fd){

}

void I2S_Init(i2s_mode_t MODE, i2s_bits_per_sample_t BPS, i2s_port_t I2S_NUM, int rate) {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | MODE),
        .sample_rate = rate,
        .bits_per_sample = BPS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = 0,
        .dma_buf_count = 16,
        .dma_buf_len = 60
    };

    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = PIN_I2S_BCLK;
    pin_config.ws_io_num = PIN_I2S_LRC;
    if (MODE == I2S_MODE_RX) {
        pin_config.data_out_num = I2S_PIN_NO_CHANGE;
        pin_config.data_in_num = PIN_I2S_DIN;
    }
    else if (MODE == I2S_MODE_TX) {
        pin_config.data_out_num = PIN_I2S_DOUT;
        pin_config.data_in_num = I2S_PIN_NO_CHANGE;
    }

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);

    // i2s_set_clk(I2S_NUM, rate, BPS, I2S_CHANNEL_STEREO);
}

void wifiEvent(WiFiEvent_t event) {
    switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WIFISTA Connected");
        Serial.print("WIFISTA MAC: ");
        Serial.print(WiFi.macAddress());
        Serial.print(", IPv4: ");
        Serial.println(WiFi.localIP());
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WIFISTA Disconnected");
        WiFi.reconnect();
        break;
    default:
        break;
    }
}

void connectToNetwork()
{
    WiFi.onEvent(wifiEvent);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.println(String("connection to WiFi ") + String(ssid));
    Serial.println(String("password: ") + String(password));
}

void sipMachineEvent(int code) {
    switch (code) {
    case 0:
        
        break;

    case 1:
        
        break;
    default:
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    if (SPIFFS.begin())
    {
        Serial.println("SPIFFS begin success");

        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        while (file) {
            String fileName = file.name();
            size_t fileSize = file.size();
            Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
            file = root.openNextFile();
        }
        Serial.printf("\n");
    } else {
        Serial.println("SPIFFS begin failed");
        SPIFFS.format();
    }

    if (SD.begin(PIN_CARDCS))
    {
        Serial.println("SD begin success");

        File root = SD.open("/");
        File file = root.openNextFile();
        while (file) {
            String fileName = file.name();
            size_t fileSize = file.size();
            Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
            file = root.openNextFile();
        }
        Serial.printf("\n");
    } else {
        Serial.println("SD begin failed");
    }

    connectToNetwork();

    sipMachine.setup(WiFi.localIP().toString(), serverIp);
    sipMachine.setEvent(sipMachineEvent);

    I2S_Init(I2S_MODE_TX, I2S_BITS_PER_SAMPLE_16BIT, I2S_NUM_TX, 8000);
    // I2S_Init(I2S_MODE_RX, I2S_BITS_PER_SAMPLE_16BIT, I2S_NUM_RX, 8000);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {

        pcmIn = sipMachine.loop(pcmOut);
        pcmOut = 0;
        status = sipMachine.getStatus();

        switch (status)
        {
        //待机状态
        case SipMachine::idle:
            // if ((t2 + 5000 < millis()) & (t2 + 6000 > millis())) {
            //     //拨打电话
            //     sipMachine.makeCall("10012");
            // }
            break;

        //接到电话，响铃
        case SipMachine::ringIn:
            Serial.println(String("Ringing Call Nr. ") + sipMachine.getTelNrIncomingCall());

            delay(1000);

            Serial.println(String("Accept incoming Call ") + sipMachine.getTelNrIncomingCall());
            //接听电话
            sipMachine.acceptIncomingCall();
            break;

        //拨出电话，响铃
        case SipMachine::ringOut:
            break;

        //通话中
        case SipMachine::call:
            // pcmOut = pcmIn;

            // if ((t5 + 20000) < millis())
            // {
            //     t5 += 20000;
            //     sipMachine.bye();
            //     fin.close();

            //     Serial.printf("bye bye\n");

            // }

            // if (t4 < micros())
            // {
            //     t4 += 125;

            //     if (pcmIn == (688))
            //     {
            //         pcmIn = 0;
            //     }
            //     if (pcmIn > maxP)
            //     {
            //         maxP = pcmIn;
            //     }
            //     if (pcmIn < minP)
            //     {
            //         minP = pcmIn;
            //     }
            //     fin.write((uint8_t*)&pcmIn, 2);

            // }
            break;

        //收到短信
        case SipMachine::messageIn:
            Serial.println(String("Received message Nr. ") + sipMachine.getTelNrIncomingCall());
            Serial.println(String("Message:\n") + sipMachine.getMessageInData());

            delay(1000);

            //回复消息
            sipMachine.sendMessage(sipMachine.getTelNrIncomingCall(), "Hello, I amnowsomethingis not,onewillcontact you.");
            break;

        default:
            ;    
            break;
        }
    }
}

