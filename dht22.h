#include "main.h"
#include "DHTesp.h"

class DHT22 {
  public:
    DHTesp dhtSensor;
    void setup_DHT22 (uint8_t PIN);
    void read_DHT22 (sensor *data);
};