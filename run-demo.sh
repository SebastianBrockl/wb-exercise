#!/bin/bash

# Enable verbose mode to print commands and outputs
set -x

# Define the device paths
device1="/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if00-port0"
device2="/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if01-port0"

# Verify if devices exist
if [[ ! -e "$device1" || ! -e "$device2" ]]; then
    echo "Mistral unit not found. Please check the connections."
    exit 1
fi

# Pull the latest Docker image
docker pull ghcr.io/sebastianbrockl/mistral-demo:latest

# Run the Docker container
docker run --rm -it \
  -p 9002:9002 \
  --device="$device1" \
  --device="$device2" \
  ghcr.io/sebastianbrockl/mistral-demo:latest
