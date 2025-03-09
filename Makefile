BUILD_DIR=build
NATIVE_BUILD_DIR=build-native

# Cross-compile main application
build:
	mkdir -p $(BUILD_DIR)/bin
	# cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm.cmake
	cd ..
	cmake --build $(BUILD_DIR) --target docker-build

# Push Docker Image
push:
	mkdir -p $(BUILD_DIR)/bin
	cd $(BUILD_DIR) && cmake ..
	# cd $(BUILD_DIR) && cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-arm.cmake
	cd ..
	cmake --build $(BUILD_DIR) --target docker-push

# Run Tests (builds natively)
test:
	rm -rf $(NATIVE_BUILD_DIR)  # Ensure a clean native build
	mkdir -p $(NATIVE_BUILD_DIR)/bin
	cd $(NATIVE_BUILD_DIR) && cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-native.cmake
	cd ..
	cmake --build $(NATIVE_BUILD_DIR) --target util_tests
	cd $(NATIVE_BUILD_DIR) && ctest --output-on-failure

# Clean both builds
clean:
	rm -rf $(BUILD_DIR) $(NATIVE_BUILD_DIR)

# Phony targets
.PHONY: build push clean test
