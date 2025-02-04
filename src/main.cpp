#include "websocket_server.h"

int main() {
    std::cout << "Starting WebSocket server" << std::endl;
    WebSocketServer ws_server;
    ws_server.run(9002); // Run WebSocket server on port 9002
    return 0;
}