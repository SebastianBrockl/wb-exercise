# Title

## Running the image

### prerequisites


✅ Raspberry pi with mistral devboard connected

✅ access to the `ghcr.io/sebastianbrockl` registry

✅ pull the latest image with `docker pull ghcr.io/sebastianbrockl/mistral-demo:latest`

### Running the container

assuming the mistral devboards UART interfaces are mounted at

```
/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if00-port0
/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if01-port0
```

for the config and data ports respectively

Run the container with:

```bash
docker run --rm -it \
  -p 9002:9002 \
  --device=/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if00-port0 \
  --device=/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if01-port0 \
  ghcr.io/sebastianbrockl/mistral-demo:latest

docker run --rm -p 9002:9002 ghcr.io/sebastianbrockl/mistral-demo:latest
```

You can override the containers default CLI arguments like so:

```bash
docker run --rm -it \
    -v /path/to/config:/some-path/some-config.cfg \
    --device=/dev/ttyUSB0 \
    --device=/dev/ttyUSB1 \
    ghcr.io/sebastianbrockl/mistral-demo:latest \
    --config_path /some-path/some-config.cfg \
    --read_port /dev/ttyUSB0 \
    --write_port /dev/ttyUSB1 \
    --read_baudrate 115200 \
    --write_baudrate 115200
```


The container will open a websocket server on port 9002 for broadcasting TLV data

## build prerequisites

✅ Ensure you have docker with buildx installed

✅ The Docker daemon running

✅ Access to the docker registry

🛠 **Note!** Im using my local docker registry, you can override my defaults to any registry of your choise with the `env` variable `LOCAL_DOCKER_REGISTRY`, or by editing the `Makefile` directly.

```bash
sudo apt-get update && 
sudo apt-get install -y \
    g++ \
    gcc-arm-linux-gnueabihf \
    cmake \
    git \
    libboost-all-dev \
    libwebsocketpp-dev \
    libspdlog-dev \
    nlohmann-json3-dev
```