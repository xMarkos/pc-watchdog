# Project Settings
#SKETCH = $(notdir $(CURDIR)).ino
SKETCH = Watchdog
BUILD_DIR = build
LIB_PATH = ./libraries

# FQBN for ATTinyCore 2.x Micronucleus
FQBN = ATTinyCore:avr:attinyx5micr:LTO=enable

# Tools
CLI = arduino-cli

.PHONY: all compile upload clean

all: compile

compile:
	@echo "Compiling $(SKETCH) with extra libraries..."
	$(CLI) compile --fqbn $(FQBN) \
		--libraries $(LIB_PATH) \
		--output-dir $(BUILD_DIR) \
		--warnings all \
		$(SKETCH)

upload: compile
	@echo "Prepare to plug in Digispark..."
	$(CLI) upload --fqbn $(FQBN) --input-dir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm $(SKETCH)/*.ino.lst
