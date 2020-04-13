/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define STASSID "AndroidAP"
#define STAPSK  "jxri3115"
//#define STASSID "HOME-3FE7"
//#define STAPSK  "4E467053ED711202"


const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.43.41";
const uint16_t port = 5204;

uint8_t select = 0;

ESP8266WiFiMulti WiFiMulti;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {
  //  Serial.print("connecting to ");
  //  Serial.print(host);
  //  Serial.print(':');
  //  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  switch (select) {
    case 0:
      if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
      } else {
        select = 1;
      }
      break;

    case 1:

      if (Serial.available()) {
        // This will send the request to the server
        char msg = (char)Serial.read();
        if (msg == '&') {
          String sensor = "Sensor1 > 1023";
          client.println(sensor);
        } else {
          client.print(msg);
        }
      }

      //read back one line from server
      //Serial.println("receiving from remote server");
      if (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.println(line);
      }
      break;
  }
}
