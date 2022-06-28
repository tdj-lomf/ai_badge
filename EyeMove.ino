#include <Servo.h>
#include "EyeLink.h"
#include "main.h"

Servo servo[4];
EyeLink eyeLink;

// define min and max values
// 1
/*
const double exMax = 3.0;
const double eyMax = 3.0;
const double exOffset = 0.0;
const double eyOffset = 0.0;
const double uOpen = 3.5;
const double uClose = -4.0;
const double lOpen = -3.5;
const double lClose = 2.4;
*/

// 2
const double exMax = 3.0;
const double eyMax = 2.0;
const double exOffset = 1.0;
const double eyOffset = 0.0;
const double uOpen = 2.0;
const double uClose = -3.5;
const double lOpen = -3.5;
const double lClose = 3.3;

double uState = 0.0;  // upper eyelid position state[%]
double lState = 0.0;  // lower eyelid position state[%]
double exState = 0.0; // eye x position state[%]
double eyState = 0.0; // eye y position state[%]


void setup_servo() {
  servo[0].attach(9);   // eyelid lower
  servo[1].attach(10);  // eye x
  servo[2].attach(7);   // eye y
  servo[3].attach(8);   // eyelid upper

  pinMode(M_POWER, OUTPUT);  // motor power supply (High: On)

  // move to initial position
  setMotorPower(true);
  moveEye(0.0, 0.0);
  moveEyelid(uOpen, lOpen);
  delay(500);  
  setMotorPower(false);
}

void setMotorPower(bool on) {
  if (on) {
    digitalWrite(M_POWER, HIGH);
  } else {
    digitalWrite(M_POWER, LOW);    
  }
}

double getUState() {
  return uState;
}

double getLState() {
  return lState;
}

void moveEye(double ex, double ey) {
  double diffX, diffY;
  eyeLink.inverseKinema30(ex + exOffset, ey + eyOffset, diffX, diffY);
  const double angleX = 90.0 + diffX * 30 / 1.2;  // [mm] -> [deg]  
  const double angleY = 90.0 - diffY * 30 / 1.2;  // [mm] -> [deg]
  if (angleX < 0.0 || 180 < angleX) {
    Serial.println("angleX out of range");
    return;
  }
  if (angleY < 0.0 || 180 < angleY) {
    Serial.println("angleY out of range");
    return;
  }
  servo[1].write(angleX);
  servo[2].write(angleY);
}

void moveEyelid(double u_mm, double l_mm) {
  const double angleU = 90.0 + u_mm * 30 / 1.2;  // [mm] -> [deg]
  const double angleL = 90.0 - l_mm * 30 / 1.2;  // [mm] -> [deg]
  if (angleU < 0.0 || 180.0 < angleU) {
    Serial.println("angleU out of range");
    Serial.println(angleU);
    return;    
  }  
  if (angleL < 0.0 || 180.0 < angleL) {
    Serial.println("angleL out of range");
    Serial.println(angleL);
    return;    
  }
  servo[3].write(angleU);
  servo[0].write(angleL);
}

// ex_per: [-100.0, 100.0]
// ey_per: [-100.0, 100.0]
void moveEyeP(double ex_per, double ey_per) {
  const double ex = exMax * ex_per / 100.0;
  const double ey = eyMax * ey_per / 100.0;
  moveEye(ex, ey);
}

// u_per: [0.0, 100.0]
// l_per: [0.0, 100.0]
void moveEyelidP(double u_per, double l_per) {
  const double u_mm = u_per * (uClose - uOpen) / 100.0 + uOpen;
  const double l_mm = l_per * (lClose - lOpen) / 100.0 + lOpen;
  moveEyelid(u_mm, l_mm);
}

void moveEyeSync(double ex_per, double ey_per, int time_ms) {
  const int TIME_STEP = 1;  // [ms]
  int loopMax = time_ms / TIME_STEP;
  const int restTime = time_ms % TIME_STEP;
  if (restTime > 0) {
    ++loopMax;
  }

  const double xStep = (ex_per - exState) / loopMax;
  const double yStep = (ey_per - eyState) / loopMax;
  setMotorPower(true);
  for (int i = 0; i < loopMax; ++i) {
    exState += xStep;
    eyState += yStep;
    moveEyeP(exState, eyState);
    if (i == loopMax - 1 && restTime > 0) {
      delay(restTime);
    } else {
      delay(TIME_STEP);
    }
  }
  delay(100);
  setMotorPower(false);
}

void moveEyelidSync(double u_per, double l_per, int time_ms) {
  const int TIME_STEP = 1;  // [ms]
  int loopMax = time_ms / TIME_STEP;
  const int restTime = time_ms % TIME_STEP;
  if (restTime > 0) {
    ++loopMax;
  }

  const double uStep = (u_per - uState) / loopMax;
  const double lStep = (l_per - lState) / loopMax;
  setMotorPower(true);
  for (int i = 0; i < loopMax; ++i) {
    uState += uStep;
    lState += lStep;
    moveEyelidP(uState, lState);
    if (i == loopMax - 1 && restTime > 0) {
      delay(restTime);
    } else {
      delay(TIME_STEP);
    }
  }    
  delay(100);
  setMotorPower(false);
}

void moveEyeDiff(double diffx_per, double diffy_per, int time_ms) {
  double xTarget = exState + diffx_per;
  double yTarget = eyState + diffy_per;
  if (xTarget < -100.0) {
    xTarget = -100.0;
  } else if (xTarget > 100.0) {
    xTarget = 100.0;
  }
  if (yTarget < -100.0) {
    yTarget = -100.0;
  } else if (yTarget > 100.0) {
    yTarget = 100.0;
  }
  Serial.println(xTarget);
  Serial.println(yTarget);
  moveEyeSync(xTarget, yTarget, time_ms);
}

void moveEyelidDiff(double diffu_per, double diffl_per, int time_ms) {
  double uTarget = uState + diffu_per;
  double lTarget = lState + diffl_per;
  if (uTarget < 0.0) {
    uTarget = 0.0;
  } else if (uTarget > 100.0) {
    uTarget = 100.0;
  }
  if (lTarget < 0.0) {
    lTarget = 0.0;
  } else if (lTarget > 100.0) {
    lTarget = 100.0;
  }
  Serial.println(uTarget);
  Serial.println(lTarget);
  moveEyelidSync(uTarget, lTarget, time_ms);
}
