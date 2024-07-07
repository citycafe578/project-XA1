#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
String received_text;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    if (!radio.begin()) {
        Serial.println("Radio hardware is not responding!");
        while (1);
    }
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.stopListening();
}

void loop() {
    if (Serial.available()) {
        radio.stopListening(); 
        String data = Serial.readStringUntil('\n');
        char sendData[64];
        data.toCharArray(sendData, 64);
        Serial.print("Sending: ");
        Serial.println(data);
        radio.write(&sendData, sizeof(sendData));
    }
    delay(200);
}
