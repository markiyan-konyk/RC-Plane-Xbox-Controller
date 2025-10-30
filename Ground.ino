// Xbox Controller Data Receiver for Arduino
// Receives data from C++ XInput program via Serial

#include <SoftwareSerial.h>
SoftwareSerial HC12(10,11); // or whatever is RX and TX


// Structure to hold all controller data
struct ControllerData {
  float LX;
  float LY;
  float RX;
  float RY;
  float Trig;
  bool R3;
  bool L3;
  bool A;
  bool B;
  bool X;
  bool Y;
  bool DU;
  bool DD;
  bool DR;
  bool DL;
  bool RB;
  bool LB;
};

ControllerData Data;
String inputBuffer = "";

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  
  // Initialize all values to defaults
  Data.A = false;
  Data.B = false;
  Data.Y = false;
  Data.X = false;
  Data.RB = false;
  Data.LB = false;
  Data.R3 = false;
  Data.L3 = false;
  Data.DU = false;
  Data.DD = false;
  Data.DR = false;
  Data.DL = false;
  Data.Trig = 0.0;
  Data.LX = 0.0;
  Data.LY = 0.0;
  Data.RX = 0.0;
  Data.RY = 0.0;
  
}

void loop() {
  
  // The following Loop reads the HC12 serial char by char until \n
  // after which it updates the variables and resets input Buffer
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == '\n') {
      parseControllerData(inputBuffer);
      inputBuffer = "";
    } else {
      inputBuffer += inChar;
    }
  }

  HC12.print("T:");
  HC12.print(Data.Trig, 2);
  HC12.print(",R:");
  HC12.print(Data.LX, 2);
  HC12.print(",P:");
  HC12.print(Data.LY, 2);
  HC12.print(",A:");
  HC12.print(Data.A);
  HC12.print(",B:");
  HC12.println(Data.B);

  // delay is used as constant Serial traffic is not needed
  delay(20);
}

void parseControllerData(String data) {
  Data.Trig = getFloatValue(data, "Trig:");
  Data.LX = getFloatValue(data, "LX:");
  Data.LY = getFloatValue(data, "LY:");
  Data.RX = getFloatValue(data, "RX:");
  Data.RY = getFloatValue(data, "RY:");
  Data.L3 = getBoolValue(data, "L3:");
  Data.R3 = getBoolValue(data, "R3:");
  Data.X = getBoolValue(data, "X:");
  Data.Y = getBoolValue(data, "Y:");
  Data.A = getBoolValue(data, "A:");
  Data.B = getBoolValue(data, "B:");
  Data.DD = getBoolValue(data, "DD:");
  Data.DU = getBoolValue(data, "DU:");
  Data.DR = getBoolValue(data, "DR:");
  Data.DL = getBoolValue(data, "DL:");
  Data.RB = getBoolValue(data, "RB:");
  Data.LB = getBoolValue(data, "LB:");
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
