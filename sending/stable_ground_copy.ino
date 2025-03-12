#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
char receivedData[32] = {0};
bool isSynced = false;
String received_text;

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
                Serial.println("Arduino: Drone is READY!");
                break;
            }
        }
    }
    radio.stopListening();
}

void loop() {
    if (Serial.available() >= sizeof(float) * 4) {  // 假設我們知道要接收的浮點數陣列長度為 4
        float receivedFloats[4];
        Serial.readBytes((char*)receivedFloats, sizeof(receivedFloats));
        Serial.print("Received floats: ");
        for (int i = 0; i < 4; i++) {
            Serial.print(receivedFloats[i]);
            Serial.print(" ");
        }
        Serial.println();
    }
    
    delay(250);
}
