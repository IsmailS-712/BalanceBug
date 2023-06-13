// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const int dirPin_r = 16; 
const int stepPin_r = 17;
const int dirPin_l = 15;
const int stepPin_l = 2;

const int stepsPerRevolution = 50;

bool mpu_begin = false;

float accelXAvg = 0.0, accelYAvg = 0.0, accelZAvg = 0.0;
float gyroXAvg = 0.0, gyroYAvg = 0.0, gyroZAvg = 0.0;

void setup(void) {
  Serial.begin(115200);

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  while (mpu_begin == false) {
    Serial.println("Not found");
    if (mpu.begin()){
      mpu_begin = true;
    }
  }
  Serial.println("MPU6050 Found!");

  // motor setup
  pinMode(dirPin_r, OUTPUT);
  pinMode(dirPin_l, OUTPUT);
  pinMode(stepPin_r, OUTPUT);
  pinMode(stepPin_l, OUTPUT);

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  calibrateSensors();
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Gyroscope: ");
  Serial.print("x: ");
  Serial.print(g.gyro.x - gyroXAvg);
  Serial.print(", y: ");
  Serial.print(g.gyro.y - gyroYAvg);
  Serial.print(", z: ");
  Serial.println(g.gyro.z- gyroZAvg);

  Serial.print("Accelerometer: ");
  Serial.print("x: ");
  Serial.print(a.acceleration.x - accelXAvg);
  Serial.print(", y: ");
  Serial.print(a.acceleration.y - accelYAvg);
  Serial.print(", z: ");
  Serial.println(a.acceleration.z - accelZAvg);

  delay(500);

   // Set motor direction clockwise
  digitalWrite(dirPin_r, HIGH);
  digitalWrite(dirPin_l, LOW);

  if (g.gyro.y < 0) {
    // Set motor direction clockwise
    digitalWrite(dirPin_r, LOW);
    digitalWrite(dirPin_l, HIGH);

    for(int x = 0; x < stepsPerRevolution; x++)
    {
      digitalWrite(stepPin_r, HIGH);
      digitalWrite(stepPin_l, HIGH);
      delayMicroseconds(2500);
      digitalWrite(stepPin_r, LOW);
      digitalWrite(stepPin_l, LOW);
      delayMicroseconds(2500);
    }
  } else {
    for(int x = 0; x < stepsPerRevolution; x++)
    {
      digitalWrite(stepPin_r, HIGH);
      digitalWrite(stepPin_l, HIGH);
      delayMicroseconds(2500);
      digitalWrite(stepPin_r, LOW);
      digitalWrite(stepPin_l, LOW);
      delayMicroseconds(2500);
    }
  }
}

void calibrateSensors() {
  Serial.println("Starting sensor calibration...");
  delay(200);

  Serial.println("Collecting calibration data...");
  const int numSamples = 100;

  for (int i = 0; i < numSamples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accelXAvg += a.acceleration.x;
    accelYAvg += a.acceleration.y;
    accelZAvg += a.acceleration.z;
    gyroXAvg += g.gyro.x;
    gyroYAvg += g.gyro.y;
    gyroZAvg += g.gyro.z;
    delay(10);
  }

  accelXAvg /= numSamples;
  accelYAvg /= numSamples;
  accelZAvg /= numSamples;
  gyroXAvg /= numSamples;
  gyroYAvg /= numSamples;
  gyroZAvg /= numSamples;

  Serial.println("Calibration complete!");
  Serial.print("Accelerometer Averages: ");
  Serial.print("x: ");
  Serial.print(accelXAvg);
  Serial.print(", y: ");
  Serial.print(accelYAvg);
  Serial.print(", z: ");
  Serial.println(accelZAvg);
  Serial.print("Gyroscope Averages: ");
  Serial.print("x: ");
  Serial.print(gyroXAvg);
  Serial.print(", y: ");
  Serial.print(gyroYAvg);
  Serial.print(", z: ");
  Serial.println(gyroZAvg);
}
