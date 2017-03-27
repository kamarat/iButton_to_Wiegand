#ifndef WIEGAND_H
#define WIEGAND_H

#include <Arduino.h>
#include "FamilyCode.h"

#define DEBUG 1

// Definicie protokolu
const uint8_t WIEGAND26 = 3;  // pocet bajtov predstavujucich data
const uint8_t WIEGAND34 = 4;

// Struktura pre urcenie vtahu vstupov a vystupov
struct Slot {
  uint8_t citacka;    // vstupny pin pre pripojenie citacky
  uint8_t vystup[2];  // vystupne piny - D0 (index 0), D1 (index 1)
  uint8_t protokol;
};

// Struktura protokolu Wiegand 26/34
struct Paket {
  uint8_t paritaMSB;
  uint32_t data;
  uint8_t paritaLSB;
};

// Deklaracia funkcii
extern void posliKod( uint8_t * k, uint8_t p, const uint8_t * v );

static void vytvorPaket( uint8_t * k, uint8_t p, struct Paket & pkt );
static uint8_t vypocitajParitu( uint32_t n );
static void posliBit( uint8_t vystup);

#endif  // #ifndef WIEGAND_H
