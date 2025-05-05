
#include <SPI.h>
#include <RF24.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <ESP32Servo.h>
#include <Wire.h>
#include <LSM303.h>
#include <math.h>

Servo alien, vt, ht, motor;
int servo_pins[4] = { 12, 14, 27, 26 };
int servo_degree[4] = {};

RF24 radio(4, 5);
const byte address[6] = "00001";
char groundData[32] = { 0 };
char sendData[32] = { 0 };
bool isSynced = false;

LSM303 compass;
char gy511Data[32] = { 0 };

void gy511() {
  compass.read();
  float ax = compass.a.x;
  float ay = compass.a.y;
  float az = compass.a.z;
  float mx = compass.m.x;
  float my = compass.m.y;
  float mz = compass.m.z;

  int pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
  if (pitch < 0) {
    pitch += 360;
  }

  int roll = atan2(ay, az) * 180.0 / M_PI;
  if (roll < 0) {
    roll += 360;
  }

  int heading = atan2(my, mx) * 180.0 / M_PI;
  if (heading < 0) {
    heading += 360;
  }

  sprintf(gy511Data, "%d %d %d", pitch, roll, heading);
  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print("\tRoll: ");
  Serial.print(roll);
  Serial.print("\tYaw: ");
  Serial.println(heading);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32 Initialized!");

  alien.attach(servo_pins[0]);
  ht.attach(servo_pins[1]);
  vt.attach(servo_pins[2]);
  motor.attach(servo_pins[3]);

  if (!radio.begin()) {
    Serial.println("ESP32: Radio hardware is not responding!");
    while (1)
      ;
  }

  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(100);
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.startListening();

  Serial.println("ESP32: Waiting for SYNC...");
  unsigned long startTime = millis();
  while (millis() - startTime < 100) {
    if (radio.available()) {
      radio.read(&groundData, sizeof(groundData));
      if (strcmp(groundData, "SYNC") == 0) {
        Serial.println("ESP32: SYNC received, sending READY...");
        radio.stopListening();
        const char readyMsg[] = "READY";
        radio.write(&readyMsg, sizeof(readyMsg));
        radio.startListening();
        isSynced = true;
        break;
      }
    }
  }
}

void loop() {
  gy511();
  if (radio.available()) {
    radio.read(&groundData, sizeof(groundData));
    // Serial.print("ESP32: Received - ");
    // Serial.println(groundData);
    radio.read(&groundData, sizeof(groundData));
    char *token = strtok(groundData, " ");
    int index = 0;
    while (token != NULL && index < 4) {
      servo_degree[index] = atoi(token);
      index++;
      token = strtok(NULL, " ");
    }
    servo_degree[0] = map(servo_degree[0], 0, 1999, 45, 135);
    servo_degree[1] = map(servo_degree[1], 0, 1999, 40, 140);
    servo_degree[2] = map(servo_degree[2], 0, 1999, 70, 130);
    servo_degree[3] = map(servo_degree[3], 0, 1999, 1000, 2000);
    alien.write(servo_degree[0]);
    ht.write(servo_degree[1]);
    vt.write(servo_degree[2]);
    motor.write(servo_degree[3]);

    sendData[0] = 'B';  //測試用
    radio.stopListening();
    bool success = radio.write(&sendData, sizeof(sendData));
    if (success) {
      // Serial.println("ESP32: ACK send successfully!");
    } else {
      // Serial.println("ESP32: ACK send failed");
    }
    radio.startListening();
  }
}
