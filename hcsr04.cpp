#include "hcsr04.h"

void HCSR04::setup_HCSR04 () {
  pinMode(HC_TRIG, OUTPUT);
  pinMode(HC_ECHO, INPUT);
  distance = 0;
}

void HCSR04::read_HCSR04 () {
  long durasi;
  int jarak;
  digitalWrite(HC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(HC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG, LOW);

  durasi = pulseIn(HC_ECHO, HIGH);
  jarak = durasi * 0.03408 / 2;
  distance = jarak;
}