#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
char receivedData[32] = {0}; 

void setup() {
    Serial.begin(9600);
    Serial.println("Arduino Initialized!");

    if (!radio.begin()) {
        Serial.println("Arduino: Radio hardware is not responding!");
        while (1);
    }

    radio.setPALevel(RF24_PA_HIGH);
    radio.setDataRate(RF24_2MBPS);
    radio.setChannel(100);
    radio.openWritingPipe(address);
    radio.openReadingPipe(1, address);
    radio.startListening();
}

void loop() {
    if (Serial.available()) {
        char sendData[32] = {};
        Serial.readBytesUntil('\n', sendData, 31);
        Serial.print("Received from Serial: ");
        Serial.println(sendData);

        radio.stopListening();
        bool success = radio.write(&sendData, sizeof(sendData));
        if (success) {
            Serial.println("Arduino: Data sent via RF24!");
        } else {
            Serial.println("Arduino: RF24 send failed!");
        }
        radio.startListening();
    }
    if (radio.available()) {
        radio.read(&receivedData, sizeof(receivedData));
        Serial.print("Received from RF24: ");
        Serial.println(receivedData);

        const char ackMsg[] = "ACK";
        radio.stopListening();
        radio.write(&ackMsg, sizeof(ackMsg));
        Serial.println("Arduino: ACK sent!");
        radio.startListening();
    }

    delay(100);
}