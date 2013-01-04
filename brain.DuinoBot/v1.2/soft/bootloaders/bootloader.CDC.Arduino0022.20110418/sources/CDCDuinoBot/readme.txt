Para grabar usando avrdude de la siguiente manera:

avrdude -p m32u4 -c avr109 -P \\.\COM8 -b 115200 -D -Uflash:w:./VirtualSerial.hex