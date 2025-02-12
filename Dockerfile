FROM arm32v7/debian:latest

WORKDIR /app

COPY build/bin/mistral-demo /usr/local/bin/mistral-demo
COPY default-config.cfg /app/default-config.cfg

RUN chmod +x /usr/local/bin/mistral-demo

CMD ["/usr/local/bin/mistral-demo", "--data_port", "/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if01-port0 ", "--config_port", "/dev/serial/by-id/usb-Silicon_Labs_CP2105_Dual_USB_to_UART_Bridge_Controller_01448E45-if00-port0 ", "--data_baudrate", "921600", "--config_baudrate", "115200"]
