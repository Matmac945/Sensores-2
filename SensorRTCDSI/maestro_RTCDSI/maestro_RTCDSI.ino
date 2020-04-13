#include <Wire.h>
#define INPUT_SIZE 30
byte SLAVE_ADDRESS = 104;
int sel = 0;
int select = 1;
int dir = 8;
int pos = 0;
int posData = 0;
int intPos = 0;
int x = 0, y = 0, z = 0, k = 0, w = 0;
char mensaje[30];
boolean num = 0;
char data[15];
char Inmsg[INPUT_SIZE + 1];
char temp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission();
}

void loop() {
  while (Serial.available()) {
    temp = Serial.read();
    if (temp != '\n' && temp != ':') {
      if (num == 0) {
        Inmsg[pos]  = temp;
        pos++;
      } else {
        if (temp != ' ') {
          data[posData] = temp;
          posData++;
        }
      }
    } else if (temp == ':') {
      num = 1;
      select = 0;
    }
  }
  data[posData + 1] = '*';
  if (temp == '\n') {
    String msg = Inmsg;
    switch (select) {
      case 0:
        switch (sel) {
          case 0:
            if (data[intPos] == ',') {
              sel = 1;
              intPos++;
            } else {
              x = x * 10 + data[intPos] - 48;
              intPos++;
            }
            break;
          case 1:
            if (data[intPos] == '*') {
              sel = 4;
            }
            else if (data[intPos] == ',') {
              sel = 2;
              intPos++;
            } else {
              mensaje[w++] = data[intPos];
              y = y * 10 + data[intPos] - 48;
              intPos++;
            }
            break;
          case 2:
            if (data[intPos] == ',') {
              sel = 3;
              intPos++;
            } else {
              z = z * 10 + data[intPos] - 48;
              intPos++;
            }
            break;
          case 3:
            if (data[intPos] == '*') {
              sel = 4;
              intPos++;
            } else {
              k = k * 10 + data[intPos] - 48;
              intPos++;
            }
            break;
          case 4:
            select = 1;
            break;
        }
        break;
      case 1:
        if (msg == "look for sensor") {
          lookForSensor();
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
        }
        else if (msg == "set time ") {
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
          setTime();
        }
        else if (msg == "set date ") {
          setDate();
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
        }
        else if (msg == "read time") {
          readTime();
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
        }
        else if (msg == "read date") {
          readDate();
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
        }
        else if (msg == "save in RAM ") {
          saveRAM();
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
        }
        else if (msg == "read RAM ") {
          readRAM();
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          pos  = 0;
          num = 0;
          posData  = 0;
          temp = 0;
        }
        else {
          temp = 0;
          Serial.print(Inmsg);
          Serial.println(" is not a valid command");
          for (int i = 0; i < pos; i++) {
            Inmsg[i] = 0;
          }
          for (int i = 0; i < posData; i++) {
            data[i] = 0;
          }
          for (int i = 0; i < w; i++) {
            mensaje[i] = 0;
          }
          pos = 0;
          num = 0;
          posData  = 0;
        }
        sel = 0;
        intPos = 0;
        break;
    }
  }
}

void lookForSensor() {
  for (int i = 0; i < 128; i++) {
    Wire.beginTransmission(i);
    Wire.write(1);
    int success = Wire.endTransmission();
    if (success == 0) {
      Serial.println("Sensor is conected!");
      Serial.print("Sensor address: ");
      Serial.println(i);
    }
  }
}

void setTime() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(0);                                    // address
  Wire.write(z + (6 * int(z / 10)));                // seconds
  Wire.write(y + (6 * int(y / 10)));              // minutes
  Wire.write((x + (6 * int(x / 10)) + (64 * k))); // hours and PM/AM
  Wire.endTransmission(true);
  Serial.println("Time has been set");
  x = 0;
  y = 0;
  z = 0;
  k = 0;
  w = 0;
  for (int i = 0; i < w; i++) {
    mensaje[i] = 0;
  }
}

void setDate() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(3);                       // address
  Wire.write(x + (6 * int(x / 10)));   // week day
  Wire.write(y + (6 * int(y / 10))); // date
  Wire.write(z + (6 * int(z / 10)));   // month
  Wire.write(k + (6 * int(k / 10))); // year
  Wire.endTransmission();
  Serial.println("Date has been set");
  x = 0;
  y = 0;
  z = 0;
  k = 0;
  w = 0;
  for (int i = 0; i < w; i++) {
    mensaje[i] = 0;
  }
}

void readTime() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(SLAVE_ADDRESS, 3);
  byte c[3] ;
  while (Wire.available()) {
    for (int i = 0; i < 3; i++) {
      c[i] = Wire.read();
    }
    byte timeSelect = ((c[2] >> 6) & 1);
    byte Dhour;
    byte AmPm;
    if (timeSelect == 1) {
      Dhour = ((c[2] >> 4) & 1);
      AmPm = ((c[2] >> 5) & 1);
    } else {
      Dhour = ((c[2] >> 4) & 3);
    }
    byte hour = (c[2] & 15);
    byte minutes = (c[1] & 15);
    byte Dminutes = ((c[1] >> 4) & 7);
    byte seconds = (c[0] & 15);
    byte Dseconds = ((c[0] >> 4) & 7);
    Serial.print(Dhour);
    Serial.print(hour);
    Serial.print(":");
    Serial.print(Dminutes);
    Serial.print(minutes);
    Serial.print(":");
    Serial.print(Dseconds);
    Serial.print(seconds);
    if (timeSelect != 0) {
      if (AmPm == 1) {
        Serial.println(" PM");
      } else {
        Serial.println(" AM");
      }
    } else {
      Serial.println("");
    }
  }
}

void readDate() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(3);
  Wire.endTransmission();
  Wire.requestFrom(SLAVE_ADDRESS, 4);
  byte c[4] ;
  while (Wire.available()) {
    for (int i = 0; i < 4; i++) {
      c[i] = Wire.read();
    }
    byte day = (c[0] & 7);
    byte Ddate = ((c[1] >> 4) & 3);
    byte date = (c[1] & 15);
    byte Dmonth = ((c[2] >> 4) & 1);
    byte month = (c[2] & 15);
    byte Dyear = ((c[3] >> 4) & 15);
    byte year = (c[3] & 15);
    Serial.print(day);
    Serial.print("/");
    Serial.print(Ddate);
    Serial.print(date);
    Serial.print("/");
    Serial.print(Dmonth);
    Serial.print(month);
    Serial.print("/");
    Serial.print(Dyear);
    Serial.println(year);
  }
}

void saveRAM() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(x + 8);
  Wire.write(mensaje);
  Wire.endTransmission();
  Serial.println("info has been saved in RAM");
  x = 0;
  y = 0;
  z = 0;
  k = 0;
  w = 0;
  for (int i = 0; i < w; i++) {
    mensaje[i] = 0;
  }
}

void readRAM() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(x + 8);
  Wire.endTransmission();
  char c;
  Wire.requestFrom(SLAVE_ADDRESS, y);
  while (Wire.available()) {
    c = Wire.read();
    Serial.print(c);
  }
  x = 0;
  y = 0;
  z = 0;
  k = 0;
  w = 0;
  for (int i = 0; i < w; i++) {
    mensaje[i] = 0;
  }
}
