#include "pir.h"


PIR::PIR(){
  pirState = LOW;
}

void PIR::setup_PIR () {
  pinMode(PIR_PIN, INPUT);
}

void PIR::read_PIR () {
  int val = digitalRead(PIR_PIN);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      Serial.println("Motion detected!");
      // data->motion = true;
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      Serial.println("Motion ended!");
      // data->motion = false;
      pirState = LOW;
    }
  }
}