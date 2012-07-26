#include "board.h"
#include <aery32/all.h>

#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include "Descriptors.h"

using namespace aery;

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		{ /* Config */
			0, /* ControlInterfaceNumber */
			{ /* DataINEndpoint */
				CDC_TX_EPADDR, /* Address */
				CDC_TXRX_EPSIZE, /* Size */
				1, /* Banks */
			},
			{ /* DataOUTEndpoint */
				CDC_RX_EPADDR, /* Address */
				CDC_TXRX_EPSIZE, /* Size */
				1, /* Banks */
			},
			{ /* NotificationEndpoint */
				CDC_NOTIFICATION_EPADDR, /* Address */
				CDC_NOTIFICATION_EPSIZE, /* Size */
				1, /* Banks */
			},
		},
	};

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);	
}

int main(void)
{
	init_board();
	gpio_init_pin(LED, GPIO_OUTPUT);

	/* Set up the USB generic clock. That's f_pll1 / 2 == 48MHz. */
	pm_init_gclk(GCLK_USBB, GCLK_SOURCE_PLL1, 1);
	pm_enable_gclk(GCLK_USBB);

	/* Register USB isr handler, from LUFA library */
	INTC_Init();
	INTC_RegisterGroupHandler(INTC_IRQ_GROUP(AVR32_USBB_IRQ), 1, &USB_GEN_vect);
	GlobalInterruptEnable();

	/* Initiliaze the USB, LUFA magic */
	USB_Init();

	/* All init done, turn the LED on */
	gpio_set_pin_high(LED);

	for(;;) {
		CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 'a');
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}

	return 0;
}
