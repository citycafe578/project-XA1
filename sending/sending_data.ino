#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE = 7, CSN = 8
const byte address[5] = {'0', '0', '0', '9', '6'};  // Address length limited to 5 Bytes

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!radio.begin()) {
    Serial.println("RF24 module not connected");
    while (1);
  }

  radio.setChannel(100);            // Set channel, same as ESP32
  radio.setDataRate(RF24_250KBPS);  // Set data rate, same as ESP32
  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_LOW);    // Set maximum power level
  radio.openWritingPipe(address);   // Set transmitter address
  radio.stopListening();            // Set to transmit mode

  Serial.println("RF24 transmitter ready");
}

void loop() {
  if (Serial.available()) {
    radio.stopListening();  // Ensure transmit mode
    char sendData[32] = {0};  // Initialize to avoid garbage data
    Serial.readBytesUntil('\n', sendData, 31);  // Limit length to avoid exceeding 32 Bytes

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