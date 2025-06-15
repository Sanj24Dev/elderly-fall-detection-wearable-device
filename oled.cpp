#include "oled.h"

void OLED::setup_OLED (uint8_t addr) {
  oled.begin(&Adafruit128x64, addr);
  oled.setFont(System5x7);
  oled.setContrast(255);
  oled.clear();
  oled.print("Hello world!");
}

void OLED::print_message(const char *msg) {
  oled.clear();
  oled.print(msg);
}