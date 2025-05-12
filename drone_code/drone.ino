#include <SPI.h>
#include <RF24.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <ESP32Servo.h>
#include <Wire.h>
#include <LSM303.h>
#include <math.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

#define RXPin 16
#define TXPin 17
#define GPSBaud 9600
TinyGPSPlus gps;
HardwareSerial ss(2);

Servo alien, vt, ht, motor;
int servo_pins[4] = { 12, 14, 27, 26 };
int servo_degree[4] = {};
char send_data[32] = {};

LSM303 compass;
RF24 radio(4, 5);
const byte address[6] = "00001";
char receivedData[32] = { 0 };
bool isSynced = false;

String geohash = "INVALID";
int satallites_value = 0;
int hdop_value = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32 Initialized!");

  if (!radio.begin()) {
    Serial.println("ESP32: Radio hardware is not responding!");
    while (1)
      ;
  }

  // NRF24設定
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(100);
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.setAutoAck(false);
  radio.startListening();

  // 伺服馬達設定
  alien.attach(servo_pins[0]);
  ht.attach(servo_pins[1]);
  vt.attach(servo_pins[2]);
  motor.attach(servo_pins[3]);

  // 陀螺儀設定
  Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.m_min = (LSM303::vector<int16_t>){ -32767, -32767, -32767 };
  compass.m_max = (LSM303::vector<int16_t>){ +32767, +32767, +32767 };

  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin, false);
  Serial.println("=== GPS Module Initialized ===");
  Serial.println("TinyGPS++ Version: " + String(TinyGPSPlus::libraryVersion()));

  // GPS任務
  xTaskCreatePinnedToCore(
    gpsTask,
    "GPS Task",
    4096,
    NULL,
    1,
    NULL,
    1);
}

void loop() {
  if (radio.available()) {
    //接收資料
    radio.read(&receivedData, sizeof(receivedData));
    // Serial.print("Received: ");
    // Serial.println(receivedData);

    //轉換文字訊號
    char *token = strtok(receivedData, " ");
    int index = 0;
    while (token != NULL && index < 4) {
      servo_degree[index] = atoi(token);
      index++;
      token = strtok(NULL, " ");
    }

    change_servo_degree();
    gy511();

    if (gps.encode(ss.read())) {
      displayInfo();
    }

    //資料回傳
    radio.stopListening();
    const char ackMsg[] = "ACK";
    bool success = radio.write(&send_data, sizeof(send_data));
    if (success) {
      // Serial.println("ESP32: ACK sent successfully!");
    } else {
      // Serial.println("ESP32: ACK send failed!");
    }
    radio.startListening();
  }
}

void gpsTask(void *pvParameters) {
  for (;;) {
    if (ss.available()) {
      gps.encode(ss.read());
      displayInfo();
    }
    vTaskDelay(pdMS_TO_TICKS(250));  // 每 100ms 檢查一次
  }
}

// --------------------GPS-------------------- //
void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    double lat = gps.location.lat();
    double lon = gps.location.lng();
    Serial.print(lat, 6);
    Serial.print(F(","));
    Serial.print(lon, 6);

    // 轉換 Geohash
    geohash = encodeGeohash(lat, lon, 6);
    // Serial.print(F("  | Geohash: "));
    // Serial.print(geohash);
  } else {
    // Serial.print(F("INVALID"));
    geohash = "INVALID";
  }

  // Serial.print(F("  | Satellites: "));
  // Serial.print(gps.satellites.value());

  // Serial.print(F("  | HDOP: "));
  // Serial.print(gps.hdop.value());

  // Serial.print(F("  | UTC Time: "));
  if (gps.time.isValid()) {
    // Serial.printf("%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
  } else {
    // Serial.print(F("INVALID"));
  }

  Serial.println();
  satallites_value = gps.satellites.value();
  hdop_value = gps.hdop.value();
}

String encodeGeohash(double lat, double lon, int precision) {
  static const char BASE32[] = "0123456789bcdefghjkmnpqrstuvwxyz";
  bool is_even = true;
  int bit = 0, ch = 0;
  String geohash = "";

  double lat_min = -90.0, lat_max = 90.0;
  double lon_min = -180.0, lon_max = 180.0;

  while (geohash.length() < precision) {
    if (is_even) {
      double lon_mid = (lon_min + lon_max) / 2;
      if (lon >= lon_mid) {
        ch |= (1 << (4 - bit));
        lon_min = lon_mid;
      } else {
        lon_max = lon_mid;
      }
    } else {
      double lat_mid = (lat_min + lat_max) / 2;
      if (lat >= lat_mid) {
        ch |= (1 << (4 - bit));
        lat_min = lat_mid;
      } else {
        lat_max = lat_mid;
      }
    }

    is_even = !is_even;
    if (bit < 4) {
      bit++;
    } else {
      geohash += BASE32[ch];
      bit = 0;
      ch = 0;
    }
  }
  return geohash;
}

// --------------------Servo-------------------- //
void change_servo_degree() {
  servo_degree[0] = map(servo_degree[0], 0, 1999, 45, 135);
  servo_degree[1] = map(servo_degree[1], 0, 1999, 40, 140);
  servo_degree[2] = map(servo_degree[2], 0, 1999, 70, 130);
  servo_degree[3] = map(servo_degree[3], 0, 1999, 1000, 2000);
  alien.write(servo_degree[0]);
  ht.write(servo_degree[1]);
  vt.write(servo_degree[2]);
  motor.write(servo_degree[3]);
}

// --------------------GY511-------------------- //
void gy511() {
  compass.read();

  float ax = compass.a.x;
  float ay = compass.a.y;
  float az = compass.a.z;
  float mx = compass.m.x;
  float my = compass.m.y;
  float mz = compass.m.z;

  int pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
  if (pitch < 0) pitch += 360;

  int roll = atan2(ay, az) * 180.0 / M_PI;
  if (roll < 0) roll += 360;

  int heading = atan2(my, mx) * 180.0 / M_PI;
  if (heading < 0) heading += 360;

  // 顯示結果
  // Serial.print("Pitch: ");
  // Serial.print(pitch);
  // Serial.print("\tRoll: ");
  // Serial.print(roll);
  // Serial.print("\tYaw: ");
  // Serial.println(heading);

  String combined = String(pitch) + " " + String(roll) + " " + String(heading)+ " " + String(satallites_value)+ " " + String(hdop_value)+ " " + geohash;
  combined.toCharArray(send_data, sizeof(send_data));
  Serial.println(send_data);
}