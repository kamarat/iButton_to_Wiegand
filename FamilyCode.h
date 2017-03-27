#ifndef FAMILY_CODE_H
#define FAMILY_CODE_H

#include <Arduino.h>

/*
Family Code   Part
() - iButton Package  Description
(Memory size in bits unless specified)
01 (hex)  (DS1990A), (DS1990R), DS2401, DS2411  1-Wire net address (registration number) only
02  (DS1991)¹   Multikey iButton, 1152-bit secure memory
04  (DS1994), DS2404  4Kb NV RAM memory and clock, timer, alarms
05  DS2405¹   Single addressable switch
06  (DS1993)  4Kb NV RAM memory
08  (DS1992)  1Kb NV RAM memory
09  (DS1982), DS2502  1Kb EPROM memory
0A  (DS1995)  16Kb NV RAM memory
0B  (DS1985), DS2505  16Kb EPROM memory
0C  (DS1996)  64Kb NV RAM memory
0F  (DS1986), DS2506  64Kb EPROM memory
10  (DS1920)  Temperature with alarm trips
12  DS2406, DS2407¹   1Kb EPROM memory, 2-channel addressable switch
14  (DS1971), DS2430A¹  256-bit EEPROM memory and 64-bit OTP register
1A  (DS1963L)¹  4Kb NV RAM memory with write cycle counters
1C  DS28E04-100   4096-bit EEPROM memory, 2-channel addressable switch
1D  DS2423¹   4Kb NV RAM memory with external counters
1F  DS2409¹   2-channel addressable coupler for sub-netting
20  DS2450  4-channel A/D converter (ADC)
21  (DS1921G), (DS1921H), (DS1921Z)   Thermochron® temperature logger
23  (DS1973), DS2433  4Kb EEPROM memory
24  (DS1904), DS2415  Real-time clock (RTC)
27  DS2417  RTC with interrupt
29  DS2408  8-channel addressable switch
2C  DS2890¹   1-channel digital potentiometer
2D  (DS1972), DS2431  1024-bit, 1-Wire EEPROM
37  (DS1977)  Password-protected 32KB (bytes) EEPROM
3A  (DS2413)  2-channel addressable switch
41  (DS1922L), (DS1922T), (DS1923), DS2422  High-capacity Thermochron (temperature) and Hygrochron™ (humidity) loggers
42  DS28EA00  Programmable resolution digital thermometer with sequenced detection and PIO
43  DS28EC20  20Kb 1-Wire EEPROM
*/

//            iBUTTON   FAMILY CODE   POPIS ( Pokial nie je uvedene inak, velkost pamate je v bitoch)
const uint8_t DS1990A = 0x01;         // DS1990R, DS2401, DS2411  1-Wire net address (registration number) only
const uint8_t DS1991  = 0x02;         // Multikey iButton, 1152-bit secure memory



#endif  // #ifndef FAMILY_CODE_H
