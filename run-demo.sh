#!/bin/bash

# Enable verbose mode to print commands and outputs
set -x

# Define the device paths
config_device_symlink="/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if00-port0"
data_device_symlink="/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if01-port0"

# Verify if devices exist
if [[ ! -e "$config_device_symlink" || ! -e "$data_device_symlink" ]]; then
    echo "Mistral unit not found. Please check the connections."
    exit 1
fi

# Resolve the actual device paths
config_device=$(readlink -f "$config_device_symlink")
data_device=$(readlink -f "$data_device_symlink")

# Pull the latest Docker image
docker pull ghcr.io/sebastianbrockl/mistral-demo:latest

# Run the Docker container
docker run --rm -it \
  -p 9002:9002 \
  --device="$config_device" \
  --device="$data_device" \
  ghcr.io/sebastianbrockl/mistral-demo:latest \
  /usr/local/bin/mistral-demo \
  --data_port "$data_device" \
  --data_baudrate 921600 \
  --config_port "$config_device" \
  --config_baudrate 115200
