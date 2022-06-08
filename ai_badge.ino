#include "main.h"

int inputValue;
int swIds[SW_SIZE];

void setup_io() {
  swIds[0] = SW1;
  swIds[1] = SW2;
  swIds[2] = SW3;
  swIds[3] = SW4;
  swIds[4] = SW5;
  swIds[5] = SW6;
  swIds[6] = SW7;
  swIds[7] = SW8;

  pinMode(LED2, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED3, OUTPUT);
  for (int i = 0; i < 8; ++i) {
    pinMode(swIds[i], INPUT_PULLUP);  // SW1 - 8
  }
}


void setup() {
  setup_io();
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  Serial.begin(9600);
  setup_servo();
  //setup_wifi();
  setup_ble();
}

  
void loop() {

  // check switch
  const int SW_TIME = 50;
  for (int i = 0; i < SW_SIZE; ++i) {
    inputValue = digitalRead(swIds[i]);
    if (inputValue == LOW) {
      Serial.print("pushed ");
      Serial.println(i);
      while (inputValue == LOW) {
        inputValue = digitalRead(swIds[i]); // wait switch release
      }
      switch (i) {
        case 0: moveEyeDiff(10.0, 0.0, SW_TIME); break;
        case 1: moveEyeDiff(-10.0, 0.0, SW_TIME); break;
        case 2: moveEyeDiff(0.0, 10.0, SW_TIME); break;
        case 3: moveEyeDiff(0.0, -10.0, SW_TIME); break;
        case 4: moveEyelidDiff(10.0, 0.0, SW_TIME); break;
        case 5: moveEyelidDiff(-10.0, 0.0, SW_TIME); break;
        case 6: moveEyelidDiff(0.0, 10.0, SW_TIME); break;
        case 7: moveEyelidDiff(0.0, -10.0, SW_TIME); break;
      }
    }
  }

  // loop_wifi();
  loop_ble();

}




