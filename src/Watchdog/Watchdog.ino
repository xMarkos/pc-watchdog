#include <DigiUSB.h>
#include <avr/wdt.h>

/* Blink code:
*  1 short: received a pulse
*  2 short: device is in starting mode (no reset configured)
*  3 short: device booted
*  1 sec long blinks: timed-out - device is in grace period, restart can be averted
*  300ms long blinks: restart is imminent
*/

#define VERSION "2.0"

//const int LED_PIN = LED_BUILTIN;
const int PIN_LED = 1;
const int PIN_RESET = 0;
const int BLINK_LEN_SHORT = 50;
const int BLINK_LEN_MEDIUM = 300;
const int BLINK_LEN_LONG = 1000;
const unsigned long RESET_TIMEOUT_GRACE_PERIOD = 10000;
const unsigned long RESET_TIMEOUT_PING_MULTIPLIER = 1000;

char ping_received = 0;
unsigned long last_ping = 0;
unsigned long configured_reset_timeout = 0;
int led_intensity = 1;

// USB Vendor requests
#define USBRQ_PING						1
#define USBRQ_SET_BRIGHTNESS	2

void blink(int length, int count = 1);

uchar handleVendorRequest(uchar request, usbWord_t wValue, usbWord_t wIndex, usbWord_t wLength) {
	// Uncomment for debugging received commands
	// DigiUSB.printf(F("Vendor request: request=%d, value= %d, index= %d, length= %d\n"), request);
	
	switch (request) {
		case USBRQ_PING:
			ping_received = 1;
			configured_reset_timeout = wValue.word * RESET_TIMEOUT_PING_MULTIPLIER;
			break;
		case USBRQ_SET_BRIGHTNESS:
			led_intensity = wValue.word;
			DigiUSB.printf(F("Brightness= %d\n"), led_intensity);
			break;
		default:
			DigiUSB.printf(F("Unknown: request=%d, value= %d, index= %d, length= %d\n"), request);
			break;
	}
	
	return 1;
}

void setup() {
	wdt_enable(WDTO_8S);

	pinMode(PIN_LED, OUTPUT);
	pinMode(PIN_RESET, OUTPUT);

	DigiUSB.begin();
	DigiUSB.printf(F("Booted; version= %s\n"), VERSION);

	blink(BLINK_LEN_SHORT, 3);
}

void loop() {
	wdt_reset();
	unsigned long now = millis();

	// ping_received can change anytime usbPoll() is called, i.e. when DigiUSB.refresh() or DigiUSB.delay(int) is called.
	if (ping_received) {
		ping_received = 0;
		last_ping = now;
		
		if (configured_reset_timeout == 0) {
			DigiUSB.print('0');
		} else {
			DigiUSB.print('.');
			blink(BLINK_LEN_SHORT);
		}
	}
	
	// Note: If the host computer sends data using the USBRQ_HID_SET_REPORT, it will appear here;
	// however, all commands are implemented as vendor commands inside handleVendorRequest() fn.
	//while (DigiUSB.available()) {
	//	int value = DigiUSB.read();
	//}

	if (configured_reset_timeout == 0) {
		blink(BLINK_LEN_SHORT, 2);
	} else {
		unsigned long delta = now - last_ping;

		if (delta > configured_reset_timeout + RESET_TIMEOUT_GRACE_PERIOD) {
			DigiUSB.print('#');
			blink(BLINK_LEN_MEDIUM, 7);
			trigger_reset();
		} else if (delta > configured_reset_timeout) {
			DigiUSB.print('!');
			blink(BLINK_LEN_LONG);
		}
	}

	DigiUSB.delay(1000);
}

void blink(int length, int count) {
	for (int i = 0; i < count; i++) {
		// Technically it is possible to disable the led if USBRQ_BRIGHTNESS command sets it to 0
		analogWrite(PIN_LED, led_intensity);
		DigiUSB.delay(length);
		analogWrite(PIN_LED, 0);

		if (i < count - 1)
			DigiUSB.delay(length);

		wdt_reset();
	}
}

void trigger_reset() {
	configured_reset_timeout = 0;

	for (int i = 0; i < 2; i++) {
		digitalWrite(PIN_RESET, HIGH);
		DigiUSB.delay(500);
		digitalWrite(PIN_RESET, LOW);
		DigiUSB.delay(500);
	}
}
