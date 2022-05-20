unsigned int freq = 120;

#include "Dimmy.h"

#define crossZeroPin 8

#define lightPinL 6   //Green
#define lightPinR 5   //Blue
#define lightPinC 11  //Red

Dimmy dimmy;
Channel lightL( lightPinL );
Channel lightR( lightPinR );
Channel lightC( lightPinC );

void  setupTimerTwo();

void setup() {
  setupTimerTwo();
  
  // again like in RSD, a combination of the static class with dinamics objets
  int dimmL = 20;
  int dimmR = 20;
  int dimmC = 25;

  lightL.set( dimmL );
  lightR.set( dimmR );
  lightC.set( dimmC );
  
  dimmy.attachChannel( &lightL );
  dimmy.attachChannel( &lightR );
  dimmy.attachChannel( &lightC );
  
  dimmy.begin( crossZeroPin ); 
  
  Serial.begin( 9600 );
}

void loop() {
  static unsigned int i;
  int valueR =  127 - 126*sin( i / (TWO_PI*10) );
  int valueL =  127 - 126*sin( i / (TWO_PI*10) + 10 );
  lightR.set( valueR );
  lightL.set( valueL );
  i++;
  Serial.println( valueL );
  delay( 40 );
}

void  setupTimerTwo(){
  //--> Automatic timer preescaler selector ( 8 bit , 10 bit preescaler )
    
  uint32_t ocr = F_CPU / freq ;
  uint8_t prescalarbits = 0b001;  // ck/1
      
  if ( ocr > 256 ) {
    ocr >>= 3; //divide by 8
    prescalarbits = 0b010;  // ck/8
    if ( ocr > 256 ) {
      ocr >>= 2; //divide by a further 4
      prescalarbits = 0b011; // ck/32
      if ( ocr > 256 ) {
        ocr >>= 1; //divide by a further 2
        prescalarbits = 0b100; // ck/64
        if ( ocr > 256 ) {
          ocr >>= 1; //divide by a further 2
          prescalarbits = 0b101; // ck/128
          if ( ocr > 256 ) {
            ocr >>= 1; //divide by a further 2
            prescalarbits = 0b110; // ck/256
            if ( ocr > 256 ) {
              // can't do any better than /1024
              ocr >>= 2; //divide by a further 4
              prescalarbits = 0b111; // ck/1024
            }
          }
        }
      }
    }
  }
  
  //<--

  // Fast PWM w/ OCR2A as TOP
  TCCR2A = ( 1 << WGM21 )|( 1 << WGM20 )|( 1 << COM2B1 ); // Clear OC2B on compare match, set OC2B at BOTTOM

  TCCR2B = ( 1 << WGM22 )|prescalarbits;
  
  OCR2A = ocr;
  OCR2B = OCR2A - OCR2A/10 ; // 90% PWM

  // pin-out Timmer 2
  // OC2A : D11, PB3, MOSI
  // OC2B : D3, PD3, INT1
  pinMode( 3 , OUTPUT );
  
}
