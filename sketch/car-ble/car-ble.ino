#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8); // RX, TX
// Connect HM10      Arduino Uno
//     Pin 1/TXD          Pin 7
//     Pin 2/RXD          Pin 8

void setup() {
    Serial.begin(9600);
    // AT+BAUD0 for 9600 bauds
    mySerial.begin(9600);
}

void loop() {
    char c;
    if (Serial.available()) {
       c = Serial.read();
       mySerial.print(c);
    }

    if (mySerial.available()) {
        c = mySerial.read();
        Serial.print(c);
    }
}
