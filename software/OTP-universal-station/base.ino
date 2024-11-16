#include <ESP8266WebServer.h>
#include "pages.h"


ESP8266WebServer webServer(80);
DNSServer dnsServer;



void setupBase() {
  Serial.println("Starting base...");

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(serverIp, serverIp, APmask);
  WiFi.softAP(open_ssid);

  dnsServer.start(DNS_PORT, "*", serverIp);

  webServer.enableCORS(true);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  
  webServer.begin();
};

void loopBase() {
  dnsServer.processNextRequest();
  webServer.handleClient();
};