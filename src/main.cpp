#include "websocket_server.h"

int main() {
    WebSocketServer ws_server;
    ws_server.run(9002); // Run WebSocket server on port 9002
    return 0;
}