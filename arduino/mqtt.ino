#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

int rate = RATE;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
bool isPlay = false;
long startPlay = 0;
char msg[50];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  digitalWrite(BUILTIN_LED, HIGH);   
  if(strcmp(topic, "tx") == 0) {
     Serial.print("-- tx --> ");
      for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
      }
  } else if(strcmp(topic, "list") == 0 ) {
      Serial.println("--> LIST --> ");
      client.publish("connect", String(ESP.getChipId()).c_str());
      client.publish("info", ("RATE" + String(RATE)).c_str());
  } else if(strcmp(topic, "rate") == 0 ) {
       Serial.println("RATE -->  ");
       int myRate = atoi((char*)payload);
       Serial.println(myRate);
       if(myRate != rate) {
          Serial.println("BINGOOOO");       
       }
       for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
      }
  } 
  Serial.println();
  digitalWrite(BUILTIN_LED, LOW);   
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), USER, PASS) ) {
      client.publish("connect", String(ESP.getChipId()).c_str());
      client.publish("info", ("RATE" + String(RATE)).c_str());
      client.subscribe("tx");
      client.subscribe("list");
      client.subscribe("rate");
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void connect_wifi() {
   delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT); 
  digitalWrite(BUILTIN_LED, LOW);   
  Serial.begin(rate); 
  connect_wifi();
  client.setServer(MQTT, PORT);
  client.setCallback(callback);
}

void loop() {
        if (Serial.available() > 0) {
                byte incomingByte = Serial.read();
                client.publish("rx", String(incomingByte, DEC).c_str());
        }
        if (!client.connected()) {
          reconnect();
        }
        client.loop();
} 
