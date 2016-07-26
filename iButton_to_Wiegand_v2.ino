/*== iButton to Wiegand ==
* ================================
*
* iButton_to_Wiegand.ino - v2
*
* Program nacita ID z iButtona a vytvori signal v protokole Wiegand
*
* @author: mr.nobody
* @date:   marec 2016
*
* mr.nobody (cleft) 2016
*/

/*== ZAPOJENIE PINOV ==
* =====================
*
* ARDUINO --- PERIFERIE
* A   0 -
*
* D  10 - kontakt TM citacky 1 iButton
* D  11 - kontakt TM citacky 2 iButton
* D  12 - kontakt TM citacky 3 iButton
* D  13 - kontakt TM citacky 4 iButton
*
* D   2 - vystup Wiegand 1 DATA0
* D   3 - vystup Wiegand 1 DATA1
* D   4 - vystup Wiegand 2 DATA0
* D   5 - vystup Wiegand 2 DATA1
* D   6 - vystup Wiegand 3 DATA0
* D   7 - vystup Wiegand 3 DATA1
* D   8 - vystup Wiegand 4 DATA0
* D   9 - vystup Wiegand 4 DATA1
*
* Rezistor 2k2 - medzi D10 a 5V
* Rezistor 2k2 - medzi D11 a 5V
* Rezistor 2k2 - medzi D12 a 5V
* Rezistor 2k2 - medzi D13 a 5V
*/

/*== KNIZNICE A SUBORY ==
* =======================
*/
#include <OneWire.h>

/*== GLOBALNE PREMENNE ==
* =======================
* premenne popisujuce zapojene piny
* premenne popisujuce nastavenie Arduina
*/

// Definicia odladovania a vypisov
#define DEBUG 1

/*== Deklaracia konstant ==
*/
const uint8_t WIEGAND = 26; // nastavenie protokolu
const uint8_t POCET_CITACIEK = 2;

// analogove piny

// digitalne piny
// pripojenie citaciek na digitalne piny
const uint8_t CITACKA1 = 10;
const uint8_t CITACKA2 = 11;
const uint8_t CITACKA3 = 12;
const uint8_t CITACKA4 = 13;
const uint8_t CITACKY[] = { CITACKA1, CITACKA2, CITACKA3, CITACKA4 };
// zapojenie Wiegand vystupov
const uint8_t W1_DATA0 = 2;
const uint8_t W1_DATA1 = 3;
const uint8_t W2_DATA0 = 4;
const uint8_t W2_DATA1 = 5;
const uint8_t W3_DATA0 = 6;
const uint8_t W3_DATA1 = 7;
const uint8_t W4_DATA0 = 8;
const uint8_t W4_DATA1 = 9;
const uint8_t W_VYSTUPY[] = { W1_DATA0, W1_DATA1, W2_DATA0, W2_DATA1, W3_DATA0, W3_DATA1, W4_DATA0, W4_DATA1 };

/*== Deklaracia premennych ==
*/
OneWire dsButton[ 4 ] = {
  OneWire( CITACKA1 ),
  OneWire( CITACKA2 ),
  OneWire( CITACKA3 ),
  OneWire( CITACKA4 ) };
uint8_t addr[8] = {0};
String keyStatus = "";
uint32_t paket = 0;

/*== DEFINICIA FUNKCII ==
* =======================
*/
  

void vytvorTelo()
{
  //paket = 0;
  for ( uint8_t x = 1; x < 4; x++ ) {
    paket |= addr[x];
    if ( x < 3 ) {
      paket <<= 8;
    }
    else {
      paket <<= 1;
    }
  }
  
  #if DEBUG == 1
  Serial.println("kod ");
  Serial.println(paket >> 1, BIN);
  Serial.println(paket >> 1, DEC);
  Serial.println(paket >> 1, HEX);
  #endif

  uint32_t paketParita = paket >> 1;

  for ( uint8_t i = 0; i < 2; i++ ) {
    uint8_t parita = 0;
    for ( uint8_t y = 0; y < 12; y++ ) {
      parita += bitRead( paketParita, y + 12 * i );
    }
    
    #if DEBUG == 1
    Serial.print("i=");
    Serial.print(i);
    Serial.print(" parita=");
    Serial.print(parita);
    Serial.print(" !(parita&1)=");
    Serial.print(!(parita & 1));
    Serial.print(" (i ^ !( parita & 1)=");
    Serial.print((i ^ !( parita & 1)));
    Serial.print(" 1 << ( i * 26)=");
    Serial.print((uint32_t)1 << ( i * WIEGAND - 1));
    Serial.println();
    #endif

    paket |= (( uint32_t )(i ^ !( parita & 1)) << ( i * WIEGAND - 1));
  }
  
  #if DEBUG == 1
  Serial.println("paket ");
  Serial.println(paket, BIN);
  Serial.println(paket, DEC);
  Serial.println(paket, HEX);
  #endif
}

void posliKod(uint8_t vystup)
{
  vystup = vystup * 2;
  
  #if DEBUG == 1
  Serial.println("Wiegand");
  #endif
  
  for ( short i = ( WIEGAND - 1 ); i >= 0; i-- ) {
    if ( bitRead( paket, i ) == 1 ) {
      
      #if DEBUG == 1
      Serial.print( 1 );
      #endif
      
      digitalWrite( W_VYSTUPY[ vystup + 1 ], LOW );
      delayMicroseconds( 34 );
      digitalWrite( W_VYSTUPY[ vystup + 1 ], HIGH);
    }
    else if ( bitRead( paket, i ) == 0 ) {
      
      #if DEBUG == 1
      Serial.print( 0 );
      #endif
      
      digitalWrite( W_VYSTUPY[ vystup ], LOW);
      delayMicroseconds( 35 );
      digitalWrite( W_VYSTUPY[ vystup ], HIGH);
    }
    delayMicroseconds( 1860 );

  }
  Serial.println();
}


/*== INICIALIZACIA ==
* ===================
*/
void setup()
{
  // inicializacia tried OneWire pre jednotlive citacky
  // konstruktor triedy obsahuje aj inicializaciu pinov
  //for ( uint8_t i = 0; i < POCET_CITACIEK; i++ ) {
  //  dsButton[ i ] = OneWire( CITACKY[ CITACKA1 + i ]);
  //}

  // nastavenie datovych pinov pre vystup Wiegand
  for ( uint8_t i = W1_DATA0; i < ( POCET_CITACIEK * 2  + W1_DATA0); i++ ) {
    pinMode( i, OUTPUT );
    digitalWrite( i, HIGH );
  }

  Serial.begin(57600); // Inicializacia serioveho vystupu
  Serial.println("Inicializacia ukoncena.");
  delay (500);
}


/*== HLAVNY PROGRAM ==
* ====================
*/
void loop()
{
  for ( uint8_t i = 0; i < POCET_CITACIEK; i++ ) {
    
    if ( !dsButton[ i ].search( addr )) {
      dsButton[ i ].reset_search();
      keyStatus = "No iButton";
      continue;
    }
    
    if ( OneWire::crc8( addr, 7 ) != addr[ 7 ]) {
      keyStatus = "CRC invalid";
      continue;
    }
    
    if ( addr[ 0 ] != 0x01 ) {
      keyStatus = "not DS1990A";
      continue;
    }
    
    keyStatus = "ok";
    dsButton[ i ].reset();
    
    #if DEBUG == 1
    Serial.print( "Citacka " );
    Serial.print( i + 1 );
    Serial.print( " = ");
    for ( uint8_t i = 0; i < 8; i++ ) {
      Serial.print( addr[i], HEX );
      Serial.print( ":" );
    }
    Serial.println( "" ); 
    #endif
    
    vytvorTelo();

    posliKod(i);
    paket = 0;
    keyStatus =  "";
    
    // nastavenie oneskorenie kvoli nechcenemu opakovanemu dotyku iButtona
    delay( 1000 );
  }
    
  //delay(1000);
}
