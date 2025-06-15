#include "wifi_thingspeak.h"

WiFiClient client;

void setup_WiFi () {  
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PWD, 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Wifi connected !"); // Print a message if WiFi is connected
  Serial.println("Local IP: " + String(WiFi.localIP())); // Print the local IP address
  WiFi.mode(WIFI_STA); // Set the WiFi mode to station mode
  ThingSpeak.begin(client); // Initialize the ThingSpeak library
}

void push_data(sensor d, int myChannelNumber, const char* myApiKey) {
  if (isnan(d.accX) || isnan(d.accY) || isnan(d.accZ) ||
      isnan(d.temp) || isnan(d.humidity)) {
    Serial.printf("Invalid data, skipping.\n");
    return;
  }

  ThingSpeak.setField(1, d.accX);
  ThingSpeak.setField(2, d.accY);
  ThingSpeak.setField(3, d.accZ);
  ThingSpeak.setField(4, d.temp);
  ThingSpeak.setField(5, d.humidity);
  ThingSpeak.setField(6, d.motion);
  ThingSpeak.setField(7, d.status);

  int x = ThingSpeak.writeFields(myChannelNumber, myApiKey);

  if (x == 200) {
    Serial.printf("Data pushed successfully.\n");
  } else {
    Serial.printf("Push error: %d\n", x);
  }

  delay(10000); // Delay 10s to be safe
}
