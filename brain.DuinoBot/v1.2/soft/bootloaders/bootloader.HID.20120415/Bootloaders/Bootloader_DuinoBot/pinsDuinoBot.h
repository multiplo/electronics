#ifndef _PINS_DUINOBOT__H
#define _PINS_DUINOBOT__H

#define RUN_SW_PIN			_BV(2)
#define RUN_SW_DDR			DDRE
#define RUN_SW_PORT			PORTE
#define RUN_SW_PORTIN		PINE

#define LED_PIN 				_BV(1)
#define LED_PORT 				PORTB
#define LED_DDR 				DDRB

#define GREEN_LED_PIN 	_BV(5)
#define GREEN_LED_PORT 	PORTD
#define GREEN_LED_DDR 	DDRD

#define RED_LED_PIN 		_BV(6)
#define RED_LED_PORT 		PORTD
#define RED_LED_DDR 		DDRD

#define MOT_B_PWM_PIN		_BV(6)
#define MOT_B_PWM_PORT	PORTB
#define MOT_B_PWM_DDR		DDRB	

#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#endif