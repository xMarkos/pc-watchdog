# Watchdog USB Device

The Watchdog USB Device is a compact hardware utility designed to automatically restart a PC in the event of a system freeze. It is engineered to plug directly into a USB header on the computer's motherboard.

## How It Works

When connected to a USB port, the device (the **Watchdog**) listens for periodic pulses (**kicks**) sent by a software service (the **Pulser**) running on the PC. It triggers a hardware reset if it fails to receive a kick within a specified timeframe.

### The Process

1. **Initialization:** When the PC boots, the Pulser identifies the connected Watchdog and begins sending pulses at regular intervals.
2. **Configuration:** Each pulse configures the Watchdog's timeout duration (the maximum allowed time between pulses).
3. **Monitoring:** If the Watchdog does not receive a pulse within the timeout period, it enters a **grace period** (defaulting to 10 seconds).
4. **Recovery:** If no pulse is received before the grace period expires, the Watchdog triggers a hardware restart by sending a signal to the motherboard's reset pin. It then resets and waits for a new connection from the Pulser.

### Timeout Parameters

* **Range:** The timeout is fully controlled by the Pulser and can be set between 1 and 65,535 seconds (approx. 18 hours).
* **Deactivation:** A special value of `0` disables the Watchdog functionality.

### Status LED

The onboard LED provides real-time feedback on the device's current state:

| Pattern        | Frequency   | Meaning                                         |
| :------------- | :---------- | :---------------------------------------------- |
| 3 short blinks | Once        | Device booted successfully                      |
| 2 short blinks | Each second | Stand-by mode (indefinite timeout)              |
| 1 short blink  | Once        | Pulse received                                  |
| Long blinks    | Each second | Device in grace period                          |
| Fast blinking  | 2 seconds   | **[Full brightness]** Triggering system restart |

---

## Hardware

The heart of the device is an **AVR ATtiny85 MCU**. It connects physically to a motherboard USB header and the system's reset pin.

The circuit is inspired by the **Digispark (Digistump)**. While the firmware remains compatible with Digispark hardware, this device is a custom implementation rather than a direct clone.

### Form Factor

* Designed to fit on a **10x10 stripboard**.
* A **4-pin connector** is soldered to the board for direct connection to the USB header.
* A separate cable with a **1-pin female DuPont connector** attaches to the motherboard's reset pin.

![PCB schematic](hardware/pcb.png)

---

## Firmware

The firmware is written in **C++** using the Arduino framework. This project utilizes **ATtinyCore**, though it is likely compatible with most other Arduino cores for the ATtiny85.

### USB Communication

The `libraries/DigisparkUSB` directory contains the communication library derived from the original Digispark core. It has been modified to register as a **Vendor Class device** rather than a HID (Human Interface Device). This change allows for the implementation of the `handleVendorRequest` function to process custom vendor requests.

### Flashing and Fuses

Since the USB wiring is identical to a Digispark, **Micronucleus** (tested with version 2.6) can be used to flash the firmware. 

**Note:** Burning the initial bootloader is outside the scope of this manual. However, the chip's fuses **must** be configured to run at **16.5 MHz** for USB communication to function correctly.

Example command to burn the bootloader and set fuses using an **Arduino as ISP**:

```bash
avrdude -c arduino -p t85 -P /dev/ttyACM0 -b 19200 \
  -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m \
  -U flash:w:/path/to/micronucleus-2.6.hex:i
```

While it may be possible to run the firmware without a bootloader, this configuration has not been tested.
