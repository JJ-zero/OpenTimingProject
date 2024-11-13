#include <ESP8266WiFi.h>

const String open_ssid = "OpenTimingProject";
const String private_ssid = "OpenTimingProjectEvent"; // Eventualy load this from EPROM
int mode = 0;
// Mode 0 - Base mode - Hosting AP and timming server
// Mode 1 - Satelite mode - Connecting to open WiFi and working as a satelite
// Mode 2 - Private satelite mode - Connecting to private WiFi. May be in "high performance mode"

int wifi_scan() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println(F("Starting WiFi scan..."));

  int scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
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

void setupSatelite() {}



void setup() {
  // Check WiFi
  // - Setup for debug
  Serial.begin(115200);
  Serial.println(F("\nScannig for known hotspots..."));
  mode = wifi_scan();
  Serial.print("WiFi scan:");  
  Serial.println(mode);


}

void loop() {
  // put your main code here, to run repeatedly:

}
