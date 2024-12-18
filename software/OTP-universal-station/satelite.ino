#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>


ESP8266WiFiMulti WiFiMulti;

int nextCheck = 0;


void loopSatelite() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    if (showingDisconnected) { showingDisconnected = false; }
    bool buttonValue = digitalRead(buttonPin);
    if (buttonValue != buttonLastState) {
      buttonLastState = buttonValue;
      Serial.println("New button value");
      Serial.println(buttonValue);
      if (buttonValue) {
        WiFiClient client;
        HTTPClient http;

        http.setTimeout(500);
        if (http.begin(client, "http://192.168.255.1/api/gate")) {  // HTTP
          int httpCode = http.GET();

          // httpCode will be negative on error
          if (httpCode > 0) {
            // file found at server
            // if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {}
          }

          http.end();
        }
        
        pixelsStop();
      }
    }
    if (nextCheck < millis()) {
      WiFiClient client;
      HTTPClient http;

      if (http.begin(client, "http://192.168.255.1/api/state")) {  // HTTP
        int httpCode = http.GET();
        // httpCode will be negative on error
        // if (httpCode > 0) {
          // file found at server
          // if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {}
        // }
        if (httpCode == HTTP_CODE_ACCEPTED) {
          pixelsRun();
        } else if (httpCode == HTTP_CODE_OK) {
          pixelsStop();
        }
        http.end();
        nextCheck = millis() + 500;
      }
    }
  } else if (WiFiMulti.run() == WL_DISCONNECTED) {
    pixelsSetDisconnected();
    delay(200);
  } else {
    pixelsSetDisconnected();
    delay(200);
  }
};

void setupSatelite() {
  Serial.println("Setting up satelite...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("OpenTimingProject");
  pixelsSetDisconnected();
  tone(D5, 6400, 80);
  delay(160);
  tone(D5, 6400, 80);
};

