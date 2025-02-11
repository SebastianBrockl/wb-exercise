#include "websocket_server.h"
#include "util.h"
#include "RadarConfig.h"

int main() {
    // load configuration  file as RadarConfig object
    RadarConfig radarConfig;
    radarConfig.loadFromFile("/app/default-config.cfg");
    std::cout << "RadarConfig loaded" << std::endl;
    // TODO: this should be loggeed only in debug profile
    std::cout << radarConfig.toString() << std::endl;
    std::cout << radarConfig.to_json_pretty() << std::endl;


    // Setup websocket server
    std::cout << "Starting WebSocket server" << std::endl;
    WebSocketServer ws_server;
    ws_server.run(9002); // Run WebSocket server on port 9002
    return 0;
}