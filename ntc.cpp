#include "ntc.h"

void NTC::read_NTC (sensor *data) {
  int analogValue = analogRead(NTC_PIN);
  float celsius = 1 / (log(1 / (4095. / analogValue - 1)) / NTC_BETA + 1.0 / 298.15) - 273.15;
  data->temp = celsius;
  // Serial.print("Temperature: ");
  // Serial.print(celsius);
  // Serial.println(" ℃");
}