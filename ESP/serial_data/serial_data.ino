#include <iostream>
#include <HardwareSerial.h>
#include <Wire.h>

#define BAUD_RATE 115200
#define TX 17
#define RX 16
#define SERIAL_MODE SERIAL_8N1

//HardwareSerial SerialPort(2);

void setup() {
  Serial.begin(BAUD_RATE);  
  Serial2.begin(BAUD_RATE, SERIAL_MODE, RX, TX);
}
  //uint8_t SerialBuffer[4];

void loop() {
  if(Serial2.available()){

    uint32_t reievedFrame;

    for(int i = 0; i<4; i++){
      uint8_t byte = Serial2.read();
      Serial.println("Byte number ", i+1);
      Serial.println(byte);
      recievedFrame |= (uint32_t)byte << (i * 8);
    }
    
  }else{
    Serial.println("not recieving");
    delay(2000);
  }

  Serial.println("The data recieved is:");
  Serial.print(reciedFrame);
}
/* 
need in json format::
{
  "xpos": num
  "ypos": num
  "orientation": angle
  "lines": [0,1,0]      // corresponding to [wall on left, wall on right, wall in front]
}
*/

