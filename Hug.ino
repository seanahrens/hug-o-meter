// Includes
#include <Adafruit_NeoPixel.h>
#include <Time.h>

//Input/Output

// NEOPIXEL RING
#define NeoPixelPIN 9
// SPEAKER
#define BuzzerPIN A3
// FORCE SENSOR
#define FSRPIN A5
// 3-WAY JOG DIAL
#define ButtonDownPIN 11
#define ButtonInPIN 10 
#define ButtonUpPIN 3 

// Set up
// NEOPIXEL SETUP
#define NeoPixelOffset 7
Adafruit_NeoPixel strip = Adafruit_NeoPixel(17, NeoPixelPIN, NEO_GRB + NEO_KHZ800);

// SPEAKER SETUP
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



// GLOBAL VARIABLES

// GLOBAL HUG VARIABLES
int hugPower; 
int hugPowerMax = 12; //8;
int totalHugs = 0;
unsigned long timeOfLastHug;
int p;
int hugPowerResiliance[16] =  { 112,104,96,88,  80,72,64,56,  48,40,32,24,  16, 8, 4, 2 };
            
                           

// GLOBAL MODE-SELECTION VARIABLES
void breakOnTap(void (*mode_func)(boolean init));
boolean onNow = false;
int modeNumber = 0;
int submode = 0;
int flashSubMode = 2;
unsigned long timeToggled;
boolean muted = false;

// THE MODES AVAILABLE
void (*modes[])(boolean) = {
    hugPowerMode, 
    setBrightnessMode,
    volumeMode, 
    setColorMode,
    rainbowMode,
    flashMode
};














// SETUP All THE THINGS, ONCE.
void setup() {
  
  // SERIAL LOGGING SETUP
  Serial.begin(9600);   
  
  // NEOPIXEL SETUP
  strip.begin();
  setBrightness();
  
  // JOG DIAL SETUP
  pinMode(ButtonDownPIN,INPUT);
  pinMode(ButtonInPIN,INPUT);
  pinMode(ButtonUpPIN,INPUT);





  // GRANT INITIAL HUG POWER
  initHugPower(16);
  
  // BEEP TO LET KNOW ITS ON
  playToneHappy(); playToneHappy();
}




void loop(){  
 void (*mode_func)(boolean) = modes[modeNumber];
 clearPixels();
 mode_func(true);
 delay(100);
 
 while (true){
   // Run the Mode
   mode_func(false);

   if (modeButtonPressed())
     break;
   if (hugInitiated()){
     modeNumber = 0;
     measureHug();
     return;
   }

 }
 playToneHappy();
 modeNumber = (modeNumber + 1) % 6;
}















// SOUNDS!!

void playToneHappy(){
  int delaySec = 25;
  
  if (muted){
    delay(delaySec * 4);
    return;
  }
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
  int delaySec = 50;
  
  if (muted){
    delay(delaySec * 4);
    return;
  }

  tone(BuzzerPIN, toneA);
  delay(delaySec);
  tone(BuzzerPIN, toneE);
  delay(delaySec);
  tone(BuzzerPIN, toneG);
  delay(delaySec);
  tone(BuzzerPIN, tonec);
  delay(delaySec);
  noTone(BuzzerPIN);
}












// BRIGHTNESS!!!

int brightnessLevels[] = {1, 10, 25, 50, 100, 255};
int brightnessLevel = 1;

void turnBrightnessUp(){
  brightnessLevel = min((brightnessLevel + 1), 4); // fix with sizeof
  setBrightness();
}

void turnBrightnessDown(){
  brightnessLevel = max((brightnessLevel - 1), 0); // fix with sizeof
  setBrightness();
}

void setBrightness(){
  strip.setBrightness(brightnessLevels[brightnessLevel]);
  strip.show();
}


void neoBreathe(){
  //Written by: Jason Yandell
  
  int TOTAL_LEDS = 60;
  float MaximumBrightness = 50;
  float SpeedFactor = 0.008; // I don't actually know what would look good
  float StepDelay = 5; // ms for a step delay on the lights
  
  // Make the lights breathe
  for (int i = 0; i < 65535; i++) {
    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
    float intensity = MaximumBrightness /2.0 * (1.0 + sin(SpeedFactor * i));
    strip.setBrightness(intensity);
    // Now set every LED to that color
    for (int ledNumber=0; ledNumber<TOTAL_LEDS; ledNumber++) {
      strip.setPixelColor(ledNumber, 0, 0, 255);
    }
    
    strip.show();
    //Wait a bit before continuing to breathe
    delay(StepDelay);
  }
}








//COLOR & LED HELPERS

// CLEAR
void neoClear(){
  clearPixels();
}
void clearPixels(){
  for (int i = 0; i < strip.numPixels() + 1; i++) {
    setPixelColor(i, color("black"));
  }
  strip.show();
}



// 1 Pixel
void setPixelColor(int i, uint32_t c){
  strip.setPixelColor(offsetOrigin(i), c); 
}

void setPixelToRainbow(int i){
  setPixelColor(i, rainbowColor(i)); 
}

//N pixels
void setNPixelsToColor(int n, uint32_t c){
  setPixelRangeToColor(1,n,c);
}

void setNPixelsToRainbow(int n){
  setPixelRangeToRainbow(1,n);
}


//void setBrightness(){
//  strip.setBrightness(brightnessLevels[brightnessLevel]);
//  strip.show();
//}


//Range of Pixels
// THIS FUNCTION IS THE SAME AS BELOW Ecept its an actual range, and does strip.show
void neoRange(String name, int start_point, int n){
  uint32_t c = color(name);
  n = n - start_point;
  if (n > 0){
    for (int i=start_point; i < start_point + n; i++) {
      setPixelColor(i % strip.numPixels(), c);
    }
  } else {
    for (int i=start_point; i > start_point + n; i--) {
      setPixelColor(i % strip.numPixels(), c);
    }    
  }
  strip.show();
}

//Range of Pixels
void setPixelRangeToColor(int start_point, int n, uint32_t c){
  if (n > 0){
    for (int i=start_point; i < start_point + n; i++) {
      setPixelColor(i % strip.numPixels(), c);
    }
  } else {
    for (int i=start_point; i > start_point + n; i--) {
      setPixelColor(i % strip.numPixels(), c);
    }    
  }
  strip.show();
}

void setPixelRangeToRainbow(int start_point, int n){
  for (int i=start_point; i < (start_point + n); i++) {
    setPixelToRainbow(i % strip.numPixels());
  }
}

// All Pixels
void setAllPixelsToColor(uint32_t c){
  setNPixelsToColor(strip.numPixels(), c);
}

void setAllPixelsToRainbow(){
  setNPixelsToRainbow(strip.numPixels());
}

// Offset Origin
int offsetOrigin(int o){
  int origin = strip.numPixels() - NeoPixelOffset;
  return abs((origin + o - 1) % strip.numPixels()); // -1 to correct for CS array
}

void flashNearDeath(int i){
  setPixelColor(i, color("black")); 
  strip.show();
  delay(300);
  setPixelColor(i, rainbowColor(i)); 
  strip.show();
  delay(400);
} 












// COLOR

int colorSetting = 1;
int colorSettingsCount = 7;
String colorSettings[7] = {"red","orange","yellow","green","blue","violet","white"};

uint32_t color(String name){
  if (name == "red")
    strip.Color(255,0,0);
  else if (name == "orange")
    strip.Color(255,165,0);
  else if (name == "yellow")
    strip.Color(255,255,0);
  else if (name == "green")
    strip.Color(0,255,0);
  else if (name == "blue")
    strip.Color(0,0,255);
  else if (name == "violet")
    strip.Color(128,0,128);
  else if (name == "white")
    strip.Color(255,255,255);
  else if (name == "black")
    strip.Color(0,0,0);
  else
    strip.Color(255,255,255);
}

uint32_t rainbowColor(int LEDnum){
  return Wheel(((LEDnum * 256 / strip.numPixels()) & 255));
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




  
 







// TOUCH/BUTTON FUNCTIONS

boolean touched(){
  //return (digitalRead(ButtonPIN) == HIGH);
  return (digitalRead(ButtonInPIN) == LOW);
}

//void readTouchType(){ 
//  touchType = captureTouch();
//}

//boolean tapped(){
//  return singleClicked();
//}

//boolean singleClicked(){
//  return (touchType == 1);
//}

//boolean doubleClicked(){
//  return (touchType == 2);
//}

//boolean held(){
//  return (touchType == 3);
//}




//new hardware
boolean downButtonPressed(){
  return (digitalRead(ButtonDownPIN) == LOW);
}

boolean modeButtonPressed(){
  return (digitalRead(ButtonInPIN) == LOW);
}

boolean upButtonPressed(){
  return (digitalRead(ButtonUpPIN) == LOW);
}


//// 0 = no touch, 1 = single-click, 2 = double-click, 3 = hold
//int captureTouch(){
//
//  if (upButtonPressed() || downButtonPressed())
//    return 1;
//  if (modeButtonPressed()){
//    // if the user holds this button for 1 second it should turn off the displays
//    //    for (int elapsed_ms = 0; modeButtonPressed(); elapsed_ms += 10){
//    //      if (elapsed_ms > 1000)
//    //        playToneHappy();
//    //      delay(10);
//    //    }
//    return 2;
//  } 
//  if (beingHugged()){
//    delay(300);
//    // filter out nudges
//    if (beingHugged())
//      return 3;  
//  }
//    
//    

  
  
//  if (touched()){
//     // Acknowledge Touch
//     // Determine Type of Touch
//     // Capture next 300ms of button input
//     // read for 300 milliseconds; record in 10 ms increments
//     for (int ms = captureWindow, i = 0; ms >= 0; ms-=10, i++){
//       touchReading[i] = digitalRead(ButtonInPIN);
//       Serial.print(touchReading[i]);
//       delay(10);
//     }
//     Serial.println(" ");
//  
//    // Read the 300ms of button input
//    for (int i = 0; i <= captureReadings; i++){ 
//      if (touchReading[i]==1){ // It's a Click (because it was let up)
//        // Was it pressed back down?
//        for (; i <= captureReadings; i++){
//          if (touchReading[i]==0){
//            return 2; // It's a Double Click
//          }
//        }
//        return 1; // It's a Single Click
//      }
//    }
//    return 3; // Hold
//  } else {
//    return 0; // No Touch
//  }
  
  
  
//}












// HUG MEASURMENT!!!!

void initHugPower(int power){
  setTimeOfLastHugToNow();
  hugPower = power;
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
  //playToneSad();
  Serial.println("Dropped Hug Power");
}


// TIMER!

// 0 = do nothing. 1 = switch on. 2 = switch off. //TODO handle periodOff
int toggleTimerAction(int periodOn, int periodOff){
  unsigned long timeSinceToggled = millis() - timeToggled;
//  Serial.print("timeSinceToggled: ");
//  Serial.println(timeSinceToggled);
  if (!onNow && (timeSinceToggled > periodOff)){
    timeToggled = millis();
    return 1;   
  } else if (onNow && (timeSinceToggled > periodOn)){
    timeToggled = millis();
    return 2;   
  } else
    return 0;
}
 
 
int fsrReading(){
  return analogRead(FSRPIN);
}

// 1-8 in hug strength
int hugStrength(){
  return ((fsrReading() - 400) / 50);
}

boolean beingHugged(){
  return (fsrReading() >= 400);
}

boolean hugInitiated(){  
  if (beingHugged()){
    delay(300);// filter out nudges
    
    if (beingHugged())
      return true; 
  }
  return false;
}

void measureHug(){
  clearPixels();
  playToneHappy();
  playToneHappy();
  
  
  // Measure Hug
  int numHugReadings = 0;
  float hugStrengthAvg = 0;
  
  while (beingHugged()) {
    int intHugStrength = hugStrength();
    Serial.print("Hug Strength: ");
    Serial.println(intHugStrength);
    numHugReadings++;
    hugStrengthAvg = ((hugStrengthAvg * (numHugReadings-1)) + intHugStrength) / numHugReadings;
 
    Serial.print("Hug Avg: ");
    Serial.println(hugStrengthAvg);
    
    if (numHugReadings % 4 == 0)
      playToneHappy();
    delay(250);
    
    clearPixels();
    setPixelRangeToColor(8,max(-(numHugReadings/4),-8),color("yellow"));
    setPixelRangeToRainbow(9,intHugStrength);
    strip.show();
  }
  
//  Serial.println("");
//  Serial.println("RESULTS!");
//
//  Serial.print("Hug Time: ");
//  Serial.println(numHugReadings * 250);  
//
//  Serial.print("Hug Avg: ");
//  Serial.println(hugStrengthAvg);
//  
//  int powerOfHug = max(1,sqrt(hugStrengthAvg * numHugReadings));


  //Demonstrate Result
  clearPixels();
  playToneHappy(); playToneHappy();
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  playToneHappy(); playToneHappy();
  clearPixels();
  
  setPixelRangeToColor(8,max(-(numHugReadings/4),-8),color("yellow"));
  setPixelRangeToColor(9,hugStrengthAvg,color("blue"));
  strip.show();
  delay(2000);
  playToneHappy();
  playToneHappy();
  clearPixels(); 
  
  int addedHugPower = sqrt(sqrt(hugStrengthAvg * numHugReadings));
  
  
  setPixelRangeToRainbow(0,addedHugPower);
  strip.show();
  delay(5000);
  playToneHappy();
  playToneHappy();
  clearPixels();   
  
  hugPower = min(hugPower + addedHugPower,hugPowerMax);
 
//  for (int i=1; i < hugPower + 1; i++){
//    setPixelToRainbow(p+i-1);
//    strip.show();
//    playToneHappy();
//    delay(400);
//  }
  
  totalHugs++;
  setTimeOfLastHugToNow();    
  playToneHappy();  
  playToneHappy(); 
  delay(1000);
}














//MODES!

void hugPowerMode(boolean init) {  
  if (timeToLoseHugPower()){
    dropHugPower();    
    clearPixels();  
  }
  
  setPixelRangeToRainbow(1,hugPower);
  strip.show();
}
//  if (toggleTimerAction(40, 40)){
//    clearPixels();
////    setPixelColor(0,color("white"));
//    setPixelRangeToRainbow(p,hugPower);
//    strip.show();
//    p = (p+1) % strip.numPixels();
//  }






void totalHugsMode(boolean init) {
  Serial.print("Total Hugs Mode: ");
  Serial.println(totalHugs);
//  if (held())
//    measureHug();
  
  int t = totalHugs;
  
  //Generate Roman Numerals
  // L = 50s
  int l = t / 50; 
  t = t - l*50;
  // X = 10s
  int x = t / 10;
  t = t - x*10;
  // V = 5s
  int v = t / 5;
  t = t - v*5;
  // I = 1s
  int i = t;

  Serial.println("asdsad");
  Serial.println(l);
  Serial.println(x);
  Serial.println(v);
  Serial.println(i);

  
  //Show Roman Numerals Using Colors (from Largest to Smallest)
  int p = 1;
  setPixelRangeToColor(p,l,color("green"));
  setPixelRangeToColor(p+=l,x,color("yellow"));
  setPixelRangeToColor(p+=x,v,color("orange")); // 1,1
  setPixelRangeToColor(p+=v,i, color("red")); // 2,3
  strip.show();

  delay(200); 
}







void rainbowMode(boolean init) {
  int numSubModes = 3;
  int SubMode = 0;
  
  if (upButtonPressed()){
    SubMode = min(numSubModes-1,(SubMode + 1));
    delay(200);
  }
  if (downButtonPressed()){
    flashSubMode = max(0,(SubMode - 1));
    delay(200);
  }
  
  if (SubMode==0){
    setAllPixelsToRainbow();
    strip.show();    
  } else if (SubMode==1){
    rainbow();
  } else if (SubMode==2){
    rainbowCycle();
  }
  //FIXME BUG ON THIS ONLY SHOWING 2 modes
}


void rainbowAnimatedMode(boolean init) {
  if (init){
    p=1;
  }
  int toggleAction = toggleTimerAction(40, 40);

  if (toggleAction){
    clearPixels();
    setPixelRangeToRainbow(p,9);
    strip.show();
    p = p++ % strip.numPixels();
  }
}

void setBrightnessMode(boolean init){
  if (upButtonPressed()){
    turnBrightnessUp();
    delay(500);
  }    
  if (downButtonPressed()){
    turnBrightnessDown();
    delay(500);
  }  

  setAllPixelsToColor(color("white"));
  strip.show();
}

void setColorMode(boolean init){
  if (upButtonPressed()){
    colorSetting = (colorSetting + 1) % colorSettingsCount;
    strip.show();
    delay(500);
  }
  if (downButtonPressed()){
    colorSetting = (colorSettingsCount + colorSetting - 1) % colorSettingsCount;
    strip.show();
    delay(500);
  }
  
  setAllPixelsToColor(color(colorSettings[colorSetting]));
  strip.show();
}



void flashMode(boolean init){
  int numSubModes = 3;
  
  if (upButtonPressed()){
    flashSubMode = min(numSubModes-1,(flashSubMode + 1));
    delay(200);
  }
  if (downButtonPressed()){
    flashSubMode = max(0,(flashSubMode - 1));
    delay(200);
  }
  
  if (flashSubMode==0)
    flashColor(color("red"), 200,200);    
  else if (flashSubMode==1)
    flashColor(color("white"), 5,150);
  else if (flashSubMode==2)
    flashColor(color("white"), 200,200);
    
}


void volumeMode(boolean init){  
  if (upButtonPressed()){
    playToneHappy();
    muted = false;
    neoClear();
  }
  if (downButtonPressed()){
    playToneSad();
    muted = true;
    neoClear();
  }
  
  if (muted)
    neoRange("white",10,12);
  else
    neoRange("white",4,16);
 
}



void flashColor(uint32_t c, int periodOn, int periodOff){   
   int toggleAction = toggleTimerAction(periodOn, periodOff);
//   Serial.print("toggle action: ");
//   Serial.println(toggleAction);
   if (toggleAction == 1){
     setAllPixelsToColor(c);
     strip.show();
     onNow = true;
   } else if (toggleAction == 2) {
     setAllPixelsToColor(color("black"));
     strip.show(); 
     onNow = false;
   }
}



















// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow() {
  uint8_t wait = 5;
  
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint16_t wait = 5;
  
  uint16_t i, j,p;
  int hugPower = 10;
  int deadPixels = strip.numPixels() - hugPower;

  for(j=0,p=0; j<256*5; j++,p = p++ % strip.numPixels()) { // 5 cycles of all colors on wheel
    for(i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    for(i=0; i < deadPixels; i++) {
      strip.setPixelColor((p+i) % strip.numPixels(), 0);
    }
    strip.show();
    delay(wait);
  }

}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

