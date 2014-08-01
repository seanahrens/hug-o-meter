// Includes
#include <Adafruit_NeoPixel.h>
#include <Time.h>
#include <EEPROM.h>

//Input/Output

// NEOPIXEL RING
#define NeoPixelPIN 9
// PHOTORESISTOR PIN
#define PhotoPIN A4
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
#define neoRingOrigin 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(17, NeoPixelPIN, NEO_GRB + NEO_KHZ800);

int neoRingSize = 16;
int neoRingLastPixel = neoRingSize - 1;
int neoCenterPixelAddress = neoRingSize;

int neoCenterColor = 1;
int favoriteColor; // 227, 199, 55 all look beautiful

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


// EEPROM (Long Term Memory Storage)
int timeSinceLastHugAddress = 0;
int totalHugsAddress = 1;
int favoriteColorAddress = 2; // need to make this okay
int favoriteVizModeAddress = 3;
boolean mutedAddress = 4;
int secondsUntilStore = 5;


// GLOBAL VARIABLES

// HUG VARIABLES
int hugPowerMax = 15; // //LEDs labelled 0-15 one of these is the center pixel
int hugPower = hugPowerMax; //starting level
unsigned long timeOfLastHug;
int neoPixelResiliance[16] =  { 2,4,8,16, 30,60,60,60, 300,300,600,600, 1800,1800,3600,3600 };
int hugPowerResiliance[16];                      

// MODE-SELECTION VARIABLES
void breakOnTap(void (*mode_func)(boolean init));
boolean onNow = false;
int modeNumber = 0;
unsigned long timeToggled;
int favoriteVizMode;

boolean muted; // if i set this to true, the shit stops working... wtf
boolean sleepDisplay = false;

// THE MODES AVAILABLE
int numModes = 3;
void (*modes[])(boolean) = {
    hugMode, 
    vizMode, 
    setColorMode
};










// SETUP All THE THINGS, ONCE.
void setup() {
  
  // SERIAL LOGGING SETUP
  Serial.begin(9600);   
    
  // INPUT/OUTPUT SETUP
  pinMode(ButtonDownPIN,INPUT);
  pinMode(ButtonInPIN,INPUT);
  pinMode(ButtonUpPIN,INPUT);

  // LOAD DATA FROM EEPROM
  favoriteColor = getFavoriteColorStore();
  favoriteVizMode = getFavoriteVizModeStore();
  muted = getMutedStore();

  // SET HUG TIME NOW
  timeOfLastHug = now();
  setTime(getTimeSinceLastHugStore());

  // GENERATE hugPowerResiliance[] (Total Seconds Until that LED dies) from hugPowerMarginalResiliance[] (Seconds after the previous LED dies, does this LED die)
  hugPowerResiliance[hugPowerMax] = neoPixelResiliance[0]; 
  
  for (int i = 1, h = (hugPowerMax-1);   i <= hugPowerMax;  i++, h--)
    hugPowerResiliance[h] = hugPowerResiliance[h+1] + neoPixelResiliance[i]; 

  // NEOPIXEL SETUP
  autoSetBrightness();  
  strip.begin();
    
  // BEEP TO LET KNOW ITS ON
  playToneHappy(); playToneHappy();
}




void loop(){  
  // Clear Display, and Make Sure it's On
  sleepDisplay = false;
  neoClear();
  
  // Grab the correct mode from the mode array
  void (*mode_func)(boolean) = modes[modeNumber];
  // Run it once to initialize
  mode_func(true);
  
  // Loop the Mode 
  while (true){  
    autoSetBrightness();

    // Detect Button Input
    if (inButtonPressed()){
      if (inButtonLetUpQuickly())
        break; // Switch Modes
      else {
        // Mute or Sleep 
        playMuteNotif();
        
        if (inButtonLetUpQuickly())
          toggleMute();
        else {
          // Sleep
          sleepDisplay = true;
          neoClear();
          while (inButtonPressed()){} //Hold here until they let go
        }
      }
    }
    if (hugInitiated()){
      modeNumber = 0;
      measureHug();
      return;
    }
    if (timeToLoseHugPower()){
      dropHugPower();
    }
    
    // Every 30 min store to EEPROM the Time Since Last Hug
    if (timeSinceLastHug() > (getTimeSinceLastHugStore() + 1800))
      storeTimeSinceLastHug();

    if (sleepDisplay)
      neoClear();
    else
      mode_func(false);
  }
  
  // We just broke into the next mode
  modeNumber = (modeNumber + 1) % numModes;
  
  // Since we switching modes, let's remember the settings of the current mode (these fx only actually do this if changed)
  storeFavoriteVizMode();
  storeFavoriteColor();
  
  neoSetAll("black");
  playToneHappy();
  delay(200);
}






// BRIGHTNESS!!!
int autoBrightnessLevels[] = {10,12,15,17,20,25,27,35,55,100,150,255,255,255,255,255,255,255,255};
float averageAmbientLevel = 0; // this will be the moving average of readings to prevent flicker

void autoSetBrightness(){
   int ambientLevelNow = analogRead(PhotoPIN) / 90;
   // Update Moving Average of 10 Readings:
   averageAmbientLevel = ((9 * averageAmbientLevel) + ambientLevelNow) / 10;
   strip.setBrightness(autoBrightnessLevels[(int) averageAmbientLevel]);
}










// SOUNDS!!

void toggleMute(){
  muted = !muted;
  storeMuted();
}

int happyTones[] = {tonec, toneG, toneE, toneA};
int sadTones[] = {toneA, toneE, toneG, tonec};

int toneSoundToggle[] = {toneG, tonec, toneG, toneG, tonec, toneG};

void playToneHappy(){
  play(happyTones,4,25,false);
}

void playToneSad(){
  play(sadTones,4,50,false);
}

void playMuteNotif(){
  play(toneSoundToggle,6,50,true); 
}

void play(int tones[], int numTones, int tempo, boolean ignoreMuted){
  // sizeofTones http://stackoverflow.com/questions/37538/how-do-i-determine-the-size-of-my-array-in-c
  for (int i = 0; i < numTones; i++){
    if (!muted || ignoreMuted)
      tone(BuzzerPIN, tones[i]);
    delay(tempo);
  }
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
  return Wheel(((LEDnum * 256 / neoRingSize) & 255));
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
boolean downButtonPressed(){
  return (digitalRead(ButtonDownPIN) == LOW);
}

boolean upButtonPressed(){
  return (digitalRead(ButtonUpPIN) == LOW);
}


boolean inButtonPressed(){
  return (digitalRead(ButtonInPIN) == LOW);
}

boolean inButtonLetUpQuickly(){
  int msHeld;
  
  for(msHeld = 0; (inButtonPressed() && (msHeld < 1000)); msHeld += 10)
    delay(10);
  
  if (msHeld < 1000)
    return true;
  else
    return false;
}







// HUG MEASURMENT!!!!

int timeSinceLastHug(){
  return (now() - timeOfLastHug);
}

void setTimeOfLastHugToNow(){
  timeOfLastHug = now();
  storeTimeSinceLastHug();
}

boolean timeToLoseHugPower(){
  return (timeSinceLastHug() > hugPowerResiliance[hugPower]); 
}

void dropHugPower(){
  hugPower = max((hugPower-1), 0);
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

int toggleTimerAction(int periodOn){
  return toggleTimerAction(periodOn, 0);
}
 
 
int fsrReading(){
  return analogRead(FSRPIN);
}

// 1-8 in hug strength
int hugStrength(){
  return max(0,((fsrReading() - 400) / 50)); // don't ever want to give a negative number
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
  // Set up Display for reading
  playToneHappy(); playToneHappy();
  neoClear();
  neoSetRange("rainbow",0,hugPower);
    
  // MEASURE HUG  
  int numHugReadings = 0;
  float hugStrengthAvg = 0;
  int addedHugPower = 0;
  int newHugPower = hugPower+1; //default hug gives +1

  while (beingHugged()) {
 
    // Calculate New Hug Power
    hugStrengthAvg = ((hugStrengthAvg * (numHugReadings-1)) + hugStrength()) / ++numHugReadings; 
    // Tweak this formula to adjust how quickly hugs add hug power
    addedHugPower = max(1,sqrt((hugStrengthAvg * numHugReadings) / 3));
    newHugPower = min((hugPower + addedHugPower),hugPowerMax);

    // Animate and Play Sounds
    neoFlashRange("rainbow", hugPower+1, newHugPower, 200, 200);
    if ((numHugReadings % 5) == 0)
      playToneHappy();
      
    delay(200);
  }
  


  // SHOW OFF NEW HUG LEVEL //TODO: make this non-blocking so if a new hug comes in it can
  // Adjust Light
  for(int i=0;i<10;i++)
    autoSetBrightness();
  // Flash Hug Power  
  for(int i=0;i<2500;i+=500){
    if (newHugPower == hugPowerMax)
      neoFlashRange("rainbow", 0, newHugPower, 200, 200);
    else
      neoFlashRange("rainbow", hugPower+1, newHugPower, 200, 200);
    playToneHappy();
    delay(500);
  }  
  
  // RECORD HUG
  hugPower = newHugPower;
  incrementTotalHugsStore();
  setTimeOfLastHugToNow();
}











//MODES!
int subModeIncrementer = 1;


void hugMode(boolean init) {  
  
  while (upButtonPressed() || downButtonPressed()){
    neoClear();
    showTotalHugs();
    delay(300);
    neoClear();
  }

  vizRainbowSnake();
}


int start_point = 0;

void vizRainbowSnake(){
  int toggleAction = toggleTimerAction(300, 300);
  if (toggleAction){
    neoClear();  
    neoSetRange("rainbow", start_point, start_point+hugPower);
    start_point++;
  } 
}


// breaks if totalHugs > 60
void showTotalHugs() {
  int totalHugs = getTotalHugsStore();
  int numberingSystem = 10;
  
  if (totalHugs <= numberingSystem)
    neoSetRange(199,0,totalHugs-1);
  else {
    int numFullFills = totalHugs / numberingSystem;
    int remainingSingleHugs = totalHugs - (numFullFills * numberingSystem);
    neoSetRange("red",0,numFullFills-1);
    neoSetRange(199,numFullFills,numFullFills+remainingSingleHugs-1);
  }
}









int numSubModes = 6;
int vizColor = 0;

void vizMode(boolean init){  
  if (upButtonPressed() || downButtonPressed()){
    
    if (upButtonPressed()){ subModeIncrementer = 1; }
    else if (downButtonPressed()){ subModeIncrementer = -1; }
    
    favoriteVizMode = (numSubModes + favoriteVizMode + subModeIncrementer) % numSubModes;
    vizColor = 0;
    neoClear();
    
    setIdleAlarm();
    while (upButtonPressed() || downButtonPressed())
      delay(100);
  }

  switch(favoriteVizMode){
    case 0:
      vizCoolColorCycle();
      break;
    case 1: // strobe light
      neoFlashAll("white", 5,150);
      break;
    case 2: // bicycle safety
      neoFlashAll("white", 200,200);
      break;
    case 3: // plain rainbow
      neoSetAll("rainbow");
      break;
    case 4: // flashing rainbow
      neoFlashAll("rainbow",200,200);
      break;
    case 5:
      vizColorCycle();
      break;

    // add: color cycling modes
    // add: default color modes
    // add: cool color mode
    // add: breathing modes
  }
  
  // if it's been N seconds since this mode has changed, store in EEPROM   
  if (idleAlarmTriggered(secondsUntilStore))
    storeFavoriteVizMode();
}



void setColorMode(boolean init){ 
  if (upButtonPressed() || downButtonPressed()){
    if (upButtonPressed())
      favoriteColor = (255 + favoriteColor + 1) % 255;
    else if (downButtonPressed())
      favoriteColor = (255 + favoriteColor - 1) % 255;  
   
    setIdleAlarm();
    delay(10);
  }

  neoSetAll(Wheel(favoriteColor));
    
  // if it's been N seconds since this mode has changed, store in EEPROM 
  if (idleAlarmTriggered(secondsUntilStore)){
    storeFavoriteColor();
  }
}


unsigned long lastInteractionAt;
boolean notifiedOfTimeUp = true;

void setIdleAlarm(){
  lastInteractionAt = now();
  notifiedOfTimeUp = false;
}

boolean idleAlarmTriggered(int time){
  if (!notifiedOfTimeUp && (now() >= (lastInteractionAt + time)))
    return notifiedOfTimeUp = true;
  else
    return false;
}



//int cbIncrementer = 1;
//int colorBreatheColor = 0;
//
//void colorBreathe(int color){
//  int low = (color - 10) % 256;
//  int high = (color + 10) % 256;
//    
//  if (toggleTimerAction(25,25)){
//    if (colorBreatheColor < low || colorBreatheColor > high)
//      colorBreatheColor = low;
//    else if (colorBreatheColor == high) //127
//      cbIncrementer = -1;
//    else if (colorBreatheColor == low)
//      cbIncrementer = 1;
//    
//    Serial.println(colorBreatheColor);
//    colorBreatheColor = colorBreatheColor + cbIncrementer;
//    neoSetAll(Wheel(colorBreatheColor));
//  }
//}







void vizColorCycle(){
  if (toggleTimerAction(20,20)){
    vizColor = (vizColor + 1) % 255;
    neoSetAll(Wheel(vizColor));
//    Serial.println(vizColor);
//    delay(10);
  }
}

int incrementer = 1; // makes vizColor ping/pong vs. loop

void vizCoolColorCycle(){
  if (toggleTimerAction(25,25)){
    if (vizColor == 45) //127
      incrementer = -1;
    else if (vizColor == 10)
      incrementer = 1;
      
    vizColor = vizColor + incrementer;
    neoSetAll(Wheel(vizColor + 178));
  }
}


















// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i < neoRingSize; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow() {
  uint8_t wait = 5;
  
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<neoRingSize; i++) {
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
  int deadPixels = neoRingSize - hugPower;

  for(j=0,p=0; j<256*5; j++,p = p++ % neoRingSize) { // 5 cycles of all colors on wheel
    for(i=0; i < neoRingSize; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / neoRingSize) + j) & 255));
    }
    for(i=0; i < deadPixels; i++) {
      strip.setPixelColor((p+i) % neoRingSize, 0);
    }
    strip.show();
    delay(wait);
  }

}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < neoRingSize; i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < neoRingSize; i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < neoRingSize; i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < neoRingSize; i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}















//COLOR & LED HELPERS
void neoClear(){
  neoSetAll("black");
}

void neoSetAll(String color_name){
  neoSetRange(color_name,0,neoRingLastPixel);
}

void neoSetAll(uint32_t color){
  neoSetRange(color,0,neoRingLastPixel);
}

void neoFlashAll(String color_name, int periodOn, int periodOff){  
  neoFlashRange(color_name, 0, neoRingLastPixel, periodOn, periodOff);
}

//void neoSetCenter(String color_name){
//  strip.setPixelColor(neoCenterPixelAddress, color(color_name));
//  strip.show();
//}

//Range of Pixels
void neoSetRange(String color_name, int start_point, int end_point){
  if (color_name == "rainbow"){
    for (int i=start_point; i <= end_point; i++)
      setPixelToRainbow(i); strip.show();
  } else
    neoSetRange(color(color_name), start_point, end_point);
}

void neoSetRange(uint32_t color, int start_point, int end_point){
  for (int i=start_point; i <= end_point; i++)
    setPixelColor(i, color);   
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
  strip.setPixelColor(neoRingPixelAddress(i), c); 
}

void setPixelToRainbow(int i){
  setPixelColor(i, rainbowColor(i)); 
}



// Takes a number 0-15 and returns a number 0-15 that orients the ring according to where the necklace holds it.
int neoRingPixelAddress(int pixelNum){  
  return (neoRingOrigin + pixelNum) % neoRingSize;
}






















// EEPROM

unsigned long getTimeSinceLastHugStore(){
  return EEPROM.read(timeSinceLastHugAddress);
}
int getTotalHugsStore(){
  return EEPROM.read(totalHugsAddress);
}
int getFavoriteColorStore(){
  return EEPROM.read(favoriteColorAddress);
}
int getFavoriteVizModeStore(){
  return EEPROM.read(favoriteVizModeAddress);
}
boolean getMutedStore(){
  return EEPROM.read(mutedAddress);
}


void storeTimeSinceLastHug(){
  if (timeSinceLastHug() != getTimeSinceLastHugStore()){
    EepromAlarm();
    playToneSad(); playToneSad();
    EEPROM.write(timeSinceLastHugAddress,timeSinceLastHug());
  }
}
void incrementTotalHugsStore(){
  EepromAlarm();
  EEPROM.write(totalHugsAddress,(getTotalHugsStore()+1));
}
void storeFavoriteColor(){
  if (favoriteColor != getFavoriteColorStore()){
    EepromAlarm();
    EEPROM.write(favoriteColorAddress,favoriteColor);
  }
}
void storeFavoriteVizMode(){
  if (favoriteVizMode != getFavoriteVizModeStore()){
    EepromAlarm();
    EEPROM.write(favoriteVizModeAddress,favoriteVizMode);
  }
}
boolean storeMuted(){
  if (muted != getMutedStore()){
    EepromAlarm();
    EEPROM.write(mutedAddress,muted);
  }
}


// Since EEPROM is low-durability storage, it's important to ensure minimal writes (can only do 100,000). 
boolean EepromAlarmOn = false; // to trigger the alarm, turn this on
void EepromAlarm(){
  if (EepromAlarmOn){ playToneSad(); playToneSad(); }
}
