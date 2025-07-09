#include "buzzer.h"

void BUZZER::setup_buzzer () {
  pinMode(BUZZ_PIN, OUTPUT);
  // tone(BUZZ_PIN, 262, 1000); 
}

void BUZZER::output_buzzer() {
  tone(BUZZ_PIN, 262, 1000);
}

void BUZZER::stop_buzzer() {
  noTone(BUZZ_PIN);
}
