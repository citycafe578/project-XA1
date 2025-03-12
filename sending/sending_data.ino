#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE = 7, CSN = 8
const byte address[5] = {'0', '0', '0', '9', '6'};

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!radio.begin()) {
    Serial.println("RF24 module not connected");
    while (1);
  }

  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(address);
  radio.stopListening();
  Serial.println("RF24 transmitter ready");
}

void loop() {
  if (Serial.available()) {
    radio.stopListening();
    char sendData[32] = {0};
    Serial.readBytesUntil('\n', sendData, 31); 
    Serial.print("Sending data: ");
    Serial.println(sendData);

    bool success = radio.write(&sendData, sizeof(sendData));
    if (success) {
      Serial.println("Send successful");
    } else {
      Serial.println("Send failed");
    }
  }
  delay(100);
}