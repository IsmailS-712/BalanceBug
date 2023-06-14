#include <iostream>
#include <HardwareSerial.h>
#include <Wire.h>

#define BAUD_RATE 115200
#define TX 17
#define RX 16
#define SERIAL_MODE SERIAL_8N1

HardwareSerial SerialPort(2);

void setup() {
  Serial.begin(115200);  
  SerialPort.begin(BAUD_RATE, SERIAL_MODE, RX, TX);
}
  uint8_t SerialBuffer[4];


void loop() {
  if(SerialPort.available()){
    Serial.print(SerialPort.read());
  }
}

