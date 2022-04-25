#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif

#define DIN PB3
#define DT  PB2
#define CLK PB1
#define SW  PB4

Adafruit_NeoPixel pixels(1, DIN, NEO_GRB + NEO_KHZ800);

byte red = 255;
byte blue = 255;
byte green = 255;
int strobe = 23; 

int lastClk = HIGH;
byte count = 0;
unsigned long modeLastChanged = 0;

typedef enum {
  SET_RED,
  SET_GREEN,
  SET_BLUE,
  SET_STROBE,
} Mode;

Mode mode = SET_RED;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pixels.begin();
}

void loop() {
  pixels.setPixelColor(0, pixels.Color(red, green, blue));
  pixels.show();
  delayMicroseconds(strobe);
  pixels.clear();
  pixels.show();
  delay(strobe);

  int newClk = digitalRead(CLK);
  if (newClk != lastClk) {
    lastClk = newClk;
    int dtValue = digitalRead(DT);
    if (newClk == LOW && dtValue == HIGH) {
      count++;
    }
    if (newClk == LOW && dtValue == LOW) {
      count--;
    }
    switch (mode) {
      case SET_RED:
        red = count;
        break;
      case SET_GREEN:
        green = count;
        break;
      case SET_BLUE:
        blue = count;
        break;       
      case SET_STROBE:
        strobe = constrain(count, 0, 100);
        break;
    }
  }
  
  if (digitalRead(SW) == LOW && millis() - modeLastChanged > 300) {
    modeLastChanged = millis();
    count = 0;
    switch (mode) {
      case SET_RED:
        mode = SET_GREEN;
        break;
      case SET_GREEN:
        mode = SET_BLUE;
        break;
      case SET_BLUE:
        mode = SET_STROBE;
        break;       
      case SET_STROBE:
        mode = SET_RED;
        break;
    }
  }
}
