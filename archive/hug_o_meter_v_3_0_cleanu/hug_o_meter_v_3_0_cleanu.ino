// Includes
#include <Adafruit_NeoPixel.h>
#include <Time.h>

// I/O Pins
#define NeoPixelPIN 9 //11
#define ForcePIN 5
#define ButtonPIN 2
#define BuzzerPIN 3 //2 
#define ledPIN 3
#define NeoPixelOffset 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, NeoPixelPIN, NEO_GRB + NEO_KHZ800);

int forceReading;

#define toneC    1911
#define toneC1    1804
#define toneD    1703
#define toneEb    1607
#define toneE    1517
#define toneF    1432
#define toneF1    1352
#define toneG    1276
#define toneAb    1204
#define toneA    1136
#define toneBb    1073
#define toneB    1012
#define tonec       955
#define tonec1      902
#define toned       851
#define toneeb      803
#define tonee       758
#define tonef       716
#define tonef1      676
#define toneg       638
#define toneab      602
#define tonea       568
#define tonebb      536
#define toneb       506
#define tonep       0 


//Modes
int mode = 0;
int modeCount = 9;

//HugPower Memory / Related Vars
int hugPower; 
int hugPowerMax = 16; //8;
int totalHugs = 0;
unsigned long timeOfLastHug;
int hugPowerResiliance[16] =  { 
//                                18144000, // Month
//                                604800, // Week
//                                86400, // Day
//                                43200, // 12 Hours  
//                                
//                                21600, // 6 Hours                              
//                                10800, // 3 Hours                              
//                                3600, // 1 Hour
//                                2700, // 45 min  
                                10000, // Month
                                5000, // Week
                                1000, // Day
                                360, // 12 Hours  
                                
                                180, // 6 Hours                              
                                150, // 3 Hours                              
                                120, // 1 Hour
                                90, // 45 min  
                                
                                60, // 15 min
                                45, // 5 min
                                30, // 1 min
                                20, // 10 sec
                                
                                10, // 15 min
                                5, // 5 min
                                3, // 1 min
                                1 // 10 sec
         
                              };
                           







void setup() {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
  
  // Initialize the strip
  strip.begin();
  setBrightnessCustom();

  // Sing
  playToneHappy();
  playToneHappy();

  // Set initial power-on hug level
  initHugPower(8);
}



void loop(){
  
  // Choose Mode
  if (mode == 0)
    hugRainbowMode(5);
  else if (mode == 1)
    totalHugsMode();
  else if (mode == 3)
    setBrightnessMode();
  else if (mode == 4)
    danceModeRainbow(20, 90000);
  else if (mode == 5)
    rainbowCycleFull(20);
  else if (mode == 6)
    flashLightMode();
  else if (mode == 7)
    strobeLightMode();
  else if (mode == 8)
    flashSafetyMode();
  else if (mode == 9)
    flashSafetyRearMode();

  if (isCompressed())
    processCompression();
  
  // Drop Hug Power Over Time
  if (timeToLoseHugPower())
    dropHugPower();  
  
      
  // Wait Between Loops
  delay(200);
}



























// SOUNDS!!

void playToneHappy(){
  int delaySec = 25;
  tone(BuzzerPIN, tonec);
  delay(delaySec);
  tone(BuzzerPIN, toneG);
  delay(delaySec);
  tone(BuzzerPIN, toneE);
  delay(delaySec);
  tone(BuzzerPIN, toneA);
  delay(delaySec);
  noTone(BuzzerPIN);
}

void playToneSad(){
  tone(BuzzerPIN, toneA);
  delay(50);
  tone(BuzzerPIN, toneE);
  delay(50);
  tone(BuzzerPIN, toneG);
  delay(50);
  tone(BuzzerPIN, tonec);
  delay(50);
  noTone(BuzzerPIN);
}












// BRIGHTNESS!!!

int brightnessLevels[] = {10, 30, 50};
int brightnessLevel = 0;

void setBrightnessMode(){
  fillWholeStrip(strip.Color(255,165,0));
  strip.show();
  
  while (true) {
    if (isCompressed()){
      
      if (tapped()){
        changeMode();
        return;
      } else {
        iterateBrightness();
        while (isCompressed())
          delay(100);
      }
      Serial.print("brightness: ");
      Serial.println(brightnessLevel);
        
    }
  }
}

void iterateBrightness(){
  brightnessLevel = (brightnessLevel + 1) % 3; // fix with sizeof
  setBrightnessCustom();
}

void setBrightnessCustom(){
  strip.setBrightness(brightnessLevels[brightnessLevel]);
  strip.show();
}

void setBrightnessFull(){
  strip.setBrightness(brightnessLevels[2]); //fix with sizeof
  strip.show();
}













//COLOR & LED HELPERS

uint32_t color(String name){
  if (name == "orange")
    strip.Color(127,0,0);
  else
    strip.Color(255,255,255);
}

void setNPixelsToColor(int n, uint32_t c){
  for (int i=0; i < n; i++) {
    setPixelColor(i, c);
  }  
}

void setPixelColor(int i, uint32_t c){
  strip.setPixelColor(offsetOrigin(i), c); 
}

void setPixelToRainbow(int i){
  strip.setPixelColor(offsetOrigin(i), Wheel(((i * 256 / strip.numPixels()) & 255))); 
}

void flashNearDeath(int i){
  while (!isCompressed() & !timeToLoseHugPower()){
    strip.setPixelColor(offsetOrigin(i), strip.Color(0,0,0)); 
    strip.show();
    delay(300);
    strip.setPixelColor(offsetOrigin(i), Wheel(((i * 256 / strip.numPixels()) & 255))); 
    strip.show();
    delay(400);
  }
} 

uint32_t healthColor(int i) {
  if(i < 4) {
   return strip.Color(255,0,0);
  } else if(i < 7) {
   return strip.Color(255,165,0);
  } else {
   return strip.Color(0,255,0);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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

int offsetOrigin(int o){
  int origin = strip.numPixels() - NeoPixelOffset;
  return abs((origin + o) % strip.numPixels());
}

void verticalMirrorPixelColor(int i, uint32_t c){
  strip.setPixelColor(offsetOrigin(i), c);
  strip.setPixelColor(offsetOrigin(-i-1), c); 
}


void verticalMirrorPixelRainbow(int i){
  strip.setPixelColor(offsetOrigin(i), Wheel(((offsetOrigin(i) * 256 / strip.numPixels())) & 255));
  strip.setPixelColor(offsetOrigin(-i-1), Wheel(((offsetOrigin(-i-1) * 256 / strip.numPixels())) & 255)); 
}

void fillWholeStrip(uint32_t c){
  setNPixelsToColor(strip.numPixels(), c);
}

boolean processCompression(){    
  if (tapped()){
    changeMode(); 
  } else {
    changeMode(0); // change to hug mode
    measureHug();
  }
}
  
 











// COMPRESSION FUNCTIONS

boolean tapped(){
  if (!isCompressed())
    return false;
    
  // Test if it was tapped by delaying between readings.
  delay(300);
  return !(isCompressed());
}

boolean isCompressed(){
  Serial.print("buttonpin = ");
  Serial.println(digitalRead(ButtonPIN));
  return (digitalRead(ButtonPIN) == HIGH);
}

//int measureForceReading(){
//
//  //return analogRead(ForcePIN);  
//}














// HUG MEASURMENT!!!!

void initHugPower(int power){
  setTimeOfLastHugToNow();
  hugPower = power;
}

void measureHug(){  
  // Count Hug Strength
  while (isCompressed()){
    if (hugPower == hugPowerMax)
      break;
    
    hugPower++;
    totalHugs++; 
    
    setTimeOfLastHugToNow(); 
    playToneHappy();
 
    setPixelToRainbow(hugPower-1);
    strip.show();

    delay(200);
  }   
  
  playToneHappy();  
  playToneHappy(); 
}

int timeSinceLastHug(){
  return (now() - timeOfLastHug);
}

void setTimeOfLastHugToNow(){
  timeOfLastHug = now();
}

boolean timeToLoseHugPower(){
  return (timeSinceLastHug() > hugPowerResiliance[hugPower-1]);
}

void dropHugPower(){
  hugPower = max((hugPower-1), 1);
}































//MODES!
void changeMode(){
  mode = (mode + 1) % modeCount; 
  setBrightnessCustom();
  playToneHappy();
}

void changeMode(int n){
  mode = n;
  setBrightnessCustom();
  playToneHappy();
}


void hugMeterMode(uint8_t wait) {
  uint16_t i;
  
  int deadLevels = hugPowerMax - hugPower;
  Serial.println(hugPower);

  for(i=0; i < hugPowerMax; i++) {
    //verticalMirrorPixelColor(i, Wheel( ((((i+15) % strip.numPixels()) * 256 / strip.numPixels())) & 255));
    verticalMirrorPixelColor(i, healthColor(hugPower));
  }
  for(i=0; i < deadLevels; i++) {
    verticalMirrorPixelColor(i,0);
  }
  strip.show();
  delay(wait); 
}


void hugRainbowMode(uint8_t wait) {
  fillWholeStrip(strip.Color(0,0,0));
  uint16_t i;
  
  int deadLevels = hugPowerMax - hugPower;
  Serial.println(hugPower);

  for(i=0; i < hugPower; i++) {
    setPixelToRainbow(i);
  }

  strip.show();
  flashNearDeath(hugPower-1);

  delay(wait); 
}

void totalHugsMode() {
  fillWholeStrip(strip.Color(0,0,0));
  uint16_t i;
  
  int deadLevels = hugPowerMax - hugPower;
  Serial.println(hugPower);

  for(i=0; i < hugPower; i++) {
    setPixelToRainbow(i);
  }

  strip.show();
  delay(200); 
}


void danceModeRed(uint16_t wait, int loops) {
  uint16_t i, j,p;
  int deadPixels = 10; //strip.numPixels() - hugPower;

  for(j=0,p=0; j<loops; j++,p = p++ % strip.numPixels()) { // 5 cycles of all colors on wheel
    for(i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255,0,0));
    }
    for(i=0; i < deadPixels; i++) {
      strip.setPixelColor((p+i) % strip.numPixels(), 0);
    }
    strip.show();
    delay(wait);
    
    if (isCompressed())
      return;
  }
}


void danceModeRainbow(uint16_t wait, int loops) {
  uint16_t i, j,p;
  int deadPixels = 6; //strip.numPixels() - hugPower;

  for(j=0,p=0; j<loops; j++,p = p++ % strip.numPixels()) { // 5 cycles of all colors on wheel
    for(i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    for(i=0; i < deadPixels; i++) {
      strip.setPixelColor((p+i) % strip.numPixels(), 0);
    }
    strip.show();
    delay(wait);
    
    if (isCompressed())
      return;
  }
}

void flashLightMode(){
 setBrightnessFull();
 for(int i=0; i < strip.numPixels(); i++) {
  strip.setPixelColor(i, strip.Color(255,255,255));
 }
 strip.show();
}


void flashSafetyMode(){
 setBrightnessFull();
 for(int i=0; i < strip.numPixels(); i++) {
  strip.setPixelColor(i, strip.Color(255,255,255));
 }
 strip.show();
 
 delay(200);
 
 for(int i=0; i < strip.numPixels(); i++) {
  strip.setPixelColor(i, 0);
 }
 strip.show(); 
}


void strobeLightMode(){
 setBrightnessFull();
 
 while (!isCompressed()){
   fillWholeStrip(strip.Color(255,255,255));
   strip.show();
   delay(5);
   fillWholeStrip(strip.Color(0,0,0));
   strip.show();
   delay(150);
 }

}


void flashSafetyRearMode(){
 setBrightnessFull();
 for(int i=0; i < strip.numPixels(); i++) {
  strip.setPixelColor(i, strip.Color(255,0,0));
 }
 strip.show();
 
 delay(200);
 
 for(int i=0; i < strip.numPixels(); i++) {
  strip.setPixelColor(i, 0);
 }
 strip.show(); 
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  int deadLevels = hugPowerMax - hugPower;
  Serial.println(hugPower);

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    for(i=0; i < deadLevels; i++) {
      verticalMirrorPixelColor(i,0);
      //strip.setPixelColor(i, );

    }
    if (isCompressed() || timeToLoseHugPower())
      return;
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycleFull(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      if (isCompressed())
        return;
    }
    strip.show();
    delay(wait);
  }
}

