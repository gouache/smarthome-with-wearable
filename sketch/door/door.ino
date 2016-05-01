#include <SoftwareSerial.h>

#include "ESP8266.h"

#define SSID AP_SSID
#define PASS AP_PASS

SoftwareSerial esp8266Serial = SoftwareSerial(11, 12);
ESP8266 wifi = ESP8266(esp8266Serial);
int i;

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

    // setMultipleConnections
    Serial.print("setMultipleConnections: ");
    Serial.println(getStatus(wifi.setMultipleConnections(true)));

    Serial.print("createServer: ");
    Serial.println(getStatus(wifi.createServer(9999)));
}

void loop()
{
    // read data
    unsigned int id;
    int length;
    int totalRead;
    char buffer[512] = {};

    if ((length = wifi.available()) > 0) {
        id = wifi.getId();

        totalRead = wifi.read(buffer, 512);

        if (length > 0) {
            Serial.print("Received ");
            Serial.print(totalRead);
            Serial.print("/");
            Serial.print(length);
            Serial.print(" bytes from client ");
            Serial.print(id);
            Serial.print(": ");
            Serial.println((char*)buffer);
            if (buffer[0] == '!') {
                // COSTEL Multimedia Home System
                // turn on screen
                irsend.sendNEC(0x4100E817, 32);
                delay(5000);
                // open the door
                irsend.sendNEC(0x41006897, 32);
            }
        }
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
