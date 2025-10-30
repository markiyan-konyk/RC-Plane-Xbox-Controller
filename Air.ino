#include <SoftwareSerial.h>
#include <Servo.h>
//#include <MPU6050.h>
//#include <Wire.h>
/*
  PINS USED
  HC12 - 2,3
  Motor - 9
  Servos - 6,10,11
*/

// Set up the components 
SoftwareSerial HC12(2,3);
Servo ESC;
Servo L_Flap;
Servo R_Flap;
Servo Rudder;
MPU6050 mpu;

// Our variables
String inputBuffer = "";
float Throttle = 0;
float Roll = 0;
float Pitch = 0;
bool Flaps = false;
// Accelerometre variables
//int16_t ax, ay, az;
//int16_t gx, gy, gz;
//float accelAngleX, accelAngleY;

void setup() {
  HC12.begin(9600);
  //Wire.begin();
  //mpu.initialize();
  /*
  if (!mpu)
  */

  ESC.attach(9, 1000, 2000);
  L_Flap.attach(10);
  R_Flap.attach(11);
  Rudder.attach(6);

  ESC.write(0);
  L_Flap.write(90);
  R_Flap.write(90);
  Rudder.write(90);
}

void loop() {

  // The following Loop reads the HC12 serial char by char until \n
  // after which it updates the variables and resets input Buffer
  while (HC12.available() > 0) {
    char inChar = HC12.read();
    if (inChar == '\n') {
      // Complete message received, parse it immediately
      Throttle = constrain(Throttle + (0.1 * getFloatValue(inputBuffer, "T:")), 0, 1);
      Roll = constrain(Roll + (0.25 * getFloatValue(inputBuffer, ",P:")), -1, 1);
      Pitch = constrain(Pitch + (0.25 * getFloatValue(inputBuffer, ",R:")), -1, 1);
      if (getBoolValue(inputBuffer, ",A:")) {
        Flaps = false;
      }
      if (getBoolValue(inputBuffer, ",B:")) {
        Flaps = true;
      }
      inputBuffer = "";  // Clear buffer for next message
    } else {
      inputBuffer += inChar;
    }
  }

  // You cannot use map with floats, so I convert them to ints in place
  ESC.write(map((int)(1000 * Throttle), 0, 1000, 0, 180));
  // At 0 degrees the flaps point up
  // The range is from 45 to 135 degrees to allow a max of 45 degrees of rotation
  if (Flaps == false){
    L_Flap.write(map((int)(1000 * Roll), -1000, 1000, 45, 135));
    R_Flap.write(map((int)(-1000 * Roll), -1000, 1000, 45, 135));
  } else{
    R_Flap.write(135);
    L_Flap.write(135);
  }
  // At 0 degrees rudder points up, same range
  Rudder.write(map((int)(-1000 * Pitch), -1000, 1000, 45, 135));
}

float getFloatValue(String data, String key) {
  int startIndex = data.indexOf(key);
  if (startIndex == -1) return 0.0;

  startIndex += key.length();
  int endIndex = data.indexOf(' ', startIndex);
  if (endIndex == -1) endIndex = data.length();
  
  String valueStr = data.substring(startIndex, endIndex);
  return valueStr.toFloat();
}

bool getBoolValue(String data, String key) {
  int startIndex = data.indexOf(key);
  if (startIndex == -1) return false;
  
  startIndex += key.length();
  int endIndex = data.indexOf(' ', startIndex);
  if (endIndex == -1) endIndex = data.length();
  
  String valueStr = data.substring(startIndex, endIndex);
  return (valueStr.toInt() != 0);
}