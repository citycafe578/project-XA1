#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";
char droneData[32] = { 0 };
char linuxData[32] = { 0 };
bool isSynced = false;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Arduino Initialized!");

  if (!radio.begin()) {
    Serial.println("Arduino: Radio hardware is not responding!");
    while (1)
      ;
  }

  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(100);
  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.stopListening();


  Serial.println("Arduino: Sending SYNC...");
  const char syncMsg[] = "SYNC";
  bool success = (radio.write(&syncMsg, sizeof(syncMsg)));
  if(success){
    Serial.println("SYNC sent successfully");
  }else{
    Serial.println("SYNC send failed");
  }

  radio.startListening();
  unsigned long startTime = millis();
  while (millis() - startTime < 100) {
    if (radio.available()) {
      radio.read(&droneData, sizeof(droneData));
      if (strcmp(droneData, "READY") == 0) {
        isSynced = true;
        Serial.println("Arduino: Drone is READY!");
        break;
      }
    }
  }
  radio.stopListening();
}

void loop() {
  // Serial.readBytesUntil('\n', linuxData, 31);
  linuxData[0] = 'A';  //測試用
  Serial.print("Received from Python: ");
  Serial.println(linuxData);
  bool success = radio.write(&linuxData, sizeof(linuxData));

  if (success) {
    radio.startListening();
    unsigned long startTime = millis();
    bool receivedAck = false;

    while (millis() - startTime < 50) {
      if (radio.available()) {
        radio.read(&droneData, sizeof(droneData));
        Serial.print("Received from Drone: ");
        Serial.println(droneData);
        receivedAck = true;
      }
    }

    if (!receivedAck) {
      Serial.println("No data back!");
    }
    radio.stopListening();
  } else {
    Serial.println("Arduino: Command send failed!");
  }
  delay(500);
}
