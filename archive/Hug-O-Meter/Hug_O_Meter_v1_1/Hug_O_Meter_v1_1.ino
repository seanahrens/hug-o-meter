// Includes
#include <Adafruit_NeoPixel.h>
#include <Time.h>

// I/O Pins
#define NeoPixelPIN 0
#define ForcePIN 1

Adafruit_NeoPixel LEDRing = Adafruit_NeoPixel(16, NeoPixelPIN, NEO_GRB + NEO_KHZ800);
int hugPower; 
unsigned long timeOfLastHug;
int hugPowerResiliance[16] =  { 
                                18144000, // Month
                                604800, // Week
                                86400, // Day
                                43200, // 12 Hours  
                                
                                21600, // 6 Hours                              
                                10800, // 3 Hours                              
                                3600, // 1 Hour
                                2700, // 45 min  
                                
                                1800, // 30 min
                                900, // 15 min
                                300, // 5 min
                                60, // 1 min

                                30,
                                15,
                                5,
                                2
                              };
                           
int forceReading;
int msUntilPowerUp;

void setup() {
  // Initialize the LEDRing
  LEDRing.begin();
  LEDRing.setBrightness(10);
  
  // Set initial power-on hug level
  initHugPower(16);
}



void loop(){ 
  // Drop Hug Power Over Time
  if (timeSinceLastHug() > hugPowerResiliance[hugPower-1])
    dropHugPower();

  // Add Hug Power When Hugged
  while (beingHugged()){
    addHugPower();
  }
  // Wait Between Loops
  delay(500);
}


void addHugPower(){
  setTimeOfLastHugToNow(); 
  hugPower = min((hugPower+1), 16);
  flashOnLED(hugPower, msUntilPowerUp);
}

void dropHugPower(){
  // Turn off the LED and Flash it Off for an amount of time commiserate with the inverse of the HugPower level
  hugPower = max((hugPower-1), 1);
  flashOffLED(hugPower+1, ((16-hugPower) * 1000));
}

boolean beingHugged(){
  forceReading = analogRead(ForcePIN);
  msUntilPowerUp = (1023 - forceReading) + 200;
  return (forceReading > 200);
}

void setTimeOfLastHugToNow(){
  timeOfLastHug = now();
}

int timeSinceLastHug(){
  return (now() - timeOfLastHug);
}

//int timeToHugPowerUp(){
//  return max((1023-forceReading),100) * 300;
//  // Fastest hugPowerUp = 100*3 = 300 per level
//  // Slowest hugPowerUp = 823*3 = ~2400 per level
//}

void flashOnLED(int LEDNum, int time){
  int numFlashes = 3;
  for(int i = 0; i <= numFlashes; i++){
    turnOnLED(LEDNum);
    delay((float)msUntilPowerUp * (2.0/(3.0*(float)numFlashes)) );
 
    turnOffLED(LEDNum);
    delay((float)msUntilPowerUp * (1.1/(3.0*(float)numFlashes)) );
  }
  turnOnLED(LEDNum);
}

void flashOffLED(int LEDNum, int time){
  while (time > 0){
    // Allow Recovery Through a Hug!
    if (beingHugged())
      break;

    turnOffLED(LEDNum);
    delay(200);
    turnOnLED(LEDNum);
    delay(100);
    // subtract time taken so far
    time-=300;
  }
  turnOffLED(LEDNum);
}

void turnOnLED(int LEDNum){
  LEDRing.setPixelColor(LEDNum-1, colorOfLED(LEDNum*(255/16)));
  LEDRing.show();
}

void turnOffLED(int LEDNum){
  LEDRing.setPixelColor(LEDNum-1, 0);
  LEDRing.show();
}

void initHugPower(int power){
  setTimeOfLastHugToNow();
  hugPower = power;
  for (int i = 0; i < hugPower+1; i++)
    turnOnLED(i);
}

//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
uint32_t colorOfLED(byte WheelPos) {
  if(WheelPos < 85) {
   return LEDRing.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return LEDRing.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return LEDRing.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

