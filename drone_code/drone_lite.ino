#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h>
#include <Wire.h>

Servo alien, vt, ht, motor;
int servo_pins[4] = {12, 14, 27, 26};

RF24 radio(4, 5);
const byte address[6] = "00001";

int data[4] = {};

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("Radio hardware is not responding!");
    while (1);
  }
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  alien.attach(servo_pins[0]);
  ht.attach(servo_pins[1]);
  vt.attach(servo_pins[2]);
  motor.attach(servo_pins[3]);

  Serial.println("Receiver ready.");
}

void loop() {
  if (radio.available()) {
    char receivedData[64] = {0};
    radio.read(receivedData, sizeof(receivedData));

    char *token = strtok(receivedData, " ");
    int index = 0;
    while (token != NULL && index < 4) {
      data[index] = atoi(token);
      index++;
      token = strtok(NULL, " ");
    }

    Serial.print("Received data: ");
    for (int i = 0; i < 4; i++) {
      data[i] = map(data[i], 0, 1999, 0, 180);
      Serial.print(data[i]);
      Serial.print(" ");
    }
    Serial.println();

    // 轉換並控制伺服機
    data[3] = map(data[3], 0, 1999, 0, 180);
    alien.write(data[0]);
    ht.write(data[1]);
    vt.write(data[2]);
    motor.write(data[3]);

    // Serial.print("Motor angle: ");
    // Serial.println(data[3]);
  } else {
    Serial.println("No data");
  }
  delay(100);
}
