#ifndef __DEBUG_FMWRK__
#define __DEBUG_FMWRK__

#include <avr/io.h>
#include <stdbool.h>

#define abs(x) ((x)>0?(x):-(x))

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Debug Framework prototypes
void DBGInit(long baud);
void DBGPrintChar(uint8_t c);
void DBGPrintString(const char * str);

#endif