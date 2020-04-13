#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "AndroidAP";
const char* password = "jxri3115";
WiFiUDP udpDevice;
uint16_t localUdpPort = 4210;


uint32_t previousMillis = 0;
#define ALIVE 1000

void setup() {
  pinMode(D0, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(D0, HIGH);
  pinMode(D8, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(D8, LOW);
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  udpDevice.begin(localUdpPort);
}


void networkTask() {
  uint8_t data;
  uint8_t packetSize = udpDevice.parsePacket();
  if (packetSize) {
    data = udpDevice.read();
    if (data == '1') {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(D8, HIGH);
    } else if (data == '0') {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(D8, LOW);
    }
    // send back a reply, to the IP address and port we got the packet from
    udpDevice.beginPacket(udpDevice.remoteIP(), udpDevice.remotePort());
    udpDevice.write('1');
    udpDevice .endPacket();
  }
}

void aliveTask() {
  uint32_t currentMillis;
  static uint8_t ledState = 0;
  currentMillis  = millis();
  if ((currentMillis - previousMillis) >= ALIVE) {
    previousMillis = currentMillis;
    if (ledState == 0) digitalWrite(D0, HIGH);
    else digitalWrite(D0, LOW);
  }
}

void loop() {
  networkTask();
  //  aliveTask();
}
