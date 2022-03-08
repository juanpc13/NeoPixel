#include <SimpleSleep.h>
SimpleSleep Sleep; 

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   0
#define PIXEL_PIN    1
#define PIXEL_COUNT 1
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int mode = 0;
boolean oldState = HIGH;

// Bucles globales
int i = 0;
int b = 0;
long firstPixelHue = 0;
// Timing
unsigned long lastTime = 0;

void interruptHandler(){
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  strip.begin();
  strip.show();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interruptHandler, LOW);
}

void loop() {
  boolean newState = digitalRead(BUTTON_PIN);

  if ((newState == LOW) && (oldState == HIGH)) {
    delay(100);
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {     // Yes, still low
      if (++mode > 11) mode = 0; // Advance to next mode, wrap around after #11

    }
  }

  switch (mode) {          // Start the new animation...
    case 0:
      strip.clear();
      strip.show();
      sleepIn(10000);// Dormir 10 segundos despues de apagado
      break;
    case 1:
      colorWipe(strip.Color(255, 255, 255), 50);    // White
      break;
    case 2:
      colorWipe(strip.Color(255,   0,   0), 50);    // Red
      break;
    case 3:
      colorWipe(strip.Color(  0, 255,   0), 50);    // Green
      break;
    case 4:
      colorWipe(strip.Color(  0,   0, 255), 50);    // Blue
      break;
    case 5:
      theaterChase(strip.Color(255, 255, 255), 60); // White
      break;
    case 6:
      theaterChase(strip.Color(255,   0,   0), 60); // Red
      break;
    case 7:
      theaterChase(strip.Color(0,   255,   0), 60); // Green
      break;
    case 8:
      theaterChase(strip.Color(  0,   0, 255), 60); // Blue
      break;
    case 9:
      rainbow(60);
      break;
    case 10:
      // Mantiene el ultimo estado del anterior
      break;
    case 11:
      theaterChaseRainbow(60);
      break;
  }
  
  oldState = newState;
}

void sleepIn(int wait) {
  unsigned long dif = millis() - lastTime;
  Serial.println(dif);
  if (dif >= wait){
    // Do Sleep
    Serial.println("sleepNow");
    delay(100);
    Sleep.deeply();
  }
}

void colorWipe(uint32_t color, int wait) {
  if (millis() - lastTime >= wait){
    strip.setPixelColor(i, color);
    strip.show();
    if (++i >= strip.numPixels()) i = 0;
    lastTime = millis();
  }  
}

void theaterChase(uint32_t color, int wait) {
  if (millis() - lastTime >= wait){

    strip.clear();
    for (int c = b; c < strip.numPixels(); c += 3) {
      strip.setPixelColor(c, color);
    }
    strip.show();
    
    if (++b >= 3) b = 0;
    lastTime = millis();
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  if (millis() - lastTime >= wait){

    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();

    firstPixelHue += 256;
    if (firstPixelHue >= 3 * 65536) firstPixelHue = 0;
    lastTime = millis();
  }
}

void theaterChaseRainbow(int wait) {
  if (millis() - lastTime >= wait){

    strip.clear();
    for (int c = b; c < strip.numPixels(); c += 3) {
      int hue = firstPixelHue + c * 65536L / strip.numPixels();
      uint32_t color = strip.gamma32(strip.ColorHSV(hue));
      strip.setPixelColor(c, color);
    }
    strip.show();


    firstPixelHue += 65536 / 90;
    if (++b >= 3) b = 0;
    lastTime = millis();    
  }
  
}
