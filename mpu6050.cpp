#include "mpu6050.h"

MPU6050_Gyro::MPU6050_Gyro(TwoWire& wire)
  : mpu(wire)
{
}

void MPU6050_Gyro::setup_MPU6050 (uint8_t addr) {
  pinMode(AD0_PIN, OUTPUT);
  if (addr == 0x68)
    digitalWrite(AD0_PIN, LOW);   // 0x68
  else
    digitalWrite(AD0_PIN, HIGH);  // 0x69

  // Initialize MPU
  byte status = mpu.begin();
  Serial.print(F("Initializing MPU-6050, error = "));
  Serial.print(status);
  if (status == 0)
    Serial.println(F(" (no error)"));
  else {
    Serial.println("\nMPU6050 init failed. Check wiring or address.");
    while (1);
  }

  // Calibrate
  mpu.calcOffsets();
}

// MPU task: Reads data from MPU6050 every 1s
void MPU6050_Gyro::read_MPU6050 (sensor *data) {
  mpu.update();

  data->accX = mpu.getAccX();
  data->accY = mpu.getAccY();
  data->accZ = mpu.getAccZ();

  data->rotX = mpu.getGyroX();
  data->rotY = mpu.getGyroY();
  data->rotZ = mpu.getGyroZ();
}