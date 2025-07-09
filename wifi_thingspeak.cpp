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
      isnan(d.temp) || isnan(d.distance)) {
    Serial.printf("Invalid data, skipping.\n");
    return;
  }

  float acc = sqrt(
                d.accX * d.accX +
                d.accY * d.accY +
                d.accZ * d.accZ
              );
  float rot = sqrt(
                d.rotX * d.rotX +
                d.rotY * d.rotY +
                d.rotZ * d.rotZ
              );

  ThingSpeak.setField(1, acc);
  ThingSpeak.setField(2, rot);
  ThingSpeak.setField(3, d.distance);
  ThingSpeak.setField(4, d.temp);
  ThingSpeak.setField(5, d.status);

  int x = ThingSpeak.writeFields(myChannelNumber, myApiKey);

  if (x == 200) {
    Serial.printf("Data pushed successfully.\n");
  } else {
    Serial.printf("Push error: %d\n", x);
  }

  delay(10000); // Delay 10s to be safe
}
