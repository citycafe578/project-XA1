#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
char receivedData[32] = {0};
bool isSynced = false;

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("Arduino Initialized!");

    if (!radio.begin()) {
        Serial.println("Arduino: Radio hardware is not responding!");
        while (1);
    }

    radio.setChannel(100);
    radio.setDataRate(RF24_250KBPS);
    radio.setAutoAck(false);
    radio.setPALevel(RF24_PA_LOW);
    radio.openWritingPipe(address);
    radio.stopListening();
    Serial.println("RF24 transmitter ready");


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
                Serial.println("Arduino: Drone is READY!");
                break;
            }
        }
    }
    radio.stopListening();
}

void loop() {
    if(Serial.available()){
        radio.stopListening();
        char sendData[32] = {0};
        Serial.readBytesUntil('\n', sendData, 31); 
        Serial.print("Sending data: ");
        Serial.println(sendData);

        bool success = radio.write(&sendData, sizeof(sendData));

        if (success) {
            Serial.println("Arduino: Command sent, waiting for ACK...");
            radio.startListening();
            unsigned long startTime = millis();
            bool receivedAck = false;

            while (millis() - startTime < 50) {
                if (radio.available()) {
                    radio.read(&receivedData, sizeof(receivedData));
                    if (strcmp(receivedData, "ACK") == 0) {
                        receivedAck = true;
                        Serial.println("Arduino: ACK received!");
                        break;
                    }
                }
            }

            if (!receivedAck) {
                Serial.println("Arduino: No ACK, resending...");
            }
            radio.stopListening();
        } else {
            Serial.println("Arduino: Command send failed!");
        }
    }
    const char cmd[] = "CMD:DATA";
    Serial.println("Arduino: Sending command...");
    bool success = radio.write(&cmd, sizeof(cmd));

    if (success) {
        Serial.println("Arduino: Command sent, waiting for ACK...");
        radio.startListening();
        unsigned long startTime = millis();
        bool receivedAck = false;

        while (millis() - startTime < 50) {
            if (radio.available()) {
                radio.read(&receivedData, sizeof(receivedData));
                if (strcmp(receivedData, "ACK") == 0) {
                    receivedAck = true;
                    Serial.println("Arduino: ACK received!");
                    break;
                }
            }
        }

        if (!receivedAck) {
            Serial.println("Arduino: No ACK, resending...");
        }
        radio.stopListening();
    } else {
        Serial.println("Arduino: Command send failed!");
    }
    delay(250);
}
