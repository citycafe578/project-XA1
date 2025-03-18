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
        char sendData[32] = {0};  // 初始化為 0
        Serial.readBytesUntil('\n', sendData, 31);
        Serial.print("Received from Serial: ");
        Serial.println(sendData);

        radio.stopListening();
        bool success = radio.write(&sendData, strlen(sendData) + 1);  // 只發送實際字串
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
<<<<<<< HEAD
        Serial.write(receivedData);
=======
>>>>>>> 6329b492508920d4d8f1800f2e73e574061464f0

        const char ackMsg[] = "ACK";
        radio.stopListening();
        bool success = radio.write(&ackMsg, strlen(ackMsg) + 1);
        if (success) {
            Serial.println("Arduino: ACK sent!");
        } else {
            Serial.println("Arduino: ACK failed!");
        }
        radio.startListening();
    }

    delay(100);
}
