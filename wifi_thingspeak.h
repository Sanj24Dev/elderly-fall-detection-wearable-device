#include "main.h"
#include <WiFi.h>
#include "ThingSpeak.h"
#include "conf.h"

void setup_WiFi ();
void push_data(sensor d, int myChannelNumber, const char* myApiKey);