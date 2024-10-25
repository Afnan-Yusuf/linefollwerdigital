#include <Arduino.h>
#include <motorinit.h>

const int irPins[8] = {4, 16, 17, 18, 32, 35, 34, 39}; // Adjust according to your setup

// Motor pins

// PID constants
float Kp = 1.5;
float Ki = 0.00;
float Kd = 0.01;

// PID variables
float error = 0;
float previousError = 0;
float integral = 0;
float derivative = 0;

// Base motor speed
int baseSpeed = 150;
int maxSpeed = 255;
int minSpeed = 0;

// Line tracking variables
int sensorWeight[8] = {-4, -3, -2, -1, 1, 2, 3, 4}; // Position weights for each sensor
int lastPosition = 0;
int readLinePosition();
void setup() {
  InitMot();

   Serial.begin(115200);

  // Initialize sensor pins
  for (int i = 0; i < 8; i++) {
    pinMode(irPins[i], INPUT);
  }
}

void loop() {
  int position = readLinePosition();
  
  // PID control
  error = position;
  integral += error;
  derivative = error - previousError;
  float correction = (Kp * error) + (Ki * integral) + (Kd * derivative);

  // Motor speed adjustments
  int leftSpeed = baseSpeed - correction;
  int rightSpeed = baseSpeed + correction;

  // Set motor speeds
  if(leftSpeed > maxSpeed) leftSpeed = maxSpeed;
  if(leftSpeed < minSpeed) leftSpeed = minSpeed;
  if(rightSpeed > maxSpeed) rightSpeed = maxSpeed;
  if(rightSpeed < minSpeed) rightSpeed = minSpeed;
  if(leftSpeed > 0){
    leftmotforward(leftSpeed);
  }else{
    leftmotbackward(abs(leftSpeed));
  }

  if(rightSpeed > 0){
    rightmotforward(rightSpeed);
  }else{
    rightmotbackward(abs(rightSpeed));
  }

  previousError = error;

  delay(10); // Adjust as necessary for smoother control
  
}

int readLinePosition() {
  int weightedSum = 0;
  int activeSensorCount = 0;

  for (int i = 0; i < 8; i++) {
    int sensorValue = digitalRead(irPins[i]);
    if (sensorValue == LOW) { // Assuming LOW when on line
      weightedSum += sensorWeight[i];
      activeSensorCount++;
    }
  }

  // Return weighted average if any sensor is active, else return last position
  return (activeSensorCount > 0) ? (weightedSum / activeSensorCount) : lastPosition;
}