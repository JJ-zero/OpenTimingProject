#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(18, D4, NEO_GRB + NEO_KHZ800);

bool showingDisconnected = false;
int last_frame = 0;
int last_frame_state = 0;
int active_animation = 0;

const uint32_t colorBlack = pixels.Color(0, 0, 0);
const uint32_t colorStartup = pixels.Color(100, 0, 200);
const uint32_t colorDisconnected = pixels.Color(130, 112, 5);
const uint32_t colorStop = pixels.Color(10, 200, 15);

void init_pixels() {
  pixels.begin();
  pixels.setBrightness(20);
  pixels.fill(colorBlack);
  pixels.show();
}

void pixelsSetDisconnected() {
  if (showingDisconnected) { return; };
  pixels.fill(colorBlack);
  for (int i = 0; i < 6; i++) {
    pixels.setPixelColor((i*3) + (2 * (1 - (i % 2))), colorDisconnected);
  }
  pixels.show();
  showingDisconnected = true;
}


void pixelsBootAnimation() {
  char g[] = {
    0, 5, 6, 11, 12, 17,
    1, 4, 7, 10, 13, 16,
    2, 3, 8,  9, 14, 15
  };
  pixels.clear();
  for (int i = 0; i < 18; i++) {
    pixels.setPixelColor(g[i], colorStartup);
    pixels.show();
    delay(75);
  }
}


void pixelsAnimate() {
  switch active_animation {
    case 1:
      // Disconnected
      int now = millis();
      if (now > 250 + last_frame) {
        if (last_frame_state == 0) {
          showingDisconnected = false;
          pixels.clear();
          pixels.show();
        } else {
          pixelsSetDisconnected();
        }
        last_frame_state = 1 - last_frame_state;
        last_frame = now;
      }
    break;
    default: break;
  }
}