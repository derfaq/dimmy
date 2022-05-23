const byte sin_table[256] PROGMEM = {127, 130, 133, 136, 139, 142, 145, 148, 151, 154, 157, 161, 164, 166, 169, 172, 175, 178, 181, 184, 187, 189, 192, 195, 197, 200, 202, 205, 207, 210, 212, 214, 217, 219, 221, 223, 225, 227, 229, 231, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 251, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 252, 252, 251, 251, 250, 249, 249, 248, 247, 246, 245, 243, 242, 241, 239, 238, 236, 235, 233, 231, 230, 228, 226, 224, 222, 220, 218, 215, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 180, 177, 174, 171, 168, 165, 162, 159, 156, 153, 150, 147, 144, 141, 137, 134, 131, 128, 125, 122, 119, 116, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 76, 73, 71, 68, 65, 62, 60, 57, 54, 52, 49, 47, 44, 42, 40, 38, 35, 33, 31, 29, 27, 25, 23, 22, 20, 18, 17, 15, 14, 12, 11, 10, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 16, 17, 19, 21, 22, 24, 26, 28, 30, 32, 34, 36, 39, 41, 43, 46, 48, 51, 53, 56, 58, 61, 64, 66, 69, 72, 75, 78, 81, 84, 87, 89, 92, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 127};
int pins[10] = { 2 , 4 , 5 , 6 , 7 , 9 , 10 , 11 , 12 , 13 };

#include "Dimmy.h"
Dimmy dimmy;

#define crossZeroPin 8

Channel l0( pins[0] );
Channel l1( pins[1] );
Channel l2( pins[2] );
Channel l3( pins[3] );
Channel l4( pins[4] );
Channel l5( pins[5] );
Channel l6( pins[6] );
Channel l7( pins[7] );
Channel l8( pins[8] );
Channel l9( pins[9] );

Channel* light[10] = { &l0 , &l1 , &l2 , &l3 , &l4 , &l5 , &l6 , &l7 , &l8 , &l9 };

unsigned int freq = 120;
void setupTimerTwo();

void setup() {
  
  setupTimerTwo();
  
  for (int i = 0 ; i < 10 ; i++ ) {
    dimmy.attachChannel( light[i] );
  }

  dimmy.begin( crossZeroPin );

  Serial.begin( 115200 );

}

int delayVal = 24;
byte counter[10] = { 0 , (255 * (1)) / 10 , (255 * (2)) / 10 , (255 * (3)) / 10 , (255 * (4)) / 10 , (255 * (5)) / 10 , (255 * (6)) / 10 , (255 * (7)) / 10 , (255 * (8)) / 10 , (255 * (9)) / 10 };

void loop() {
  
  static unsigned long lastTime;
  
  if ( ( millis() - lastTime ) > delayVal ) { 
    lastTime = millis();
    
    for ( int i = 0 ; i < 10 ; i++ ) {
      light[i]->set( pgm_read_byte( &sin_table[counter[i]] ) );
      counter[i]++;
      if ( counter[i] == 255 ) counter[i] = 0;
    } 
  }

  Serial.println( light[0]->get() );
  
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
