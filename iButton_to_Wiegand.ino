/*== iButton to Wiegand ==
 *========================
 *
 * iButton_to_Wiegand.ino
 *
 * Program nacita ID z iButtona a vytvori signal v protokole Wiegand
 *
 * @author: kamarat
 * @date:   marec 2016 - maj 2017
 *
 * @version: [3.0.1] - 2017-05-12
 *    - Zmena zapojenia pinov
 *    - Skratenie doby spanku
 *
 * @version: [3.0.0] - 2017-03-27
 *    - Doplnenie protokolu Wiegand 34
 *    - Spracovanie protokolu v oddelených súboroch
 *    - Úprava identifikácie na základe tzv. Family Code
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
 *
 * D  2 - kontakt TM citacky 1 iButton
 * D  5 - kontakt TM citacky 2 iButton
 * D 10 - kontakt TM citacky 3 iButton
 * D 13 - kontakt TM citacky 4 iButton
 *
 * D  3 - vystup Wiegand 1 DATA0
 * D  4 - vystup Wiegand 1 DATA1
 * D  6 - vystup Wiegand 2 DATA0
 * D  7 - vystup Wiegand 2 DATA1
 * D  8 - vystup Wiegand 3 DATA0
 * D  9 - vystup Wiegand 3 DATA1
 * D 12 - vystup Wiegand 4 DATA0
 * D 11 - vystup Wiegand 4 DATA1
 *
 * Rezistor 2k2 - medzi D2 a 5V
 * Rezistor 2k2 - medzi D5 a 5V
 * Rezistor 2k2 - medzi D10 a 5V
 * Rezistor 2k2 - medzi D13 a 5V
 */

/*== KNIZNICE A SUBORY ==
 *=======================
 */
#include <OneWire.h>
#include "LowPower.h"
#include "Wiegand.h"

/*== GLOBALNE PREMENNE ==
 *=======================
 */

// Definicia odladovania a vypisov
#define DEBUG 1

/*== Deklaracia konstant ==
 */
const uint8_t POCET_SLOTOV = 4;

// citacka, D0, D1, protokol
const struct Slot slot[] {
  {  2, {  3,  4 }, WIEGAND34 },
  {  5, {  6,  7 }, WIEGAND34 },
  { 10, {  8,  9 }, WIEGAND34 },
  { 13, { 12, 11 }, WIEGAND34 }
};

/*== Deklaracia premennych ==
 */

OneWire citacka[] = {
  OneWire( slot[0].citacka ),
  OneWire( slot[1].citacka ),
  OneWire( slot[2].citacka ),
  OneWire( slot[3].citacka )
};

uint8_t unikatnyROMKod[ 8 ] = {0};  // adresa + identifikator zariadenia - 8 bajtove pole
uint8_t pouzitySlot;                // aktivny slot

enum vysledkyNacitania {
  OK = 0,
  CRC_INVALID,
  NOT_DS1990A,
  NO_IBUTTON
} vysledokNacitania;

/*== INICIALIZACIA ==
 *===================
 */
void setup()
{
  #if DEBUG == 1
    Serial.begin( 57600 );  // Inicializacia serioveho vystupu
  #endif

  // Nastavenie datovych pinov pre vystup Wiegand
  for ( uint8_t i = 0; i < POCET_SLOTOV; i++ ) {
    pinMode( slot[ i ].vystup[ 0 ], OUTPUT );
    pinMode( slot[ i ].vystup[ 1 ], OUTPUT );
    digitalWrite( slot[ i ].vystup[ 0 ], HIGH );
    digitalWrite( slot[ i ].vystup[ 1 ], HIGH );
  }

  #if DEBUG == 1
    Serial.println( F( "Inicializacia ukoncena." ));
  #endif
  delay ( 500 );
}

/*== HLAVNY PROGRAM ==
 *====================
 */
void loop()
{
  // Uspatie na 120 ms, nasledne prebudenie pomocou Watchdog casovaca
  LowPower.powerDown( SLEEP_120MS, ADC_OFF, BOD_OFF );

  for ( pouzitySlot = 0; pouzitySlot < POCET_SLOTOV; pouzitySlot++ ) {
    /* Vyhladanie aktivneho kluca na citacke. Ak je kluc najdeny, pole unikatnyROMKod je naplnene
     * jedinecnym cislom a metoda search vrati TRUE. Ak kluc nie je najdeny, metoda vrati FALSE.
     */
    if ( !( citacka[ pouzitySlot ].search( unikatnyROMKod ))) {
      // Zacatie noveho vyhladavania kluca. Dalsie pouzitie search vrati udaje o dalsom kluci.
      citacka[ pouzitySlot ].reset_search();
      vysledokNacitania = NO_IBUTTON;
      continue;
    }

    // Vypocet a kontrola CRC nad polom unikatnyROMKod.
    if ( OneWire::crc8( unikatnyROMKod, 7 ) != unikatnyROMKod[ 7 ]) {
      vysledokNacitania = CRC_INVALID;
      continue;
    }

    if ( unikatnyROMKod[ 0 ] != DS1990A ) {
      vysledokNacitania = NOT_DS1990A;
      continue;
    }

    vysledokNacitania = OK;
    citacka[ pouzitySlot ].reset();

    #if DEBUG == 1
      Serial.print( F( "CITACKA " ));
      Serial.print( pouzitySlot + 1 );
      Serial.print( F( ": " ));
      for ( uint8_t i = 0; i < 8; i++ ) {
        Serial.print( unikatnyROMKod[ i ], HEX );
        Serial.print( F( " " ));
      }
      Serial.println();
    #endif

    posliKod( unikatnyROMKod, slot[ pouzitySlot ].protokol, slot[ pouzitySlot ].vystup );
  }
}
