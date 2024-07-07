#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[][6] = {"00001", "00002"};
String received_text;

void setup() {
    Serial.begin(9600);
    while (!Serial);
        if (!radio.begin()) {
        Serial.println("Radio hardware is not responding!");
        while (1);
    }
    radio.setRetries(15, 15);
    radio.openWritingPipe(address[1]);
    radio.openReadingPipe(1, address[0]);
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

        bool success = radio.write(&sendData, sizeof(sendData));
        if (success) {
            Serial.println("Send successful");
        } else {
            Serial.println("Send failed");
        }
        delay(200);
        radio.startListening();
        char text[64] = "";
        if (radio.available()) {
            radio.read(&text, sizeof(text));
            received_text = String(text);
            Serial.print("Received: ");
            Serial.println(received_text);
        }
    }
    delay(5);
}