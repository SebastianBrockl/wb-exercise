FROM arm32v7/debian:latest

WORKDIR /app

COPY build/bin/mistral-demo /usr/local/bin/mistral-demo

RUN chmod +x /usr/local/bin/mistral-demo

CMD ["/usr/local/bin/mistral-demo"]
