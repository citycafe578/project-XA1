#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#define GY511_add 0x19
#define BMP280_add 0x76
#define SDA 21
#define SCL 22

Adafruit_BMP280 bmp;
Servo alien;
int alien_pin = 14;

int data[4] = {};
RF24 radio(17, 5); // CE, CSN 引脚
const byte address[6] = "00001";

void nrf_receive_Task(void *pvParam){
  vTaskDelay(1000/portTICK_PERIOD_MS);
  while(1){
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

      // for (int i = 0; i < 4; i++) {
      //   Serial.print(data[i]);
      //   Serial.print(" ");
      // }
      // Serial.println();
    }
    alien.write(data[3]);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void i2c_Task(void *pvParam){
  vTaskDelay(1000/portTICK_PERIOD_MS);
  while(1){
    // readAccelerometer();
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// void gps_Task(void *pvParam){
//   vTaskDelay(1000/portTICK_PERIOD_MS);
//   while(1){
    
//   }
// }

// void nrf_send_Task(void *pvParam){
//   vTaskDelay(1000/portTICK_PERIOD_MS);
//   while(1){

//   }
// }

void setup(){
  Serial.begin(9600);
  xTaskCreate(nrf_receive_Task, "nrf_receive_Task", 1024, NULL, 2, NULL);
  xTaskCreate(i2c_Task, "i2c_Task", 2048, NULL, 1, NULL);
  // xTaskCreate(gps_Task, "gps_Task", 1024, NULL, 1, NULL);
  // xTaskCreate(nrf_send_Task, "nrf_send_Task", 1024, NULL, 1, NULL);
  vTaskDelay(2000/portTICK_PERIOD_MS);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  alien.attach(14);
  Wire.begin(SDA, SCL);
  initAccelerometer();
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

void loop(){

}

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
    int16_t x = Wire.read() | (Wire.read() << 8);
    int16_t y = Wire.read() | (Wire.read() << 8);
    int16_t z = Wire.read() | (Wire.read() << 8);
    
    Serial.print("Accelerometer X: "); Serial.print(x);
    Serial.print(" Y: "); Serial.print(y);
    Serial.print(" Z: "); Serial.println(z);
  }
}