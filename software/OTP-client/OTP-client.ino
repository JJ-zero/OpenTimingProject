#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_NeoPixel.h>


ESP8266WiFiMulti WiFiMulti;
const int buttonPin = 16;
bool buttonLastState = true;
int nextCheck = 0;
bool running = false;
bool showingDisconnected = false;

Adafruit_NeoPixel pixels(18, D4, NEO_GRB + NEO_KHZ800);

const uint32_t colorDisconnected = pixels.Color(130, 112, 5);
const uint32_t colorBlack = pixels.Color(0, 0, 0);
const uint32_t colorStop = pixels.Color(10, 200, 15);


void startupAnimation() {
  pixels.fill(colorBlack);
  pixels.show();

  for (int i = 0; i < 12; i++) {
    delay(80);
    pixels.fill(colorBlack);
    int index = 3*(i%6);
    pixels.setPixelColor(index, pixels.Color(0,0,255));
    pixels.setPixelColor(index+1, pixels.Color(0,0,255));
    pixels.setPixelColor(index+2, pixels.Color(0,0,255));
    pixels.show();
  }

  delay(100);
  pixels.fill(colorBlack);
  pixels.show();
}

void setDisconnected() {
  if (showingDisconnected) { return; };
  pixels.fill(colorBlack);
  for (int i = 0; i < 6; i++) {
    pixels.setPixelColor((i*3) + (2 * (1 - (i % 2))), colorDisconnected);
  }
  pixels.show();
  showingDisconnected = true;
}


void setup() {
  pinMode(buttonPin, INPUT);
  
  pixels.begin();
  pixels.setBrightness(95);
  startupAnimation();

  setDisconnected();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("OpenTimingProject");
  Serial.begin(115200);  
}

void loop() {
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
        
        pixels.fill(colorStop);
        pixels.show();
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
          pixels.fill(pixels.Color(255,15,10));
          pixels.show();
        } else if (httpCode == HTTP_CODE_OK) {
          pixels.fill(colorStop);
          pixels.show();
        }
        http.end();
        nextCheck = millis() + 500;
      }
    }
  } else if (WiFiMulti.run() == WL_DISCONNECTED) {
    setDisconnected();
    delay(200);
  } else {
    setDisconnected();
    delay(200);
  }
}
