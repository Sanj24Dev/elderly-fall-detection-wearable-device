#include "dht22.h"

void DHT22::setup_DHT22 (uint8_t PIN) {
  dhtSensor.setup(PIN, DHTesp::DHT22);
}

void DHT22::read_DHT22 (sensor *data) {
  TempAndHumidity  dht_values = dhtSensor.getTempAndHumidity();
  data->temp = dht_values.temperature;
  data->humidity = dht_values.humidity;
}