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
#define NeoCenterLED 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(17, NeoPixelPIN, NEO_GRB + NEO_KHZ800);
int neoCenterColor = 1;
int neoRingColor = 1;
int colorSettingsCount = 7;
String colorSettings[7] = {"red","orange","yellow","green","blue","violet","white"};

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
int hugPower = 8; //starting level
int hugPowerMax = 17; //0-15 (16 options) one of these is the center pixel
int totalHugs = 0;
unsigned long timeOfLastHug;
int p;
int hugPowerResilianceUnitary[18] =  { 3600,3600,  3600,3600,1800,1800,  600,600,300,300,  60,60,60,30,  16, 8, 4, 2 };
//int hugPowerResiliance[18] =  { 140,130,  112,104,96,88,  80,72,64,56,  48,40,32,24,  16, 8, 4, 2 };
int hugPowerResiliance[18];            
                           

// GLOBAL MODE-SELECTION VARIABLES
void breakOnTap(void (*mode_func)(boolean init));
boolean onNow = false;
int modeNumber = 0;
unsigned long timeToggled;
boolean muted = true;

// THE MODES AVAILABLE
int numModes = 5;
void (*modes[])(boolean) = {
    hugPowerMode,  
    setColorMode,
    vizMode,
    setBrightnessMode,
    volumeMode
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


  // SET HUG TIME NOW
  setTimeOfLastHugToNow();
  for (int i = 17; i >= 0; i--){
    if (i == 17)
      hugPowerResiliance[i] = hugPowerResilianceUnitary[i]; 
    else 
      hugPowerResiliance[i] = hugPowerResiliance[i+1] + hugPowerResilianceUnitary[i]; 
  }
  
  // BEEP TO LET KNOW ITS ON
  playToneHappy(); playToneHappy();
}




void loop(){  
 void (*mode_func)(boolean) = modes[modeNumber];
 neoClear();
 mode_func(true);
 delay(300);
 
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
 modeNumber = (modeNumber + 1) % numModes;
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















//void neoBreathe(){
//  //Written by: Jason Yandell
//  
//  int TOTAL_LEDS = 60;
//  float MaximumBrightness = 50;
//  float SpeedFactor = 0.008; // I don't actually know what would look good
//  float StepDelay = 5; // ms for a step delay on the lights
//  
//  // Make the lights breathe
//  for (int i = 0; i < 65535; i++) {
//    // Intensity will go from 10 - MaximumBrightness in a "breathing" manner
//    float intensity = MaximumBrightness /2.0 * (1.0 + sin(SpeedFactor * i));
//    strip.setBrightness(intensity);
//    // Now set every LED to that color
//    for (int ledNumber=0; ledNumber<TOTAL_LEDS; ledNumber++) {
//      strip.setPixelColor(ledNumber, 0, 0, 255);
//    }
//    
//    strip.show();
//    //Wait a bit before continuing to breathe
//    delay(StepDelay);
//  }
//}








//COLOR & LED HELPERS
void neoClear(){
  neoSetAll("black");
}

void neoSetAll(String color_name){  
  neoSetRange(color_name,0,strip.numPixels());
}

void neoSetCenter(String color_name){
  strip.setPixelColor(neoAllOffset(NeoCenterLED), color(color_name));
  strip.show();
}

void neoFlashAll(String color_name, int periodOn, int periodOff){  
  neoFlashRange(color_name, 0, strip.numPixels(), periodOn, periodOff);
}

void neoFlashCenter(String color_name){
//  strip.setPixelColor(neoAllOffset(NeoCenterLED), color(color_name));
//  strip.show();
}


//Range of Pixels
void neoSetRange(String color_name, int start_point, int end_point){


  if (color_name == "rainbow"){
    for (int i=start_point; i != end_point; i = (i + 1) % strip.numPixels()) {
      setPixelToRainbow(i);
    }
  } else {
    int n = end_point - start_point;
    uint32_t c = color(color_name);
    for (int i=start_point; i < start_point + n; i++) {
      setPixelColor(i % strip.numPixels(), c);
    }
  }
  
  strip.show();
}

void neoFlashRange(String color_name, int start_point, int end_point, int periodOn, int periodOff){  
  int toggleAction = toggleTimerAction(periodOn, periodOff);
  
  if (toggleAction == 1){
    onNow = true;
    neoSetRange(color_name, start_point, end_point);  
  } else if (toggleAction == 2) {
    onNow = false;
    neoSetRange("black", start_point, end_point);
  }
}


void setPixelColor(int i, uint32_t c){
  strip.setPixelColor(neoRingOffset(i), c); 
}

void setPixelToRainbow(int i){
  setPixelColor(i, rainbowColor(i)); 
}







// Offset Origin
int neoRingOffset(int o){
  if (o == NeoCenterLED)
    o+=1;
  return neoAllOffset(o);
}

// Offset Origin
int neoAllOffset(int o){
  int origin = strip.numPixels() - NeoPixelOffset;
  return abs((origin + o) % strip.numPixels());
}














// COLOR
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
  else if (name == "brown")
    strip.Color(139,69,19); 
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




  
 







// BUTTON FUNCTIONS

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
  return (timeSinceLastHug() > hugPowerResiliance[hugPower]);
}

void dropHugPower(){
  hugPower = max((hugPower-1), 0);
  //playToneSad();
  Serial.println("Dropped Hug Power");
}


// TIMER!

// 0 = do nothing. 1 = switch on. 2 = switch off. //TODO handle periodOff
int toggleTimerAction(int periodOn, int periodOff){
  unsigned long timeSinceToggled = millis() - timeToggled;
  
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
  playToneHappy(); playToneHappy();
  neoSetRange("rainbow",0,hugPower);
    
  // Measure Hug
  int numHugReadings = 0;
  int intHugStrength;
  float hugStrengthAvg = 0;
  int addedHugPower;
  int top = hugPower + 1;
  int elapsed_ms = 0;
  
  while (beingHugged()) {

    // Read Force
    numHugReadings++;
    intHugStrength = hugStrength();
    hugStrengthAvg = ((hugStrengthAvg * (numHugReadings-1)) + intHugStrength) / numHugReadings;     
    addedHugPower = sqrt((hugStrengthAvg * numHugReadings) / 2);
    top = min((hugPower + addedHugPower),hugPowerMax);

    if ((numHugReadings % 5) == 0)
      playToneHappy();
 
    //Animate Hug Measurement
    neoFlashRange("rainbow", hugPower, top, 200, 200);
    delay(200);
    elapsed_ms += 200;
    
  }
  
  // Show off the added hug for a bit longer so the hugger can see
  for(int i=0;i<2000;i+=300){
    neoFlashRange("rainbow", hugPower, top, 200, 200);
    delay(200);
  }

  // Log the Recored Hug
  hugPower = min((hugPower + addedHugPower),hugPowerMax);
  totalHugs++;
  setTimeOfLastHugToNow();    
  playToneHappy();  
  playToneHappy(); 
}






//MODES!
int numHugSubModes = 2;
int HugSubMode;

void hugPowerMode(boolean init) {  
  if (init){
    HugSubMode = 0;
    neoSetCenter("red");
  }
  
  if (upButtonPressed()){
    HugSubMode = (numHugSubModes + HugSubMode + 1) % numHugSubModes;
    neoClear();
    delay(300);
  }
  if (downButtonPressed()){
    HugSubMode = (numHugSubModes + HugSubMode - 1) % numHugSubModes;
    neoClear();
    delay(300);
  }
  

  
  if (HugSubMode==0){
    neoSetRange("rainbow",0,hugPower);  
  } else if (HugSubMode==1){
    showTotalHugs();
  }
  
  if (timeToLoseHugPower()){
    dropHugPower();
    neoClear();  
  }
}








void showTotalHugs() {
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

//  Serial.println("asdsad");
//  Serial.println(l);
//  Serial.println(x);
//  Serial.println(v);
//  Serial.println(i);

  //Show Roman Numerals Using Colors (from Largest to Smallest)
  int p = 0;
  neoSetRange("green",p,l);
  neoSetRange("yellow",p+=l,x);
  neoSetRange("orange",p+=x,v);
  neoSetRange("red",p+=v,i);
}



// BRIGHTNESS!!!

int brightnessLevels[] = {1, 10, 25, 50, 100, 255};
int brightnessLevel = 2;

void setBrightnessMode(boolean init){
  if (upButtonPressed()){
    brightnessLevel = min((brightnessLevel + 1), 4); // fix with sizeof
    setBrightness();
    delay(500);
  }    
  if (downButtonPressed()){
    brightnessLevel = max((brightnessLevel - 1), 0); // fix with sizeof
    setBrightness();
    delay(500);
  }  

  neoSetCenter("white");
  neoSetRange("white", 1,7);
  neoSetRange("white", 8,15);
}

void setBrightness(){
  strip.setBrightness(brightnessLevels[brightnessLevel]);
}

void setColorMode(boolean init){
  if (upButtonPressed()){
    neoRingColor = (neoRingColor + 1) % colorSettingsCount;
    delay(500);
  }
  if (downButtonPressed()){
    neoCenterColor = (colorSettingsCount + neoCenterColor - 1) % colorSettingsCount;
    delay(500);
  }
  
  neoSetAll(colorSettings[neoRingColor]);
  neoSetCenter(colorSettings[neoCenterColor]);
  strip.show();
}




int vizSubMode;
int numSubModes = 7;
int start_point;

void vizMode(boolean init){
  if (init){
    vizSubMode = 0;
    start_point = 0;
  }
  
  if (upButtonPressed()){
    vizSubMode = (numSubModes + vizSubMode + 1) % numSubModes;
    delay(500);
  }
  if (downButtonPressed()){
    vizSubMode =  (numSubModes + vizSubMode - 1) % numSubModes;
    delay(500);
  }


  if (vizSubMode != 6)
    neoSetCenter("black");
    
  if (vizSubMode==0){
    int toggleAction = toggleTimerAction(300, 300);
    if (toggleAction){
      int end_point = (start_point+9) % strip.numPixels();
      start_point = (start_point+1) % strip.numPixels();      
      neoClear();
      neoSetRange("rainbow",start_point, end_point);
    }
  } else if (vizSubMode==1)
    neoSetAll("rainbow");
  else if (vizSubMode==2)
    neoFlashAll("rainbow",200,200);
  else if (vizSubMode==3)
    neoFlashAll("red", 200,200);    
  else if (vizSubMode==4)
    neoFlashAll("white", 200,200);
  else if (vizSubMode==5)
    neoFlashAll("white", 5,150);
  else if (vizSubMode==6){
    //burning man symbol
    neoClear();
    neoSetRange("brown",1,2);
    neoSetRange("brown",5,6);
    neoSetRange("brown",10,11);
    neoSetRange("brown",14,15);
    neoSetCenter("brown");
  }
}


//void rainbowAnimatedMode(boolean init) {
//  if (init){
//    p=1;
//  }
//  int toggleAction = toggleTimerAction(40, 40);
//  if (toggleAction){
//    neoClear();
//    neoSetRange("rainbow",p,9);
//    p = p++ % strip.numPixels();
//  }
//}



    //jumping burning man symbol
//    neoClear();
//    neoSetRange("brown",1,2);
//    neoSetRange("brown",5,6);
//    neoSetRange("brown",10,11);
//    neoSetRange("brown",14,15);
//    neoSetCenter("brown");
//    
//    delay(800);
//
//    neoClear();     
//    neoSetRange("brown",0,1);
//    neoSetRange("brown",6,7);
//    neoSetRange("brown",9,10);
//    neoSetRange("brown",15,16);
//    neoSetCenter("brown");
    
//    delay(800);





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
  
  if (muted){
    // Ears
    neoSetRange("white",1,3);
    neoSetRange("white",11,13);

    // Looks like a strike through the ears    
    neoSetCenter("red");
    neoSetRange("red",3,4);   
    neoSetRange("red",12,13);     
  }
  else {
    // Ears
    neoSetCenter("black");
    neoSetRange("white",1,4);
    neoSetRange("white",10,13);
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

