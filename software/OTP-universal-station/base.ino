#include <ESP8266WebServer.h>
#include "pages.h"


ESP8266WebServer webServer(80);
DNSServer dnsServer;


int lastRunStart = 0;
int lastRunId = 0;
int lastFinished = 0;


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

  webServer.on("/app/records", []() {
    String buffer;
    buffer.reserve(127);
    buffer = "{\"current\": ";
    if (lastRunStart == 0) {
      buffer += "0";
    } else {
      buffer += millis() - lastRunStart;
    }
    buffer += ", \"runId\": ";
    buffer += lastRunId;
    buffer += ", \"lastFinished\": ";
    buffer += lastFinished;
    buffer += "}";
    webServer.send(200, "json", buffer);
  });

  webServer.on("/api/gate", []() {
    int now = millis();
    if (lastRunStart != 0) {
      lastFinished = now - lastRunStart;
      lastRunStart = 0;
      lastRunId += 1;
    }
    webServer.send(200, "text", "Stopped");
      pixelsStop();
  });

  webServer.on("/api/state", []() {
    int now = millis();
    if (lastRunStart == 0) {
      webServer.send(200, "text", "None");
    } else {
      webServer.send(202, "text", "Running");
    }
  });

  webServer.begin();
  pixelsBase();
};

void loopBase() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  bool buttonValue = digitalRead(buttonPin);
  if (buttonValue != buttonLastState) {
    if (!buttonValue) {
      lastRunStart = millis();
      pixelsRun();
    }
    buttonLastState = buttonValue;
  }
};