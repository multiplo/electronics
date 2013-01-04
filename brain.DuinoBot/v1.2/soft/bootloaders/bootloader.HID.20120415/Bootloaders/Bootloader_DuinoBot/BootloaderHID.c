/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the HID class bootloader. This file contains the complete bootloader logic.
 */
 
#include "BootloaderHID.h"

#include "pinsDuinoBot.h"

/** Flag to indicate if the bootloader should be running, or should exit and allow the application code to run
 *  via a soft reset. When cleared, the bootloader will abort, the USB interface will shut down and the application
 *  started via a forced watchdog reset.
 */
static bool RunBootloader = true;

typedef void (*AppPtr_t)(void) ATTR_NO_RETURN;

// Prototypes
void SetupLedTimer(void);
void DisableLedTimer(void);

#define _COLD_START_

void Bootloader_StartUserApp(void){      
		AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
		
		#ifdef _HOT_START_
			#warning "Using hot start mode. The bootloader wont close the CDC afeter downloading the software."
			/* Set GPIOR registers to a magic key that will be read by the user application */
			GPIOR0 = 0xDC;
			GPIOR1 = 0xA6;
		#elif defined(_COLD_START_)
			#warning "Using cold start mode. The bootloader will close the CDC afeter downloading the software."
			GPIOR0 = 0x00;
			GPIOR1 = 0x00;
		#else
			#error "Not bootloader switch mode selected"
		#endif
		
		MCUCR = (1 << IVCE);
		MCUCR = 0;
		
    AppStartPtr();
}

/** Main program entry point. This routine configures the hardware required by the bootloader, then continuously 
 *  runs the bootloader processing routine until instructed to soft-exit.
 */
int main(void)
{
	
	/* Setup hardware required for the bootloader */
	SetupHardware();
	
	// Configure DDR's for switch and Yellow Led
	LED_DDR |= LED_PIN;
	RUN_SW_DDR |= RUN_SW_PIN;
	RUN_SW_PORT |= RUN_SW_PIN;
	
	// Flash led 3 times
	_delay_ms(200);
	LED_PORT |= LED_PIN;
	_delay_ms(200);
	LED_PORT &= ~LED_PIN;
	_delay_ms(200);
	LED_PORT |= LED_PIN;
	_delay_ms(200);
	LED_PORT &= ~LED_PIN;
	_delay_ms(200);
	LED_PORT |= LED_PIN;
	_delay_ms(200);
	LED_PORT &= ~LED_PIN;

	LED_PORT |= LED_PIN;
	
	/* Enable global interrupts so that the USB stack can function */
	sei();

	while (RunBootloader){
		if (!(RUN_SW_PORTIN & RUN_SW_PIN)){
			RunBootloader = false;
		}
	  USB_USBTask();
	}
	
	/* Disconnect from the host - USB interface will be reset later along with the AVR */
//	USB_Detach();
	//USB_Controller_Reset();
	//USB_ResetInterface();
	//_delay_ms(5000);
	USB_Disable();
	Bootloader_StartUserApp();
	
	/* Enable the watchdog and force a timeout to reset the AVR */
//	GPIOR0 = 0xDC;
	//GPIOR1 = 0xA6;

	//wdt_enable(WDTO_250MS);

	for (;;);
}

/** Configures all hardware required for the bootloader. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	if ((GPIOR0 == 0xDC) && (GPIOR1 == 0xA6)){
			AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
			AppStartPtr();
	}
	
	/* Relocate the interrupt vector table to the bootloader section */
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	/* Initialize USB subsystem */
	USB_Init();
}

/** Event handler for the USB_ConfigurationChanged event. This configures the device's endpoints ready
 *  to relay data to and from the attached USB host.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	/* Setup HID Report Endpoint */
	Endpoint_ConfigureEndpoint(HID_IN_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, HID_IN_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Ignore any requests that aren't directed to the HID interface */
	if ((USB_ControlRequest.bmRequestType & (CONTROL_REQTYPE_TYPE | CONTROL_REQTYPE_RECIPIENT)) !=
	    (REQTYPE_CLASS | REQREC_INTERFACE))
	{
		return;
	}

	/* Process HID specific control requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_SetReport:
			Endpoint_ClearSETUP();
			
			/* Wait until the command has been sent by the host */
			while (!(Endpoint_IsOUTReceived()));
		
			/* Read in the write destination address */
			#if (FLASHEND > 0xFFFF)
			uint32_t PageAddress = ((uint32_t)Endpoint_Read_16_LE() << 8);
			#else
			uint16_t PageAddress = Endpoint_Read_16_LE();
			#endif
			
			/* Check if the command is a program page command, or a start application command */
			#if (FLASHEND > 0xFFFF)
			if ((uint16_t)(PageAddress >> 8) == COMMAND_STARTAPPLICATION)
			#else
			if (PageAddress == COMMAND_STARTAPPLICATION)
			#endif
			{
				RunBootloader = false;
			}
			else
			{
				SetupLedTimer();
				
				/* Erase the given FLASH page, ready to be programmed */
				boot_page_erase(PageAddress);
				boot_spm_busy_wait();
				
				/* Write each of the FLASH page's bytes in sequence */
				for (uint8_t PageWord = 0; PageWord < (SPM_PAGESIZE / 2); PageWord++)				
				{
					/* Check if endpoint is empty - if so clear it and wait until ready for next packet */
					if (!(Endpoint_BytesInEndpoint()))
					{
						Endpoint_ClearOUT();
						while (!(Endpoint_IsOUTReceived()));
					}

					/* Write the next data word to the FLASH page */
					boot_page_fill(PageAddress + ((uint16_t)PageWord << 1), Endpoint_Read_16_LE());
				}

				/* Write the filled FLASH page to memory */
				boot_page_write(PageAddress);
				boot_spm_busy_wait();

				/* Re-enable RWW section */
				boot_rww_enable();
			}

			Endpoint_ClearOUT();

			Endpoint_ClearStatusStage();
			
			DisableLedTimer();
			break;
	}
}

// =================================================================================
// Functions for the programming Led
// =================================================================================

void SetupLedTimer(void){

 	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);

	// set timer 0 prescale factor to 64	
	sbi(TCCR0B, CS02);
	cbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
	
	// enable timer 0 overflow interrupt
	sbi(TIMSK0, TOIE0);
	
	sei();
 }
 
void DisableLedTimer(void){
	cbi(TCCR0B, CS02);
	cbi(TCCR0B, CS01);
	cbi(TCCR0B, CS00);
	
	cli();
} 

SIGNAL(TIMER0_OVF_vect)
{
	static int ovfcount=0;
	if ((ovfcount++)==3){
		LED_PORT ^= LED_PIN;
		ovfcount = 0;
	}
}