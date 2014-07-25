// Includes
#include <Adafruit_NeoPixel.h>
#include <Time.h>

// I/O Pins
#define NeoPixelPIN 11
#define ForcePIN 5
#define BuzzerPIN 2 
#define ledPIN 3

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
//long vel = 20000;
//boolean hasplayed = false;





//HugPower Memory / Related Vars
int hugPower; 
unsigned long timeOfLastHug;
int msUntilPowerUp;
int hugPowerResiliance[16] =  { 
                                18144000, // Month
                                604800, // Week
                                86400, // Day
                                43200, // 12 Hours  
                                
                                21600, // 6 Hours                              
                                10800, // 3 Hours                              
                                3600, // 1 Hour
                                2700, // 45 min  
                                
                                11, // 30 min
                                10, // 15 min
                                9, // 5 min
                                8, // 1 min

                                7,
                                6,
                                5,
                                4                                
//                                1800, // 30 min
//                                900, // 15 min
//                                300, // 5 min
//                                60, // 1 min
//
//                                30,
//                                15,
//                                10,
//                                5
                              };
                           


void setup() {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
  
  // Initialize the strip
  strip.begin();
  strip.setBrightness(10);

  // Sing
  playToneHappy();
  playToneHappy();
  
  //Initialize the heart LED
  initHeartLED();

  // Set initial power-on hug level
  initHugPower(12);
}



void loop(){ 
  //do the rainbow thing
  rainbowMetered(5);
  
  // Drop Hug Power Over Time
  if (timeSinceLastHug() > hugPowerResiliance[hugPower-1])
    dropHugPower();

  // Add Hug Power When Hugged
  if (beingHugged()){
    delay(300);
    if (beingHugged()) {
      enterHugMode();
//      playToneHappy();
//      doFlirtRing(30,90);
      playToneHappy();
      playToneHappy();
    } else {
      playToneHappy();
      playToneHappy();
      doFlirtRing(30, 120);
      playToneHappy();
      theaterChase(strip.Color(127,   0,   0), 50);
      playToneHappy();
      playToneHappy();
    }
  }

  
  // Wait Between Loops
  delay(500);
}

void doFlirtRing(uint16_t wait, int loops) {
  uint16_t i, j,p;
  int deadPixels = 12; //strip.numPixels() - hugPower;

  for(j=0,p=0; j<loops; j++,p = p++ % strip.numPixels()) { // 5 cycles of all colors on wheel
    for(i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255,0,0));
    }
    for(i=0; i < deadPixels; i++) {
      strip.setPixelColor((p+i) % strip.numPixels(), 0);
    }
    strip.show();
    if (timeToBreak())
      break;
    delay(wait);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint16_t wait) {
  uint16_t i, j,p;
  int deadPixels = strip.numPixels() - hugPower;

  for(j=0,p=0; j<256*5; j++,p = p++ % strip.numPixels()) { // 5 cycles of all colors on wheel
    for(i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    for(i=0; i < deadPixels; i++) {
      strip.setPixelColor((p+i) % strip.numPixels(), 0);
    }
    strip.show();
    if (timeToBreak())
      break;
    delay(wait);
  }

}

boolean timeToBreak(){
  return (beingHugged() || timeToLoseHugPower());
}

boolean timeToLoseHugPower(){
  return (timeSinceLastHug() > hugPowerResiliance[hugPower-1]);
}


void initHeartLED(){
  digitalWrite(ledPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}


void enterHugMode(){
//  clearLEDs();
//  strip.show();  
  int powerOfHug = measureHug();
  playToneSad();
  delay(2000); //to show hugger
  playToneSad();  
  addHugPower(powerOfHug);

}

int measureHug(){
  int powerOfHug = 0;
  int hugTotal = 0;
  while (beingHugged()){
    powerOfHug++;
    hugTotal = hugPower+powerOfHug;
    
    if (hugTotal > strip.numPixels())
      break;
      
    setTimeOfLastHugToNow(); 
    playToneHappy();  

    flashOnLED(hugTotal, msUntilPowerUp);
  }   
  return powerOfHug;
}

void addHugPower(int powerOfHug){

  // add the HugPower
  hugPower = min((hugPower+powerOfHug), 16);
  
  if (powerOfHug >= 12){
    playMario(); 
    theaterChaseRainbow(50);
    playToneHappy();  
  } else if (hugPower >= strip.numPixels()) {
    playToneHappy();  
    playToneHappy();  
  } else {
    playToneHappy();  
    playToneHappy(); 
  }
    
}

void dropHugPower(){
  // Turn off the LED and Flash it Off for an amount of time commiserate with the inverse of the HugPower level
  hugPower = max((hugPower-1), 1);
}

boolean beingHugged(){
  forceReading = analogRead(ForcePIN);
//  if (forceReading < 200)
//    return false;
//  else {
//    delay(200);
//    forceReading = analogRead(ForcePIN);
  Serial.println(forceReading);  
  msUntilPowerUp = (1023 - forceReading) + 100;   
  return (forceReading > 200);
//  }
}

void setTimeOfLastHugToNow(){
  timeOfLastHug = now();
}

int timeSinceLastHug(){
  return (now() - timeOfLastHug);
}


//void flashOnLEDRed(int LEDNum, int time){
//  int numFlashes = 3;
//  for(int i = 0; i <= numFlashes; i++){
//    turnOnLEDRed(LEDNum);
//    delay((float)msUntilPowerUp * (2.0/(3.0*(float)numFlashes)) );
// 
//    turnOffLED(LEDNum);
//    delay((float)msUntilPowerUp * (1.1/(3.0*(float)numFlashes)) );
//  }
//  turnOnLEDRed(LEDNum);
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


void turnOnLED(int LEDNum){
  uint32_t color = strip.Color(255, 0, 0);
  LEDNum= (LEDNum + 3) % strip.numPixels();  
  strip.setPixelColor(LEDNum, color);
  strip.show();
}

void turnOffLED(int LEDNum){
  LEDNum= (LEDNum + 3) % strip.numPixels();
  strip.setPixelColor(LEDNum, 0);
  strip.show();
}

void initHugPower(int power){
  setTimeOfLastHugToNow();
  hugPower = power;
  for (int i = 0; i < hugPower+1; i++)
    turnOnLED(i);
}

void clearLEDs(){
  for (int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, 0);
}


//// Input a value 0 to 255 to get a color value.
//// The colours are a transition r - g - b - back to r.
uint32_t colorOfLED(byte WheelPos) {
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





int speaker = BuzzerPIN; 
long vel = 20000;
boolean hasplayed = false;
 
//void setup() {
//  pinMode(speaker, OUTPUT);
//  delay(2000);
//}

int melod[] = {tonec, toneG, toneE, toneA, toneB, toneBb, toneA, toneG, tonee, toneg, tonea, tonef, toneg, tonee, tonec, toned, toneB};
int ritmo[] = {18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 6};
 
void playMario() {
  pinMode(speaker, OUTPUT);
//  if (hasplayed == true){ return;}
  for (int i=0; i<17; i++) {
    int tom = melod[i];
    int tempo = ritmo[i];
 
    long tvalue = tempo * vel;

    tocar(tom, tvalue);

    delayMicroseconds(1000);
  }	    delay(1000);

  hasplayed = true;
}
 
void tocar(int tom, long tempo_value) {
  long tempo_gasto = 0;
  while (tempo_gasto < tempo_value) {
    digitalWrite(speaker, HIGH);
    delayMicroseconds(tom / 2);
 
    digitalWrite(speaker, LOW);
    delayMicroseconds(tom/2);	 
    tempo_gasto += tom;
  }
}

  









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







// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowMetered(uint8_t wait) {
  uint16_t i, j;
  clearLEDs();
      
  for(j=0; j<256; j++) {
    for(i=0; i<hugPower; i++) {
      strip.setPixelColor((i + 4) % strip.numPixels(), Wheel((i+j) & 255));
      if (timeToBreak())
        break;
    }
    if (timeToBreak())
      break;
    strip.show();

    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
//void rainbowCycle(uint16_t wait) {
//  uint16_t i, j,p;
//  int deadPixels = strip.numPixels() - hugPower;
//
//  for(j=0,p=0; j<256*5; j++,p = p++ % strip.numPixels()) { // 5 cycles of all colors on wheel
//    for(i=0; i < strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//    }
//    for(i=0; i < deadPixels; i++) {
//      strip.setPixelColor((p+i) % strip.numPixels(), 0);
//    }
//    strip.show();
//    if (beingHugged())
//      break;
//    delay(wait);
//  }
//
//}

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






/**
 *      ^   ^   ^  
 * ~~~~~ ColorWave ~~~~~
 *        V   V   V   
 */
void colorWave(uint8_t wait) {
  int i, j, stripsize, cycle;
  float ang, rsin, gsin, bsin, offset;
 
  static int tick = 0;
  
  stripsize = strip.numPixels();
  cycle = stripsize * 25; // times around the circle...
 
  while (++tick % cycle) {
    offset = map2PI(tick);
 
    for (i = 0; i < stripsize; i++) {
      ang = map2PI(i) - offset;
      rsin = sin(ang);
      gsin = sin(2.0 * ang / 3.0 + map2PI(int(stripsize/6)));
      bsin = sin(4.0 * ang / 5.0 + map2PI(int(stripsize/3)));
      strip.setPixelColor(i, strip.Color(trigScale(rsin), trigScale(gsin), trigScale(bsin)));
    }
 
    strip.show();
    delay(wait);
  }
 
}
 
/**
 * Scale a value returned from a trig function to a byte value.
 * [-1, +1] -> [0, 254] 
 * Note that we ignore the possible value of 255, for efficiency,
 * and because nobody will be able to differentiate between the
 * brightness levels of 254 and 255.
 */
byte trigScale(float val) {
  val += 1.0; // move range to [0.0, 2.0]
  val *= 127.0; // move range to [0.0, 254.0]
 
  return int(val) & 255;
}
 
/**
 * Map an integer so that [0, striplength] -> [0, 2PI]
 */
float map2PI(int i) {
  return PI*2.0*float(i) / float(strip.numPixels());
}


