#include <SPI.h>
#include <RF24.h>
#include <ESP32Servo.h>

RF24 radio(4, 5);  // ESP32 CE = 4, CSN = 5
const byte address[5] = {'0', '0', '0', '9', '6'};

Servo alien, vt, ht, motor;
int servo_pins[4] = {12, 14, 27, 25};
int data[4] = {0};

void setup() {
  Serial.begin(115200);
  
  if (!radio.begin()) {
    Serial.println("RF24 module not connected");
    while (1);
  }

  radio.setChannel(100);            
  radio.setDataRate(RF24_250KBPS);  
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, address);
  radio.startListening();
  radio.setAutoAck(false);
  alien.attach(servo_pins[0]);
  ht.attach(servo_pins[1]);
  vt.attach(servo_pins[2], 500, 2500); 
  motor.attach(servo_pins[3]);

  Serial.println("ESP32 receiver ready");
}

void loop() {
  if (radio.available()) {
    char receivedData[32] = {0};  // ✅ 限制大小 32 Bytes
    radio.read(receivedData, sizeof(receivedData));

    Serial.print("Received raw data: ");
    Serial.println(receivedData);

    // 解析數據
    char *token = strtok(receivedData, " ");
    int index = 0;
    while (token != NULL && index < 4) {
      data[index] = atoi(token);
      index++;
      token = strtok(NULL, " ");
    }

    data[0] = map(data[0], 0, 1999, 45, 135);
    data[1] = map(data[1], 0, 1999, 40, 140);
    data[2] = map(data[2], 0, 1999, 70, 130);
    data[3] = map(data[3], 0, 1999, 1000, 2000);
    for(int i = 0; i < 4; i++){
      Serial.print(data[i]);
    }
    Serial.println();

    // 控制伺服機
    alien.write(data[0]);
    ht.write(data[1]);
    vt.write(data[2]);
    motor.write(data[3]);

  } else {
    Serial.println("No data received");
  }
  delay(10);
}
