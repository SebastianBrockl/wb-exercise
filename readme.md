# Title

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
    libwebsocketpp-dev
```