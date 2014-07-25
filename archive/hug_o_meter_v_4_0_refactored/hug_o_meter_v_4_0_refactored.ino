// Includes
#include <Adafruit_NeoPixel.h>
#include <Time.h>

#define NeoPixelOffset 6

// I/O Pins
#define NeoPixelPIN 9
#define ButtonPIN 2
#define BuzzerPIN 3

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

//HugPower Memory / Related Vars
int hugPower; 
int hugPowerMax = 12; //8;
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
//  10000, // Month
//  5000, // Week
//  1000, // Day
//  360, // 12 Hours  
//  
//  180, // 6 Hours                              
//  150, // 3 Hours                              
//  120, // 1 Hour
//  90, // 45 min  
  
  64000, // 15 min
  32000, // 5 min
  16000, // 1 min
  8000, // 10 sec
  
  4000, // 15 min
  2000, // 5 min
  1000, // 1 min
  500, // 10 sec
  
  256, // 15 min
  128, // 5 min
  64, // 1 min
  32, // 10 sec
  
  16, // 15 min
  8, // 5 min
  4, // 1 min
  2 // 10 sec
 
};
                           
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, NeoPixelPIN, NEO_GRB + NEO_KHZ800);
int touchType = 0;

void breakOnTap(void (*mode_func)(boolean init));

boolean onNow = false;
unsigned long timeToggled;
int submode = 0;
int p;

void (*modes[])(boolean) = {
    hugPowerMode, 
    setBrightnessMode, 
    setColorMode,
    rainbowMode,
    flashMode,
    sleepMode,
};





void setup() {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
  
  // Initialize the strip
  strip.begin();
  setBrightnessCustom();

  // Set initial power-on hug level
  initHugPower(8);
  
  // Sing
  playToneHappy();
  playToneHappy();
  Serial.println("welcome");

}

int modeNumber = 0;

void loop(){
 void (*mode_func)(boolean) = modes[modeNumber];
 clearPixels();
 mode_func(true);
 delay(100);
 
 while (true){
   // Run the Mode
   mode_func(false);
   
   readTouchType();
   if (doubleClicked())
     break;
   if (held()){
     measureHug();
     modeNumber = 0;
     return;
   }

 }
 playToneHappy();
 modeNumber = (modeNumber + 1) % 6;
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

int brightnessLevels[] = {10, 30, 50, 100};
int brightnessLevel = 0;

void iterateBrightness(){
  brightnessLevel = (brightnessLevel + 1) % 4; // fix with sizeof
  setBrightnessCustom();
  
  while (touched()){
    // do nothing, hold here until released
  }
}

void setBrightnessCustom(){
  strip.setBrightness(brightnessLevels[brightnessLevel]);
  strip.show();
}

void setBrightnessFull(){ // not really full
  strip.setBrightness(brightnessLevels[2]); //fix with sizeof
  strip.show();
}













//COLOR & LED HELPERS

// CLEAR
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

//Range of Pixels
void setPixelRangeToColor(int start_point, int n, uint32_t c){
  for (int i=start_point; i < start_point + n; i++) {
    setPixelColor(i % strip.numPixels(), c);
  }
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

//uint32_t healthColor(int i) {
//  if(i < 4) {
//   return strip.Color(255,0,0);
//  } else if(i < 7) {
//   return strip.Color(255,165,0);
//  } else {
//   return strip.Color(0,255,0);
//  }
//}


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
  return (digitalRead(ButtonPIN) == HIGH);
}

void readTouchType(){ 
  touchType = captureTouch();
}

boolean tapped(){
  return singleClicked();
}

boolean singleClicked(){
  return (touchType == 1);
}

boolean doubleClicked(){
  return (touchType == 2);
}

boolean held(){
  return (touchType == 3);
}

const int captureWindow = 200;
const int captureReadings = captureWindow*10;
int touchReading[captureReadings];

// 0 = no touch, 1 = single-click, 2 = double-click, 3 = hold
int captureTouch(){
  if (touched()){
     // Acknowledge Touch
     
     // Determine Type of Touch
     // Capture next 300ms of button input
     // read for 300 milliseconds; record in 10 ms increments
     for (int ms = captureWindow, i = 0; ms >= 0; ms-=10, i++){
       touchReading[i] = digitalRead(ButtonPIN);
       Serial.print(touchReading[i]);
       delay(10);
     }
     Serial.println(" ");
  
    // Read the 300ms of button input
    for (int i = 0; i <= captureReadings; i++){ 
      if (touchReading[i]==0){ // It's a Click (because it was let up)
        // Was it pressed back down?
        for (; i <= captureReadings; i++){
          if (touchReading[i]==1){
            return 2; // It's a Double Click
          }
        }
        return 1; // It's a Single Click
      }
    }
    return 3; // Hold
  } else {
    return 0; // No Touch
  }
}












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
}


// TIMER!

// 0 = do nothing. 1 = switch on. 2 = switch off. //TODO handle periodOff
int toggleTimerAction(int periodOn, int periodOff){
  unsigned long timeSinceToggled = millis() - timeToggled;
  Serial.print("timeSinceToggled: ");
  Serial.println(timeSinceToggled);
  if (!onNow && (timeSinceToggled > periodOff)){
    timeToggled = millis();
    return 1;   
  } else if (onNow && (timeSinceToggled > periodOn)){
    timeToggled = millis();
    return 2;   
  } else
    return 0;
}
 


void measureHug(){
  clearPixels();
  playToneHappy();
  playToneHappy();
  
  // Fill Hug Power
  int powerOfHug=0;
  while (touched()) {
    powerOfHug++;
    delay(400);
  }
  
  //Demonstrate Result
  clearPixels();
  delay(500);
  playToneHappy();
  playToneHappy();
  
  hugPower = min(powerOfHug,hugPowerMax);
 
  for (int i=1; i < hugPower + 1; i++){
    setPixelToRainbow(p+i-1);
    strip.show();
    playToneHappy();
    delay(400);
  }
  
  totalHugs++;
  setTimeOfLastHugToNow();    
  playToneHappy();  
  playToneHappy(); 
  delay(1000);
}














//MODES!

void hugPowerMode(boolean init) {
  Serial.println("hug mode");
//  if (singleClicked()){
//    int wait;
//    if (p == 0){
//      setAllPixelsToColor(color("green"));
//      wait = 2000;
//    } else {
//      setAllPixelsToColor(color("red"));
//      wait = 500;
//    }
//    strip.show();
//    delay(wait);
//  }

  
    
  if (timeToLoseHugPower())
    dropHugPower();    
    
      
  if (toggleTimerAction(40, 40)){
    clearPixels();
    setPixelColor(0,color("white"));
    setPixelRangeToRainbow(p,hugPower);
    strip.show();
    p = p++ % strip.numPixels();
  }


}



//void totalHugsMode(boolean init) {
//  Serial.print("Total Hugs Mode: ");
//  Serial.println(totalHugs);
////  if (held())
////    measureHug();
//  
//  int t = totalHugs;
//  
//  //Generate Roman Numerals
//  // L = 50s
//  int l = t / 50; 
//  t = t - l*50;
//  // X = 10s
//  int x = t / 10;
//  t = t - x*10;
//  // V = 5s
//  int v = t / 5;
//  t = t - v*5;
//  // I = 1s
//  int i = t;
//
//  Serial.println("asdsad");
//  Serial.println(l);
//  Serial.println(x);
//  Serial.println(v);
//  Serial.println(i);
//
//  
//  //Show Roman Numerals Using Colors (from Largest to Smallest)
//  int p = 1;
//  setPixelRangeToColor(p,l,color("green"));
//  setPixelRangeToColor(p+=l,x,color("yellow"));
//  setPixelRangeToColor(p+=x,v,color("orange")); // 1,1
//  setPixelRangeToColor(p+=v,i, color("red")); // 2,3
//  strip.show();
//
//  delay(200); 
//}






// Slightly different, this makes the rainbow equally distributed throughout

void rainbowMode(boolean init) {
  setAllPixelsToRainbow();
  strip.show();
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
  
  if (singleClicked())
    iterateBrightness();
   
  setAllPixelsToColor(color("white"));
  strip.show();

}

void setColorMode(boolean init){
  if (singleClicked())
    colorSetting = (colorSetting + 1) % colorSettingsCount;

  setAllPixelsToColor(color(colorSettings[colorSetting]));
  strip.show();
}


int flashSubMode = 0;
void flashMode(boolean init){
  if (singleClicked())
    flashSubMode = (flashSubMode + 1) % 3;

  if (flashSubMode==0)
    flashColor(color("white"), 200,200);
  if (flashSubMode==1)
    flashColor(color("red"), 200,200);    
  else if (flashSubMode==2)
    flashColor(color("white"), 5,150);  
}


void sleepMode(boolean init){
 if (init){
   setAllPixelsToColor(strip.Color(0,0,0));
   strip.show();
 }
}

void setTestMode(boolean init){
 setPixelColor(-3,color("white")); 
 setPixelColor(-2,color("red")); 
 setPixelColor(-1,color("orange")); 
 setPixelColor(0,color("yellow")); 
 setPixelColor(1,color("green")); 
 setPixelColor(2,color("blue")); 
 setPixelColor(3,color("violet")); 
 setPixelColor(4,color("white")); 
 strip.show();
}


void flashColor(uint32_t c, int periodOn, int periodOff){   
   int toggleAction = toggleTimerAction(periodOn, periodOff);
   Serial.print("toggle action: ");
   Serial.println(toggleAction);
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

