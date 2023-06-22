// UPDATED CODE - CHANGED SERIAL2 TO SERIAL_TWO AS HARDWARESERIAL DIDNT ACCEPT NAME AND ALSO ADDED INSTANTIATION OF HARDWARESERIAL OBJECT

#include <iostream>
#include <HardwareSerial.h>
#include <Wire.h>

HardwareSerial Serial_two(2);

#define BAUD_RATE 115200
#define TX2 17
#define RX2 16
#define SERIAL_MODE SERIAL_8N1

void setup() {
  Serial.begin(BAUD_RATE);
  Serial_two.begin(BAUD_RATE, SERIAL_MODE, RX2, TX2);
}

void loop() {
  while(Serial_two.available()){
    uint8_t byte = Serial_two.read();
    Serial.println(byte); 
    //delay(1000); 
  } 
  Serial.println("not recieving");
}
