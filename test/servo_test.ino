#include <ESP32Servo.h> 
 
Servo myservo;
int servoPin=2;
 
int pos = 0;    // 定義舵機轉動位置
 
void setup()
{
      myservo.attach(servoPin);  // 設置舵機控制腳位
}
 
void loop()
{
     // 0到180旋轉舵機，每次延時15毫秒
      for(pos = 0; pos < 180; pos += 1) { 
         myservo.write(pos); 
         delay(15); 
      } 
      // 180到0旋轉舵機，每次延時15毫秒 
      for(pos = 180; pos>=1; pos-=1)
      {                               
        myservo.write(pos);
        delay(15);
      }
}