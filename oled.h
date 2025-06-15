#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "main.h"

class OLED {
  public:
    SSD1306AsciiWire oled;

    void setup_OLED (uint8_t addr);
    void print_message(const char *msg);
};

