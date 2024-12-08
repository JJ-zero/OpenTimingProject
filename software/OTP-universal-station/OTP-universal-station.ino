#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include "settings.h"


int mode = 0;
// Mode 0 - Base mode - Hosting AP and timming server
// Mode 1 - Satelite mode - Connecting to open WiFi and working as a satelite
// Mode 2 - Private satelite mode - Connecting to private WiFi. May be in "high performance mode"
const int buttonPin = 16;

// Base logic variables
int lastRunStart = 0;
int lastRunId = 0;
int lastFinished = 0;
bool buttonLastState = false;


int wifi_scan() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println(F("Starting WiFi scan..."));

  int scanResult = WiFi.scanNetworks(
    false, // asynvc
    true // hidden
  );
  if (scanResult == 0) {
    return 0;
  }

  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;

  for (int8_t i = 0; i < scanResult; i++) {
    WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
    if (ssid == open_ssid) {
      return 1;
    }
    if (ssid == private_ssid) {
      return 2;
    }
  }
  return 0;
}


void setup() {
  Serial.begin(115200);

  Serial.println("\nInitialization of pixels...");
  init_pixels();
  pixelsBootAnimation();
  
  // Check WiFi
  // - Setup for debug
  Serial.println(F("\nScannig for known hotspots..."));
  
  mode = wifi_scan();
  Serial.print("WiFi scan:");  
  Serial.println(mode);
    
  pinMode(buttonPin, INPUT);

  switch (mode) {
    case 0: setupBase(); break;
    case 1: setupSatelite(); break;
    case 2: break;
  }
  Serial.println("\nSetup completed...");
}

void loop() {
  switch (mode) {
    case 0: loopBase(); break;
    case 1: loopSatelite(); break;
    case 2: break;
  }
  pixelsAnimate();
}
