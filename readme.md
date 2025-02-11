# Title

## Running the image

### prerequisites


✅ Raspberry pi with mistral devboard connected

✅ access to the `ghcr.io/sebastianbrockl` registry

✅ pull the latest image with `docker pull ghcr.io/sebastianbrockl/mistral-demo:latest`

### Running the container

Run the container with:

```bash
docker run --rm -p 9002:9002 ghcr.io/sebastianbrockl/mistral-demo:latest
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