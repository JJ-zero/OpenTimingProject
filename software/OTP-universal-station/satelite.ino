#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>


ESP8266WiFiMulti WiFiMulti;


void loopSatelite() {
};

void setupSatelite() {
  Serial.println("Setting up satelite...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("OpenTimingProject");
};

