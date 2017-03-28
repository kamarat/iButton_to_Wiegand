#include "Wiegand.h"

uint8_t i;

/*******************************************************************************
 *    Function: posliKod
 * Description: Odoslanie kodu v prislusnom protokole
 *   Parameter: [IN] const uint8_t * k - unikatny ROM kod
 *              [IN] uint8_t p - typ protokolu - Wiegand 26/34/42
 *              [IN] uint8_t v - pole vystupov
 *      Return: nič
 ******************************************************************************/
void posliKod( uint8_t *k, uint8_t p, const uint8_t * v )
{
  struct Paket paket = { 0, k + 1, 0 };
  vytvorPaket( p, paket );

  #if DEBUG == 1
    Serial.print( F( "Paritny bit MSB: " ));
    Serial.println( paket.paritaMSB );
    Serial.println( F( "KOD: " ));
    for ( i = 0; i < p; i++ )
      Serial.print( paket.data[ i ], HEX );
    Serial.println();
    uint32_t dec = 0;
    for ( i = 0; i < p; i++ )
      dec |= paket.data[ i ] << ( 8 * ( p - 1 - i ));
    Serial.println( dec );
    for ( i = 0; i < p; i++ )
      Serial.print( paket.data[ i ], BIN );
    Serial.println();
    Serial.print( F( "Paritny bit LSB: " ));
    Serial.println( paket.paritaLSB );
  #endif

  posliBit( v[ paket.paritaMSB ]);
    for ( i = 0; i < p; i++) {
      for ( int8_t b = 7; b >= 0; b-- ) {
        posliBit( v[ paket.data[ i ] >> b ] & 0x01 );
      }
    }
  posliBit( v[ paket.paritaLSB ]);
}

/*******************************************************************************
 *    Function: vytvorPaket
 * Description: Vytvorenie paketu protokolu Wiegand
 *   Parameter: [IN] const uint8_t p - typ protokolu - Wiegand 26/34/42
 *              [OUT] struct Paket * pkt - vytvorenie paketu protokolu
 *      Return: nič
 ******************************************************************************/
static void vytvorPaket( uint8_t p, struct Paket & pkt )
{
  switch ( p ) {
    case WIEGAND26: {
      // Spocitanie parity pre hornych 12 bitov
      // Parny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove neparny
      pkt.paritaMSB = ( vypocitajParitu( pkt.data[ 0 ]) + vypocitajParitu( pkt.data[ 1 ] & 0xF0 )) & 0x01;
      
      // Spocitanie parity pre dolnych 12 bitov
      // Neparny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove parny
      pkt.paritaLSB = ~( vypocitajParitu( pkt.data[ 1 ] & 0x0F) + vypocitajParitu( pkt.data[ 2 ]) ) & 0x01;
    } break;
    case WIEGAND34: {
      // Spocitanie parity pre hornych 16 bitov
      // Parny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove neparny
      pkt.paritaMSB = ( vypocitajParitu( pkt.data[ 0 ]) + vypocitajParitu( pkt.data[ 1 ]) ) & 0x01;
      
      // Spocitanie parity pre dolnych 16 bitov
      // Neparny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove parny
      pkt.paritaLSB = ~( vypocitajParitu( pkt.data[ 2 ]) + vypocitajParitu( pkt.data[ 3 ]) ) & 0x01;
    } break;
    case WIEGAND42: {
      // Paritny bit nastaveny na 0
      pkt.paritaMSB = 0;
      // Spocitanie parity pre vsetkych 40 bitov
      // Parny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove neparny
      for ( i = 0; i < p; i++ )
        pkt.paritaLSB += vypocitajParitu( pkt.data[ i ]);
      pkt.paritaLSB &= 0x01;
    } break;
  }
}

/*******************************************************************************
 *    Function: vypocitajParitu
 * Description: Spocitanie poctu bitov s hodnotou 1 v premennej n
 *   Parameter: [IN] uint32_t n - bity pre urcenie parity
 *      Return: uint8_t - pocet bitov s hodnotou 1
 ******************************************************************************/
/*static uint8_t vypocitajParitu( uint32_t n )
{
  n = (( n & 0xAAAAAAAA ) >> 1 ) + ( n & 0x55555555 );
  n = (( n & 0x30C30C30 ) >> 4 ) + (( n & 0x0C30C30C ) >> 2 ) + ( n & 0xC30C30C3 );
  return ( n % 63 );
}*/

/*******************************************************************************
 *    Function: vypocitajParitu
 * Description: Spocitanie poctu bitov s hodnotou 1 v premennej n
 *   Parameter: [IN] uint8_t n - bity pre urcenie parity
 *      Return: uint8_t - pocet bitov s hodnotou 1
 ******************************************************************************/
static uint8_t vypocitajParitu( uint8_t n )
{
  n = (( n & 0xAA ) >> 1 ) + ( n & 0x55 );
  n = (( n & 0xC0 ) >> 6 ) + (( n & 0x30 ) >> 4 ) + (( n & 0x0C ) >> 2 ) + ( n & 0x03 );
  return n;
}

/*******************************************************************************
 *    Function: posliBit
 * Description: Odoslanie bitu protokolom Wiegand
 *   Parameter: [IN] uint8_t vystup - vystup DATA0 alebo DATA1
 *      Return: Nič
 ******************************************************************************/
static void posliBit( uint8_t vystup)
{
  digitalWrite( vystup, LOW );
  delayMicroseconds( 34 );          // cas zotrvania v stave LOW - 50 us
  digitalWrite( vystup, HIGH );
  delayMicroseconds( 1000 );        // prestavka medzi jednotlivymi bitmi - 1000 us
}

