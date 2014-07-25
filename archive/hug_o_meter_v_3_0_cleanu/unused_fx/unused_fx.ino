//
///**
// *      ^   ^   ^  
// * ~~~~~ ColorWave ~~~~~
// *        V   V   V   
// */
//void colorWave(uint8_t wait) {
//  int i, j, stripsize, cycle;
//  float ang, rsin, gsin, bsin, offset;
// 
//  static int tick = 0;
//  
//  stripsize = strip.numPixels();
//  cycle = stripsize * 25; // times around the circle...
// 
//  while (++tick % cycle) {
//    offset = map2PI(tick);
// 
//    for (i = 0; i < stripsize; i++) {
//      ang = map2PI(i) - offset;
//      rsin = sin(ang);
//      gsin = sin(2.0 * ang / 3.0 + map2PI(int(stripsize/6)));
//      bsin = sin(4.0 * ang / 5.0 + map2PI(int(stripsize/3)));
//      strip.setPixelColor(i, strip.Color(trigScale(rsin), trigScale(gsin), trigScale(bsin)));
//    }
// 
//    strip.show();
//    delay(wait);
//  }
// 
//}
 
/**
 * Scale a value returned from a trig function to a byte value.
 * [-1, +1] -> [0, 254] 
 * Note that we ignore the possible value of 255, for efficiency,
 * and because nobody will be able to differentiate between the
 * brightness levels of 254 and 255.
 */
//byte trigScale(float val) {
//  val += 1.0; // move range to [0.0, 2.0]
//  val *= 127.0; // move range to [0.0, 254.0]
// 
//  return int(val) & 255;
//}
// 
///**
// * Map an integer so that [0, striplength] -> [0, 2PI]
// */
//float map2PI(int i) {
//  return PI*2.0*float(i) / float(strip.numPixels());
//}





//// Fill the dots one after the other with a color
//void colorWipe(uint32_t c, uint8_t wait) {
//  for(uint16_t i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, c);
//      strip.show();
//      delay(wait);
//  }
//}
//
//
//void rainbow(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//      if (isCompressed())
//        return;
//    }
//    strip.show();
//    delay(wait);
//  }
//}



//Theatre-style crawling lights.
//void theaterChase(uint32_t c, uint8_t wait) {
//  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
//    for (int q=0; q < 3; q++) {
//      for (int i=0; i < strip.numPixels(); i=i+3) {
//        strip.setPixelColor(i+q, c);    //turn every third pixel on
//      }
//      strip.show();
//     
//      delay(wait);
//     
//      for (int i=0; i < strip.numPixels(); i=i+3) {
//        strip.setPixelColor(i+q, 0);        //turn every third pixel off
//      }
//    }
//  }
//}

////Theatre-style crawling lights with rainbow effect
//void theaterChaseRainbow(uint8_t wait) {
//  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
//    for (int q=0; q < 3; q++) {
//        for (int i=0; i < strip.numPixels(); i=i+3) {
//          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
//        }
//        strip.show();
//       
//        delay(wait);
//       
//        for (int i=0; i < strip.numPixels(); i=i+3) {
//          strip.setPixelColor(i+q, 0);        //turn every third pixel off
//        }
//    }
//  }
//}



//
//int speaker = BuzzerPIN; 
//long vel = 20000;
//boolean hasplayed = false;
// 
//
//
//int melod[] = {tonec, toneG, toneE, toneA, toneB, toneBb, toneA, toneG, tonee, toneg, tonea, tonef, toneg, tonee, tonec, toned, toneB};
//int ritmo[] = {18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 6};
// 
//void playMario() {
//  pinMode(speaker, OUTPUT);
////  if (hasplayed == true){ return;}
//  for (int i=0; i<17; i++) {
//    int tom = melod[i];
//    int tempo = ritmo[i];
// 
//    long tvalue = tempo * vel;
//
//    tocar(tom, tvalue);
//
//    delayMicroseconds(1000);
//  }	    delay(1000);
//
//  hasplayed = true;
//}
// 
//void tocar(int tom, long tempo_value) {
//  long tempo_gasto = 0;
//  while (tempo_gasto < tempo_value) {
//    digitalWrite(speaker, HIGH);
//    delayMicroseconds(tom / 2);
// 
//    digitalWrite(speaker, LOW);
//    delayMicroseconds(tom/2);	 
//    tempo_gasto += tom;
//  }
//}





//
//void flashOnLED(int LEDNum, int time){
//  int numFlashes = 3;
//  for(int i = 0; i <= numFlashes; i++){
//    turnOnLED(LEDNum);
//    delay((float)msUntilPowerUp * (2.0/(3.0*(float)numFlashes)) );
// 
//    turnOffLED(LEDNum);
//    delay((float)msUntilPowerUp * (1.1/(3.0*(float)numFlashes)) );
//  }
//  turnOnLED(LEDNum);
//}
//
//
//void turnOnLED(int LEDNum){
//  uint32_t color = strip.Color(255, 0, 0);
//  LEDNum= (LEDNum + 3) % strip.numPixels();  
//  strip.setPixelColor(LEDNum, color);
//  strip.show();
//}
//
//void turnOffLED(int LEDNum){
//  LEDNum= (LEDNum + 3) % strip.numPixels();
//  strip.setPixelColor(LEDNum, 0);
//  strip.show();
//}
//

//
//void clearLEDs(){
//  for (int i = 0; i < strip.numPixels(); i++)
//    strip.setPixelColor(i, 0);
//}
//
//
////// Input a value 0 to 255 to get a color value.
////// The colours are a transition r - g - b - back to r.
//uint32_t colorOfLED(byte WheelPos) {
//  if(WheelPos < 85) {
//   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//  } else if(WheelPos < 170) {
//   WheelPos -= 85;
//   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  } else {
//   WheelPos -= 170;
//   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//}

void initHeartLED(){
  digitalWrite(ledPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
