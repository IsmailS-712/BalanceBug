#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <AutoPID.h>
#include <ContinuousStepper.h>

#define IMU_SCL_PIN 22
#define IMU_SDA_PIN 21
#define IMU_INT_PIN 23

static MPU6050 imu;

static double pitch, pitch_setpoint, pitch_action;
static AutoPID pitch_controller(&pitch, &pitch_setpoint, &pitch_action, -250, 250, 7, 0, 0);

static ContinuousStepper left_stepper, right_stepper;

//hw_timer_t* stepper_timer = nullptr;

void ARDUINO_ISR_ATTR on_stepper_timer()
{
    left_stepper.loop();
    right_stepper.loop();
}

volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

void setup()
{
    Serial.begin(1000000);
    Serial.println();
    
    Wire.begin(IMU_SDA_PIN, IMU_SCL_PIN, 400000);
    pinMode(IMU_INT_PIN, INPUT);

    imu.initialize();
    Serial.printf("imu.testConnection() -> %s\n", imu.testConnection() ? "OK" : "Bad");
    Serial.printf("imu.dmpInitialize() -> %s\n", imu.dmpInitialize() ? "Bad" : "OK");
    imu.CalibrateAccel(10);
    imu.CalibrateGyro(10);
    imu.setDMPEnabled(true);
    attachInterrupt(IMU_INT_PIN, dmpDataReady, RISING);

     pitch_setpoint = 0.0;
     pitch_controller.setTimeStep(5);

    left_stepper.begin(2, 15);
    left_stepper.spin(1);
    right_stepper.begin(17, 16);
    right_stepper.spin(1);;

    //stepper_timer = timerBegin(0, 20, true);
     //timerAttachInterrupt(stepper_timer, on_stepper_timer, false);
     //timerAlarmWrite(stepper_timer, 50, true);
    //timerAlarmEnable(stepper_timer);
}

void loop()
{
    static unsigned long loop_count = 0;
    static unsigned long last_print_time_us = micros();
    static uint8_t dmp_fifo_buffer[64];
    static Quaternion quarternion;
    static VectorFloat gravity;
    static float yaw_pitch_roll[3];
    
     if (imu.dmpGetCurrentFIFOPacket(dmp_fifo_buffer)) {
        imu.dmpGetQuaternion(&quarternion, dmp_fifo_buffer);
        imu.dmpGetGravity(&gravity, &quarternion);
        imu.dmpGetYawPitchRoll(yaw_pitch_roll, &quarternion, &gravity);
        pitch = (yaw_pitch_roll[1] / 3.14 * 180);      
        pitch_controller.run();
        left_stepper.spin(-pitch_action * abs(pitch_action));
        right_stepper.spin(pitch_action * abs(pitch_action));
        left_stepper.loop();
        right_stepper.loop();
        Serial.printf("pitch = %7.2lf, pitch_action = %.1lf\n", yaw_pitch_roll[1] / 3.14 * 180, pitch_action);
     }
    
    //  pitch_controller.run();

    //  left_stepper.spin(-pitch_action * abs(pitch_action));
    //  right_stepper.spin(pitch_action * abs(pitch_action));

    //  if (loop_count++ >= 1000) {
    //     auto now_us = micros();
    //     Serial.printf("pitch = %7.2lf, pitch_action = %7.1lf, sample_rate = %.1lf\n", pitch, pitch_action, (double)loop_count / (now_us - last_print_time_us) * 1000000.0);
    //    last_print_time_us = micros();
    //     loop_count = 0;
    //  }
}