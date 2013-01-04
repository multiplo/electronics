//=======================================================================================
//																		Debug Framework
//=======================================================================================

#include "debug.h"

void DBGInit(long baud){

  uint16_t baud_setting;
  bool use_u2x;

  // U2X mode is needed for baud rates higher than (CPU Hz / 16)
  if (baud > F_CPU / 16) {
    use_u2x = true;
  } else {
    // figure out if U2X mode would allow for a better connection
    
    // calculate the percent difference between the baud-rate specified and
    // the real baud rate for both U2X and non-U2X mode (0-255 error percent)
    uint8_t nonu2x_baud_error = abs((int)(255-((F_CPU/(16*(((F_CPU/8/baud-1)/2)+1))*255)/baud)));
    uint8_t u2x_baud_error = abs((int)(255-((F_CPU/(8*(((F_CPU/4/baud-1)/2)+1))*255)/baud)));
    
    // prefer non-U2X mode because it handles clock skew better
    use_u2x = (nonu2x_baud_error > u2x_baud_error);
  }
  
  if (use_u2x) {
    UCSR1A = 1 << U2X1;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    UCSR1A = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }

  // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
  UBRR1H = baud_setting >> 8;
  UBRR1L = baud_setting;

  sbi(UCSR1B, RXEN1);
  sbi(UCSR1B, TXEN1);
  sbi(UCSR1B, RXCIE1);

}

void DBGPrintChar(uint8_t c)
{
  while (!(UCSR1A & (1 << UDRE1)))
    ;

  UDR1 = c;
}

void DBGPrintString(const char * str)
{
  while (*str)
		DBGPrintChar(*str++);
}
