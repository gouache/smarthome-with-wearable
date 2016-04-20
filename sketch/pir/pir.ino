#include <SoftwareSerial.h>

#include "ESP8266.h"

#define SSID AP_SSID
#define PASS AP_PASS

#define DST_PORT 8080
#define PIR_PIN 7

SoftwareSerial esp8266Serial = SoftwareSerial(11, 12);
ESP8266 wifi = ESP8266(esp8266Serial);

void setup()
{
    Serial.begin(9600);

    // ESP8266
    esp8266Serial.begin(9600);
    wifi.begin();
    wifi.setTimeout(1000);

    // test
    Serial.print("test: ");
    Serial.println(getStatus(wifi.test()));

    // restart
    Serial.print("restart: ");
    Serial.println(getStatus(wifi.restart()));


    // getWifiMode
    ESP8266WifiMode mode;
    Serial.print("getWifiMode: ");
    Serial.println(getStatus(wifi.getMode(&mode)));
    Serial.print("Wifi mode>>:");
    Serial.println(mode);

    // setWifiMode
    if(mode != ESP8266_WIFI_STATION) {
        Serial.print("setWifiMode: ");
        Serial.println(getStatus(wifi.setMode(ESP8266_WIFI_STATION )));
    } else
        Serial.println("already ESP8266_WIFI_STATION mode");

    // getVersion
    char version[16] = {};
    Serial.print("getVersion: ");
    Serial.print(getStatus(wifi.getVersion(version, 16)));
    Serial.print(" : ");
    Serial.println(version);

    // joinAP
    Serial.print("joinAP: ");
    Serial.println(getStatus(wifi.joinAP(SSID, PASS)));
}

void loop()
{
    int totalRead;
    char buffer[512] = {};

    motionVal = digitalRead(PIR_PIN);
    if (motionVal == LOW) {
      Serial.println("No motion");
      delay(500);
      return;
    } else {
      Serial.println("Motion!");

      t1 = now();
      if (start == 0) {
        t2 = t1;
        Serial.print("connect: ");
        Serial.println(getStatus(wifi.connect(ESP8266_PROTOCOL_TCP, DST_IP, DST_PORT)));
        delay(50);

        Serial.print("send: ");
        Serial.println(getStatus(wifi.send("GET /cctv HTTP/1.0\r\n\r\n")));
        totalRead = wifi.read(buffer, 512);
        Serial.print("close: ");
        Serial.println(getStatus(wifi.close(5)));
      }

      if (t1 < t2+5) {
        start = 1;
      } else {
        start = 0;
      }
      delay(500);
      return;
    }

    delay(500);
}

String getStatus(bool status)
{
    if (status)
        return "OK";

    return "KO";
}

String getStatus(ESP8266CommandStatus status)
{
    switch (status) {
    case ESP8266_COMMAND_INVALID:
        return "INVALID";
        break;

    case ESP8266_COMMAND_TIMEOUT:
        return "TIMEOUT";
        break;

    case ESP8266_COMMAND_OK:
        return "OK";
        break;

    case ESP8266_COMMAND_NO_CHANGE:
        return "NO CHANGE";
        break;

    case ESP8266_COMMAND_ERROR:
        return "ERROR";
        break;

    case ESP8266_COMMAND_NO_LINK:
        return "NO LINK";
        break;

    case ESP8266_COMMAND_TOO_LONG:
        return "TOO LONG";
        break;

    case ESP8266_COMMAND_FAIL:
        return "FAIL";
        break;

    default:
        return "UNKNOWN COMMAND STATUS";
        break;
    }
}

String getRole(ESP8266Role role)
{
    switch (role) {
    case ESP8266_ROLE_CLIENT:
        return "CLIENT";
        break;

    case ESP8266_ROLE_SERVER:
        return "SERVER";
        break;

    default:
        return "UNKNOWN ROLE";
        break;
    }
}

String getProtocol(ESP8266Protocol protocol)
{
    switch (protocol) {
    case ESP8266_PROTOCOL_TCP:
        return "TCP";
        break;

    case ESP8266_PROTOCOL_UDP:
        return "UDP";
        break;

    default:
        return "UNKNOWN PROTOCOL";
        break;
    }
}
