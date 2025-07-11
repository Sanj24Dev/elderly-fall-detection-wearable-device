#ifndef CONF_H
#define CONF_H

#define WIFI_SSID                           "Wokwi-GUEST"
#define WIFI_PWD                            ""
#define THINGSPEAK_CHANNEL_NUMBER           2989209
#define THINGSPEAK_API_KEY                  "MDL8KPO53VQS881Z"
#define THINGSPEAK_SERVER                   "api.thingspeak.com"

// Pins
#define AD0_PIN      15
#define NTC_PIN      2      // ADC2 CH2
#define PIR_PIN      4
#define HC_TRIG      18
#define HC_ECHO      19
#define BUZZ_PIN     5

// Address
#define I2C_MPU6050  0x68
#define I2C_OLED     0x3C

// Constants
#define NTC_BETA  3950

#endif