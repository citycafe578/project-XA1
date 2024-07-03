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
int alien_pin = 14;
TinyGPS gps;
HardwareSerial gpsSerial(2);  // Use UART2

int data[4] = {};
RF24 radio(17, 5); // CE, CSN 引脚
const byte address[6] = "00001";

String send;
int16_t x = 0;
int16_t y = 0;
int16_t z = 0;
float latitude = 0.0;
float longitude = 0.0;
int altitude = 0;

void nrf_receive_Task(void *pvParam){
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  while (1) {
    radio.startListening();
    if (radio.available()) {
      char receivedData[32] = {0};
      radio.read(&receivedData, sizeof(receivedData));

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
    alien.write(data[3]);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    radio.stopListening();
    // send = String(x) + " " + String(y) + " " + String(z) + " " + String(latitude, 6) + " " + String(longitude, 6) + " " + String(altitude);
  }
}

void i2c_Task(void *pvParam){
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  while (1) {
    readAccelerometer();
    readBMP();
  }
}

void gps_Task(void *pvParam){
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

      // Serial.print("LAT=");
      // Serial.print(latitude, 6);
      // Serial.print(" LON=");
      // Serial.println(longitude, 6);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    } else {
      // Serial.println("no gps data");
    }
  }
}

void setup(){
  Serial.begin(9600);
  xTaskCreatePinnedToCore(nrf_receive_Task, "nrf_receive_Task", 1024, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(i2c_Task, "i2c_Task", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(gps_Task, "gps_Task", 1024, NULL, 1, NULL, 1);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  alien.attach(14);
  Wire.begin(SDA, SCL);
  initAccelerometer();
  gpsSerial.begin(4800, SERIAL_8N1, 14, 12);
  delay(500);
}

void loop(){}

void initAccelerometer(){
  Wire.beginTransmission(GY511_add);
  Wire.write(0x20);
  Wire.write(0x27);
  Wire.endTransmission();
}

void readAccelerometer(){
  Wire.beginTransmission(GY511_add);
  Wire.write(0x28 | 0x80);
  Wire.endTransmission();
  
  Wire.requestFrom(GY511_add, 6);
  if (Wire.available() == 6) {
    int16_t x = Wire.read() | (Wire.read() << 8);
    int16_t y = Wire.read() | (Wire.read() << 8);
    int16_t z = Wire.read() | (Wire.read() << 8);
    
    // Serial.print("Accelerometer X: "); Serial.print(x);
    // Serial.print(" Y: "); Serial.print(y);
    // Serial.print(" Z: "); Serial.println(z);
  }
}

void readBMP(){
  altitude = bmp.readAltitude();
  // Serial.print(bmp.readAltitude(1013.25)); 
  // Serial.println(" m   ");
  // Serial.print(bmp.readTemperature()); 
  // Serial.println(" c");
}
