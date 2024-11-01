#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "pages.h"


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 255, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);
const int buttonPin = 16;
const String SSID = "OpenTimingProject";

int lastRunStart = 0;
int lastRunId = 0;
int lastFinished = 0;
bool buttonLastState = false;

Adafruit_NeoPixel pixels(18, D4, NEO_GRB + NEO_KHZ800);

const uint32_t colorBlack = pixels.Color(  0,   0,   0);
const uint32_t colorLoad  = pixels.Color(255,   0, 255);
const uint32_t colorRun   = pixels.Color(200,   0,   0);
const uint32_t colorStop  = pixels.Color( 15, 250,   0);


void startupAnimation() {
  pixels.fill(colorBlack);
  pixels.show();

  for (int i = 0; i < 12; i++) {
    delay(80);
    pixels.fill(colorBlack);
    int index = 3*(i%6);
    pixels.setPixelColor(index, colorLoad);
    pixels.setPixelColor(index+1, colorLoad);
    pixels.setPixelColor(index+2, colorLoad);
    pixels.show();
  }

  delay(100);
  pixels.fill(colorBlack);
  pixels.show();
}

void setup() {
  pinMode(buttonPin, INPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

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
    pixels.fill(colorStop);
    pixels.show();
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
  pixels.begin();
  pixels.setBrightness(90);
  startupAnimation();
  
  pixels.fill(colorStop);
  pixels.show();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  bool buttonValue = digitalRead(buttonPin);
  if (buttonValue != buttonLastState) {
    if (!buttonValue) {
      lastRunStart = millis();
      pixels.fill(colorRun);
      pixels.show();
    }
    buttonLastState = buttonValue;
  }
}
