#include "Wiegand.h"

/*******************************************************************************
 *    Function: posliKod
 * Description: Odoslanie kodu v prislusnom protokole
 *   Parameter: [IN] const uint8_t * k - unikatny ROM kod
 *              [IN] uint8_t p - typ protokolu - Wiegand 26/34
 *              [IN] uint8_t v - pole vystupov
 *      Return: nič
 ******************************************************************************/
void posliKod( uint8_t *k, uint8_t p, const uint8_t * v )
{
  struct Paket paket;
  paket.paritaLSB = 0;
  paket.data = 0;
  paket.paritaMSB = 0;
  vytvorPaket( k, p, paket );

  #if DEBUG == 1
    Serial.print( F( "Paritny bit MSB: " ));
    Serial.println( paket.paritaMSB );
    Serial.println( F( "KOD: " ));
    Serial.println( paket.data, BIN );
    Serial.println( paket.data, DEC );
    Serial.println( paket.data, HEX );
    Serial.print( F( "Paritny bit LSB: " ));
    Serial.println( paket.paritaLSB );
  #endif

  posliBit( v[ paket.paritaMSB ]);
  for ( int8_t i = ( p * 8 - 1 ); i >= 0; i-- )
    posliBit(( paket.data >> i ) & 0x01 );
  posliBit( v[ paket.paritaLSB ]);
}

/*******************************************************************************
 *    Function: vytvorPaket
 * Description: Vytvorenie paketu protokolu Wiegand
 *   Parameter: [IN] const uint8_t * k - unikatny ROM kod
 *              [IN] const uint8_t p - typ protokolu - Wiegand 26/34
 *              [OUT] struct Paket * pkt - vytvorenie paketu protokolu
 *      Return: nič
 ******************************************************************************/
void vytvorPaket( uint8_t *k, uint8_t p, struct Paket & pkt )
{
  switch ( p ) {
    case WIEGAND26: {
      for ( uint8_t i = 0; i < WIEGAND26; i++ )
        pkt.data |= ( uint32_t )k[ i + 1 ] << ( 8 * ( 2 - i ));
      
      // Spocitanie parity pre masku 111111111111000000000000
      // Parny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove neparny
      pkt.paritaMSB = vypocitajParitu( pkt.data & 0x00FFF000 ) & 0x01;
      
      // Spocitanie parity pre masku 111111111111
      // Neparny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove parny
      pkt.paritaLSB = ~vypocitajParitu( pkt.data & 0x00000FFF ) & 0x01;
      
    } break;
    case WIEGAND34: {
      for ( uint8_t i = 0; i < WIEGAND34; i++ )
        pkt.data |= ( uint32_t )k[ i + 1 ] << ( 8 * ( 3 - i));
      
      // Spocitanie parity pre masku 11111111111111110000000000000000
      // Parny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove neparny
      pkt.paritaMSB = vypocitajParitu( pkt.data & 0xFFFF0000 ) & 0x01;
      
      // Spocitanie parity pre masku 00000000000000001111111111111111
      // Neparny paritny bit je nastaveny na 1, ak je pocet jednotiek v datovom slove parny
      pkt.paritaLSB = ~vypocitajParitu( pkt.data & 0x0000FFFF ) & 0x01;
    } break;
  }
}

/*******************************************************************************
 *    Function: vypocitajParitu
 * Description: Spocitanie poctu bitov s hodnotou 1 v premennej n
 *   Parameter: [IN] uint32_t n - udp instance
 *      Return: uint8_t - pocet bitov s hodnotou 1
 ******************************************************************************/
uint8_t vypocitajParitu( uint32_t n )
{
  n = (( n & 0xAAAAAAAA ) >> 1 ) + ( n & 0x55555555 );
  n = (( n & 0x30C30C30 ) >> 4 ) + (( n & 0x0C30C30C ) >> 2 ) + ( n & 0xC30C30C3 );
  return ( n % 63 );
}

/*******************************************************************************
 *    Function: posliBit
 * Description: Odoslanie bitu protokolom Wiegand
 *   Parameter: [IN] uint8_t vystup - vystup DATA0 alebo DATA1
 *      Return: Nič
 ******************************************************************************/
void posliBit( uint8_t vystup)
{
  digitalWrite( vystup, LOW );
  delayMicroseconds( 34 );          // cas zotrvania v stave LOW - 50 us
  digitalWrite( vystup + 1, HIGH );
  delayMicroseconds( 1860 );        // prestavka medzi jednotlivymi bitmi - 2 s
}

