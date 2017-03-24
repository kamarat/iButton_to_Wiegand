/*== iButton to Wiegand ==
 *========================
 *
 * iButton_to_Wiegand.ino
 *
 * Program nacita ID z iButtona a vytvori signal v protokole Wiegand
 *
 * @author: kamarat
 * @date:   marec 2016 - marec 2017
 * 
 * @version: [2.1.0] - 2017-03-24
 * @version: [2.0.0] - 2017-03
 *
 * kamarat (cleft) 2016 - 2017
 *
 */

/*== ZAPOJENIE PINOV ==
 *=====================
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
 *=======================
 */
#include <OneWire.h>

/*== GLOBALNE PREMENNE ==
 *=======================
 */

// Definicia odladovania a vypisov
#define DEBUG 1

/*== Deklaracia konstant ==
 */
const uint8_t WIEGAND = 26;       // nastavenie protokolu
const uint8_t POCET_CITACIEK = 2;

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
  OneWire( CITACKA4 )
};

uint8_t addr[ 8 ] = {0};  // adresa + identifikator zariadenia - 8 bajtove pole
uint32_t paket = 0;       // vystupny paket s Wiegand protokolom
uint8_t citacka;          // aktivna citacka

String keyStatus = "";

/*== INICIALIZACIA ==
 *===================
 */
void setup()
{
  // Nastavenie datovych pinov pre vystup Wiegand
  for ( uint8_t i = W1_DATA0; i < ( POCET_CITACIEK * 2  + W1_DATA0); i++ ) {
    pinMode( i, OUTPUT );
    digitalWrite( i, HIGH );
  }

  Serial.begin( 57600 );  // Inicializacia serioveho vystupu
  #if DEBUG == 1
    Serial.println( F( "Inicializacia ukoncena." ));
  #endif
  delay (500);
}

/*== HLAVNY PROGRAM ==
 *====================
 */
void loop()
{
  for ( citacka = 0; citacka < POCET_CITACIEK; citacka++ ) {
    
    // Search for the next device. The addrArray is an 8 byte array.
    // If a device is found, addrArray is filled with the device's 
    // address and true is returned. If no more devices are found, false is returned. 
    if ( !dsButton[ citacka ].search( addr )) {
      dsButton[ citacka ].reset_search();  // Begin a new search. The next use of search will begin at the first device.
      keyStatus = "No iButton";
      continue;
    }
    
    // Compute a CRC check on an array of data.
    if ( OneWire::crc8( addr, 7 ) != addr[ 7 ]) {
      keyStatus = "CRC invalid";
      continue;
    }
    
    if ( addr[ 0 ] != 0x01 ) {
      keyStatus = "not DS1990A";
      continue;
    }
    
    keyStatus = "ok";
    dsButton[ citacka ].reset();
    
    #if DEBUG == 1
      Serial.print( F( "CITACKA " ));
      Serial.print( citacka + 1 );
      Serial.print( F( " = " ));
      for ( uint8_t i = 0; i < 8; i++ ) {
        Serial.print( addr[i], HEX );
        Serial.print( F( ":" ));
      }
      Serial.println(); 
    #endif
    
    vytvorTelo( &paket, addr );

    posliKod( &paket, citacka );
    paket = 0;
    keyStatus =  "";
    
    // Nastavenie oneskorenie kvoli nechcenemu opakovanemu dotyku iButtona
    delay( 800 );
  }
}

/*== DEFINICIE FUNKCII ==
 *=======================
 */

void vytvorTelo( uint32_t * paket, const uint8_t * addr )
{
  /* Vytvorenie paketu z hodnot ulozenych v poli addr
   * Seriove cislo je ulozene v poli addr ale v pakete
   * su ulozene len 3 byty - addr[1] - addr[3]
   */
  for ( uint8_t x = 1; x <= 3; x++ ) {
    *paket |= ( uint32_t )addr[ x ] << ( 16 - 8 * ( x - 1 ));  // posun postupne o 16, 8 a 0 bitov
  }
  
  #if DEBUG == 1
    Serial.println( F( "KOD: " ));
    Serial.println( *paket, BIN );
    Serial.println( *paket, DEC );
    Serial.println( *paket, HEX );
  #endif
  
  // Uvolnenie 0. bitu pre ulozenie parity
  *paket <<= 1;
  
  // Spocitanie parity pre masku 1111111111110
  // Zapise na 0. bit 1 ak je neparna parita
  *paket = *paket | ( countParity( *paket & 0x00001FFE ) & 1 );
  
  // Spocitanie parity pre masku 01111111111110000000000000
  // Zapise na 25. bit 1 ak je parna parita
  *paket = *paket | (( uint32_t )( ~countParity( *paket & 0x01FFE000 ) & 1 ) << 25 );
  
  #if DEBUG == 1
    Serial.println( F( "PAKET: " ));
    Serial.println( *paket, BIN );
    Serial.println( *paket, DEC );
    Serial.println( *paket, HEX );
  #endif
}

uint8_t countParity( uint32_t n )
{
  n = (( n & 0xAAAAAAAA ) >> 1 ) + ( n & 0x55555555 );
  n = (( n & 0x30C30C30 ) >> 4 ) + (( n & 0x0C30C30C ) >> 2 ) + ( n & 0xC30C30C3 );
  return ( n % 63 );
}

void posliKod( uint32_t *paket, uint8_t vystup )
{
  // Vypocet indexu pola W_VYSTUPY podla citacky
  vystup = vystup * 2;
  
  #if DEBUG == 1
    Serial.println( F( "WIEGAND = " ));
  #endif
  
  for ( short i = ( WIEGAND - 1 ); i >= 0; i-- ) {
    if ( bitRead( *paket, i ) == 1 ) {
      
      #if DEBUG == 1
        Serial.print( 1 );
      #endif
      
      digitalWrite( W_VYSTUPY[ vystup + 1 ], LOW );
      delayMicroseconds( 34 );
      digitalWrite( W_VYSTUPY[ vystup + 1 ], HIGH);
    }
    else if ( bitRead( *paket, i ) == 0 ) {
      
      #if DEBUG == 1
        Serial.print( 0 );
      #endif
      
      digitalWrite( W_VYSTUPY[ vystup ], LOW);
      delayMicroseconds( 35 );
      digitalWrite( W_VYSTUPY[ vystup ], HIGH);
    }
    
    delayMicroseconds( 1860 );
  }
  #if DEBUG == 1
    Serial.println();
    Serial.println();
  #endif
}

