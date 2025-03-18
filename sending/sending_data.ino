#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
char receivedData[32] = {0};
char esp32Response[32] = {0};
bool isSynced = false;

void setup() {
    Serial.begin(115200);
    delay(500);
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
    radio.stopListening();

    Serial.println("Arduino: Sending SYNC...");
    const char syncMsg[] = "SYNC";
    radio.write(&syncMsg, sizeof(syncMsg));

    radio.startListening();
    unsigned long startTime = millis();
    while (millis() - startTime < 100) { 
        if (radio.available()) {
            radio.read(&receivedData, sizeof(receivedData));
            if (strcmp(receivedData, "READY") == 0) {
                isSynced = true;
                Serial.println("Arduino: ESP32 is READY!");
                break;
            }
        }
    }
    radio.stopListening();
}

void loop() {

    if (Serial.available()) {
        char sendData[32] = {};
        Serial.readBytesUntil('\n', sendData, 31);
        Serial.print("Received from Python: ");
        Serial.println(sendData);


        radio.stopListening();
        bool success = radio.write(&sendData, strlen(sendData) + 1);
        if (success) {
//            Serial.println("Arduino: Data sent to ESP32!");
        } else {
//            Serial.println("Arduino: RF24 send to ESP32 failed!");
        }
        radio.startListening();

        unsigned long startTime = millis();
        bool receivedResponse = false;
        while (millis() - startTime < 100) {
            if (radio.available()) {
                radio.read(&esp32Response, sizeof(esp32Response));
//                Serial.print("Received from ESP32: ");
//                Serial.println(esp32Response);
                receivedResponse = true;
                break;
            }
        }

        if (!receivedResponse) {
//            Serial.println("Arduino: No response from ESP32!");
        } else {
//            Serial.print("Sending to Python: ");
            Serial.println(esp32Response);
            delay(50); 
        }
    }

    delay(100);
}