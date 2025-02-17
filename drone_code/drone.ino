#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS.h>
#include <HardwareSerial.h>

#define GY511_add 0x19
#define BMP280_add 0x76
#define SDA 21
#define SCL 22

Adafruit_BMP280 bmp;
Servo alien;
Servo vt;
Servo ht;
Servo motor;
int servo_pins[4] = {2, 0, 4, 16};
TinyGPS gps;
HardwareSerial gpsSerial(2);
RF24 radio(17, 5);
const byte address[6] = "00001";

String send;
int16_t x = 0;
int16_t y = 0;
int16_t z = 0;
float latitude = 0.0;
float longitude = 0.0;
int altitude = 0;

int data[4] = {};

void nrf_receive_Task(void *pvParam) {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  while (1) {
    radio.startListening();
    if (radio.available()) {
      char receivedData[64] = {0};
      radio.read(&receivedData, sizeof(receivedData));
      char *token = strtok(receivedData, " ");
      int index = 0;
      while (token != NULL && index < 4) {
        data[index] = atoi(token);
        index++;
        token = strtok(NULL, " ");
      }
      
      // Serial.print("Received data: ");
      // for (int i = 0; i < 4; i++) {
      //   Serial.print(data[i]);
      //   Serial.print(" ");
      // }
      Serial.print(data[3]);
      Serial.print(" ");
      data[3] = map(data[3], 0, 1999, 0, 180);
      alien.write(data[0]);
      ht.write(data[1]);
      vt.write(data[2]);
      motor.write(data[3]);
      Serial.println(data[3]);
      // Serial.println();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    radio.stopListening();
    send = String(x) + " " + String(y) + " " + String(z) + " " + String(latitude, 6) + " " + String(longitude, 6) + " " + String(altitude);
    // Serial.println(send);
    char sendData[64];
    send.toCharArray(sendData, 64);
    radio.write(&sendData, sizeof(sendData));
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void i2c_Task(void *pvParam) {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  while (1) {
    readAccelerometer();
    readBMP();
  }
}

void gps_Task(void *pvParam) {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  while (1) {
    bool newData = false;

    for (unsigned long start = millis(); millis() - start < 1000;) {
      while (gpsSerial.available()) {
        char c = gpsSerial.read();
        if (gps.encode(c))
          newData = true;
      }
    }

    if (newData) {
      float flat, flon;
      unsigned long age;
      gps.f_get_position(&flat, &flon, &age);

      latitude = (flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat);
      longitude = (flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("Radio hardware is not responding!");
    while (1);
  }
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
  radio.setPALevel(RF24_PA_MAX);
  alien.attach(servo_pins[0]);
  ht.attach(servo_pins[1]);
  vt.attach(servo_pins[2]);
  motor.attach(servo_pins[3]);
  Wire.begin(SDA, SCL);
  initAccelerometer();
  gpsSerial.begin(4800, SERIAL_8N1, 14, 12);
  delay(100);

  xTaskCreatePinnedToCore(nrf_receive_Task, "nrf_receive_Task", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(i2c_Task, "i2c_Task", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(gps_Task, "gps_Task", 1024, NULL, 1, NULL, 1);

  Serial.println("Receiver ready.");
}

void loop() {}

void initAccelerometer() {
  Wire.beginTransmission(GY511_add);
  Wire.write(0x20);
  Wire.write(0x27);
  Wire.endTransmission();
}

void readAccelerometer() {
  Wire.beginTransmission(GY511_add);
  Wire.write(0x28 | 0x80);
  Wire.endTransmission();

  Wire.requestFrom(GY511_add, 6);
  if (Wire.available() == 6) {
    x = Wire.read() | (Wire.read() << 8);
    y = Wire.read() | (Wire.read() << 8);
    z = Wire.read() | (Wire.read() << 8);
  }
}

void readBMP() {
  altitude = bmp.readAltitude();
}
