/*
 * Configuración del Timmer 2 para tener señal de cruce por cero para experimentación y desarrollo sin que estar
 * conectado a la red de 220.
 * 
 * Posibilidad de usar Tone como referencia para tener una flexibilidad en la frecuencia generada, 60 Hz ideal
 * para usar cámara como osciloscopio.
 * 
 * A diferencia de Tone, NO usar interrupciones, solo PWM automático. Salidas fijas.
 * 
 * Documentanción: 
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 *  
 *  
 */

int freq = 120;

void setup() {

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

  pinMode( 8 , INPUT );
  
  Serial.begin( 112500 );

}

void loop() {
  digitalRead( 8 ) ? Serial.println( 1 ) : Serial.println( 0 );
}
