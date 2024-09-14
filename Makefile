
MAKEFLAGS += --silent

HOME_DIR = $(HOME)
LOC_FILE = $(HOME_DIR)/.loc.json

define ReadLoc
$(shell node -p "require('$(LOC_FILE)').$(1)")
endef

APP = stampbc

BUILD_DIR = build
BUILD_DEV_DIR = $(BUILD_DIR)/dev
LIB_DIR = library
CONFIG_DIR = config

.PHONY: build
build: clean
	echo "building release ..."
	cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
	cmake --build build

.PHONY: dev
dev:
	echo "building dev"
	cmake --preset=dev
	cmake --build --preset=dev

.PHONY: run
run:
	echo "running $(APP)...\n"
	./$(BUILD_DEV_DIR)/$(APP)

.PHONY: test
test: 
	echo "running test"
	$(BUILD_DEV_DIR)/test/$(APP)_test

.PHONY: clean
clean:
	echo "cleaning build dir ..."
	rm -rf $(BUILD_DIR)

# Initialize the project
.PHONY: init
init:
	echo "initializing project..."
	mkdir -p $(CONFIG_DIR)
	mkdir -p $(LIB_DIR)
	echo "done!"

# if install or uninstall is called, check if the directories exist
ifneq ($(filter install uninstall, $(MAKECMDGOALS)),)
$(if $(call ReadLoc, bin),,$(error ensure there is a .loc.json file in the home directory. if not run `make loc`))
endif

# PATH directories, where system wide binaries are stored
PATH_BIN_DIR = $(call ReadLoc,bin)
P_PATH_LIB_DIR = $(call ReadLoc,library)/$(APP)
P_PATH_CONFIG_DIR = $(call ReadLoc,config)/$(APP)

# Move all binaries to respective directories
.PHONY: install
install: uninstall
	echo "installing library..."
	mkdir -p $(P_PATH_LIB_DIR)
	cp -r $(LIB_DIR)/* $(P_PATH_LIB_DIR) 2>/dev/null || :
	echo "installing binaries..."
	mkdir -p $(PATH_BIN_DIR)
	cp -r $(BUILD_DIR)/$(APP) $(PATH_BIN_DIR) 2>/dev/null || :
	echo "installing config files..."
	mkdir -p $(P_PATH_CONFIG_DIR)
	cp -r $(CONFIG_DIR)/* $(P_PATH_CONFIG_DIR) 2>/dev/null || :
	echo "removing unnessesary path directories..."
	rmdir $(P_PATH_LIB_DIR) 2>/dev/null || :
	rmdir $(P_PATH_CONFIG_DIR) 2>/dev/null || :
	echo "done!"

# Remove all project files from respective sys-wide directories
.PHONY: uninstall
uninstall:
	echo "uninstalling library..."
	rm -rf $(P_PATH_LIB_DIR)
	echo "uninstalling config files..."
	rm -rf $(P_PATH_CONFIG_DIR)
	echo "uninstalling binary (if exists)"
	rm -f $(PATH_BIN_DIR)/$(APP)
	echo "Done!"

.PHONY: help
help:
	echo "Available targets:"
	echo "  build - Build the release version"
	echo "  dev   - Build the development version"
	echo "  test  - Run tests"
	echo "  clean - Clean the build directory"
	echo "  init  - Create supplemental development directories"
	echo "  run   - Run the application"
	echo "  install - Install the application"
	echo "  uninstall - Uninstall the application"
	echo "  help  - Display this help message"

TARGET_LIB = $(HOME)/dev/.library
TARGET_CONF = $(HOME)/dev/.config 
TARGET_BIN = $(HOME)/dev/.bin 
TARGET_SCRIPTS = $(HOME)/dev/.scripts 
TARGET_WORK = $(HOME)/dev/workspace

.PHONY: loc
loc:
	echo "downloading loc-maker..."
	curl -L -O https://github.com/SPoovathumkadavil/loc-maker/releases/download/release/loc-maker
	chmod +x loc-maker
	echo "creating .loc.json file..."
	./loc-maker -n library=$(TARGET_LIB) bin=$(TARGET_BIN) config=$(TARGET_CONF) scripts=$(TARGET_SCRIPTS) workspace=$(TARGET_WORK)
	echo "cleaning up..."
	rm loc-maker
	echo "done."
