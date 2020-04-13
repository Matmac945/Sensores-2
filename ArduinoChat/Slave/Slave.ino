#include <Wire.h>

const byte MY_ADDRESS = 42;

uint8_t SENDING = 9;
uint8_t count = 0;
byte data;
boolean sender = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SENDING, OUTPUT);
  Wire.begin (MY_ADDRESS);
  for (byte i = 2; i <= 7; i++) {
    pinMode(i, OUTPUT);
  }
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  digitalWrite(SENDING, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    if (sender == false) {
      data = Serial.read();
      sender = true;
    }
  }

}
void receiveEvent(int howMany) {
  while (0 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
}

void requestEvent() {
  if (sender == true) {
    Wire.write(data);
    sender = false;
  } else {
    Wire.write(0);
  }
}
