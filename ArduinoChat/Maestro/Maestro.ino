#include <Wire.h>

const byte SLAVE_ADDRESS = 42;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  Wire.requestFrom(SLAVE_ADDRESS, 1);

  while (Wire.available()) {
    char c = Wire.read();
    if (c != 0) {
      Serial.print(c);
    }
  }

  if (Serial.available()) {
    Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.write(Serial.read());
    Wire.endTransmission();
  }
}
