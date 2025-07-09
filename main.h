#include <Arduino.h>
#include "conf.h"

#ifndef SENSOR_H
#define SENSOR_H

#define BUFFER_SIZE 10

typedef enum {
  SAFE,
  FALL,
  SICK
} patient_status;

typedef struct {
  float accX;
  float accY;
  float accZ;
  float rotX;
  float rotY;
  float rotZ;
  float temp;
  float distance;

  patient_status status;
} sensor;

#endif 
