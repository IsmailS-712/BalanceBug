#include <iostream>
#include <HardwareSerial.h>
#include <Wire.h>

#define BAUD_RATE 115200
#define TX2 17
#define RX2 16
#define SERIAL_MODE SERIAL_8N1

void setup() {
  Serial.begin(BAUD_RATE);
  Serial2.begin(BAUD_RATE, SERIAL_MODE, RX2, TX2);
}

void loop() {
  while(Serial2.available()){
    uint8_t byte = Serial2.read();
    Serial.println(byte); 
    //delay(1000); 
  } 
  Serial.println("not recieving");
}