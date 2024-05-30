#include <SPI.h>
#include <RF24.h>

int data[4] = {};
RF24 radio(22, 21); // CE, CSN 引脚
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char receivedData[32] = {0};
    radio.read(&receivedData, sizeof(receivedData));
    Serial.print("Received: ");


    char *token = strtok(receivedData, " ");
    int index = 0;


    while (token != NULL && index < 4) {
      data[index] = atoi(token);
      index++;
      token = strtok(NULL, " ");
    }

    for (int i = 0; i < 4; i++) {
      Serial.print(data[i]);
      Serial.print(" ");
    }
    Serial.println();
  }

  delay(10);
}
