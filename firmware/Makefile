# Project Settings
SKETCH = Watchdog
BUILD_DIR = build
LIB_PATH = ./libraries

# FQBN for ATTinyCore 2.x Micronucleus
FQBN = ATTinyCore:avr:attinyx5micr:LTO=enable

# To install the core
BOARD_URL = http://drazzy.com/package_drazzy.com_index.json
CORE_ID = ATTinyCore:avr

# Tools
CLI = arduino-cli

.PHONY: all compile upload clean install-board

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

install-board:
	$(CLI) config add board_manager.additional_urls $(BOARD_URL)
	$(CLI) core update-index
	$(CLI) core install $(CORE_ID)
