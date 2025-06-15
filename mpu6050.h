#include <Wire.h>
#include <MPU6050_light.h>
#include "main.h"

class MPU6050_Gyro {
  public:
    MPU6050 mpu;
    MPU6050_Gyro(TwoWire& wire);
    void setup_MPU6050 (uint8_t addr);
    void read_MPU6050 (sensor *data);
};