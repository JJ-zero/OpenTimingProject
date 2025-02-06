#include <ESP8266WebServer.h>
#include "pages.h"
#include <ArduinoOTA.h>


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

  webServer.on("/api/battery", []() {
    int value = analogRead(BATTERY_PIN);
    int range = BATTERY_FULL - BATTERY_EMPTY;
    int percentage = ((value - BATTERY_EMPTY) * 100) / range;
    String buffer;
    buffer.reserve(127);
    buffer = "{\"raw\": ";
    buffer += value;
    buffer += ", \"percentage\": ";
    buffer += percentage;
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


  ArduinoOTA.setHostname("OTP base");
  ArduinoOTA.setPassword("optota"); 
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    pixelsBootAnimation();
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    pixelsUpdateProgress(((progress * 2) / (total / 128)));
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  webServer.begin();
  ArduinoOTA.begin();

  tone(D5, 6400, 50);
  pixelsBase();
  delay(90);
  tone(D5, 6400, 50);
  delay(90);
  tone(D5, 6400, 50);
};

void loopBase() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  ArduinoOTA.handle();
  bool buttonValue = digitalRead(buttonPin);
  if (buttonValue != buttonLastState) {
    if (!buttonValue) {
      lastRunStart = millis();
      pixelsRun();
    }
    buttonLastState = buttonValue;
  }
};