/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for ESP32

  Copyright (c) 2017 Hristo Gochkov. All rights reserved.
  This file is part of the ESP32 WiFi library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

//how many clients should be able to telnet to this ESP32
#define MAX_SRV_CLIENTS 3
const char* ssid = "AndroidAP";
const char* password = "jxri3115";
//const char* ssid = "HOME-3FE7";
//const char* password = "4E467053ED711202";

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

int pos = 0;
int sensor1 = 1000;
int sensor2 = 532;
int actuador;

void setup() {
  Serial.begin(115200);
  Serial.println("\nConnecting");

  wifiMulti.addAP(ssid, password);


  Serial.println("Connecting Wifi ");
  for (int loops = 10; loops > 0; loops--) {
    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.print("WiFi connected ");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    else {
      Serial.println(loops);
      delay(1000);
    }
  }
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    delay(1000);
    ESP.restart();
  }

  //start UART and the server
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
}

void loop() {
  uint8_t i;
  if (wifiMulti.run() == WL_CONNECTED) {
    //check if there are any new clients
    if (server.hasClient()) {
      for (i = 0; i < MAX_SRV_CLIENTS; i++) {
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()) {
          if (serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          if (!serverClients[i]) Serial.println("available broken");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(serverClients[i].remoteIP());
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        server.available().stop();
      }
    }
    //check clients for data
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        if (serverClients[i].available()) {
          //get data from the telnet client and push it to the UART
          while (serverClients[i].available()) {
            String dataSet = serverClients[i].readStringUntil(':');
            if (dataSet == "Actuador") {
              String value = serverClients[i].readStringUntil('\r');
              if (value == "on") {
                serverClients[i].print("Actuador");
                serverClients[i].println(" Encendido");
              }
              if (value == "off") {
                serverClients[i].print("Actuador");
                serverClients[i].println(" Apagado");
              }
            }
            else if (dataSet == "Sensor 1") {
              serverClients[i].print("Sensor 1: ");
              serverClients[i].println(sensor1);
              serverClients[i].readStringUntil('\r');
            }
            else if (dataSet == "Sensor 2") {
              serverClients[i].print("Sensor 2: ");
              serverClients[i].println(sensor2);
              serverClients[i].readStringUntil('\r');
            } else {
              //serverClients[i].println("Commando erroneo");
              serverClients[i].readStringUntil('\r');
            }
            Serial.print(dataSet);
            for (int j = 0; j < MAX_SRV_CLIENTS; j++) {
              if (serverClients[j] && serverClients[j].connected()) {
                if (dataSet[0] == 'A') {
                  if (dataSet[2] == '1') {
                    serverClients[i].write("Actuador encendido!");
                  } else if (dataSet[2] == '0')
                  {
                    serverClients[i].write("Actuador apagado!");
                  }
                }
              }
            }
          }
        }
      }
      else {
        if (serverClients[i]) {
          serverClients[i].stop();
        }
      }
    }
  }
  else {
    Serial.println("WiFi not connected!");
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i]) serverClients[i].stop();
    }
    delay(1000);
  }
}
