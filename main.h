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
  float temp;
  float humidity;

  bool motion;
  patient_status status;
} sensor;

#endif 
