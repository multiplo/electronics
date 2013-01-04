
rem Erase memory (-e); Disables safe mode (-u); Writes fuses (-U):

.\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -e -u -Uefuse:w:0xFD:m
.\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -e -u -Uhfuse:w:0xD8:m 
.\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -e -u -Ulfuse:w:0xFF:m

rem Burns the bootloader in flash (does not verifies -V):
.\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -V -F -U flash:w:".\DuinoBot.v1.x.Bootloader.hex":i

rem Lockbits:
.\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -U lock:w:0xEF:m

rem Reading:
rem .\avrdude -C .\avrdude.conf -p atmega32u4 -c usbasp -U lock:r:lock.hex:i

pause
