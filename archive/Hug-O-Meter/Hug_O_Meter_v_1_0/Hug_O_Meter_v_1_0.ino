#include <Adafruit_NeoPixel.h>
#include <Time.h>

// Don't use exponents or any fancy math libraries to keep size down.

#define NeoPixelPIN 0
#define ForcePIN 1
int HugLevel = 0;
int timeSinceLastChange;
int DecaySeconds;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, NeoPixelPIN, NEO_GRB + NEO_KHZ800);


void setup() {
  strip.begin();
  strip.setBrightness(40);
  strip.show();
  
  initDecay();
}

void loop() {
  timeSinceLastChange = now();
  
  int forceReading = analogRead(ForcePIN);
  
  if (forceReading > 0) {
    registerNewHug(forceReading);
  } else if (timeSinceLastChange > DecaySeconds){
    penalizeHugMeter();
  }
    
  delay(500);
}


void registerNewHug(int powerOfHug){
  int OldHugLevel = HugLevel;
  int HugLevelsToRise = (int) powerOfHug / (1024/8);
  HugLevel = min(HugLevel+HugLevelsToRise, 16);
  fillHugMeter(25,100,OldHugLevel+1,HugLevel);

  initDecay();
}

void penalizeHugMeter(){
  if (HugLevel > 0){
    drainHugMeter(25,100,HugLevel);
    HugLevel=max(HugLevel-1,0);
    
    slowDecay();
  }
}

void initDecay(){
  DecaySeconds = 1;
  setTime(0);
}

void slowDecay(){
  // Exponentially slowing decay
  DecaySeconds = DecaySeconds * 2;
  setTime(0);
}

// Fill the dots one after the other with a color
void fillHugMeter(uint32_t c, uint8_t wait, int HugLevelOld, int HugLevelNew) {
  for(uint16_t i=HugLevelOld; i < HugLevelNew+1; i++) {

    strip.setPixelColor(i-1, Wheel(i*(255/16)));
    strip.show();
    delay(wait);
  }
}

// Drain the dots one after the other with a color
void drainHugMeter(uint32_t c, uint8_t wait, int HugLevel){
  strip.setPixelColor(HugLevel-1, 0);
  strip.show();
}


//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
