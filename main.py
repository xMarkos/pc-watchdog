import usb.core
import usb.util
import time

VID = 0x16c0
PID = 0x05df

REQUEST_TYPE_SEND = usb.util.build_request_type(usb.util.CTRL_OUT, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_DEVICE)
REQUEST_TYPE_RECEIVE = usb.util.build_request_type(usb.util.CTRL_IN, usb.util.CTRL_TYPE_CLASS, usb.util.CTRL_RECIPIENT_DEVICE)

USBRQ_HID_GET_REPORT = 0x01
USBRQ_HID_SET_REPORT = 0x09
USB_HID_REPORT_TYPE_FEATURE = 0x03

PULSE_FREQUENCY = 10
PULSE_TIMEOUT = 3    # Multiples of 5 seconds


def write(data: str):
	assert isinstance(device, usb.core.Device)

	for c in data:
		write_byte(ord(c))


def write_byte(data: int):
	assert isinstance(device, usb.core.Device)
	value = data & 0xFF
	assert value == data, "Data out of range"

	device.ctrl_transfer(bmRequestType=REQUEST_TYPE_SEND,
	                     bRequest=USBRQ_HID_SET_REPORT,
	                     wValue=(USB_HID_REPORT_TYPE_FEATURE << 8) | 0,
	                     wIndex=value,
	                     data_or_wLength=[])

	time.sleep(0.01)


def read(count=50):
	assert isinstance(device, usb.core.Device)

	text = ''

	for i in range(count):
		result = device.ctrl_transfer(
		    bmRequestType=REQUEST_TYPE_RECEIVE,
		    bRequest=USBRQ_HID_GET_REPORT,
		#wIndex=0,
		    data_or_wLength=1)

		if (not len(result)):
			break

		text += chr(result[0])

	return text


try:
	while True:
		device = usb.core.find(idVendor=VID, idProduct=PID)
		print(f'{device=!s}')

		try:
			assert isinstance(device, usb.core.Device), "Device not found"

			last_pulse = 0
			last_minutes = ''

			while True:
				if len(value := read()):
					#print(f'Reading: {value}')
					print(value, end='', flush=True)

				now = time.time()
				now_struct = time.localtime()
				now_minutes = now_struct.tm_min // 5 * 5

				if now_minutes != last_minutes:
					last_minutes = now_minutes
					print(f'\n[{time.strftime("%H:%M", now_struct)}] ', end='', flush=True)

				if now - last_pulse > PULSE_FREQUENCY:
					last_pulse = now
					write_byte(PULSE_TIMEOUT)

				time.sleep(0.1)
		except (AssertionError, usb.core.USBError) as e:
			print(f'Error: {e}')
			time.sleep(5)
except KeyboardInterrupt:
	# Important: this does not handle SIGKILL, so we need to make sure that systemd terminates the program with SIGINT
	write_byte(0)
finally:
	time.sleep(1)
	if len(value := read()):
		print(value, flush=True)
