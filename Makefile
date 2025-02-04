
BUILD_DIR=build

# CMake Build
build:
	mkdir -p $(BUILD_DIR)/bin
	cd $(BUILD_DIR) && cmake .. 
	cd ..
	cmake --build $(BUILD_DIR) --target docker-build

# CMake Build
push:
	mkdir -p $(BUILD_DIR)/bin
	cd $(BUILD_DIR) && cmake .. 
	cd ..
	cmake --build $(BUILD_DIR) --target docker-push

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets (tells make that they don't produce files)
.PHONY: build push clean