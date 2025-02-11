#include "websocket_server.h"
#include "util.h"

int main() {

    // load configuration file into shared pointer
    std::shared_ptr<std::string> config = std::make_shared<std::string>();
    load_config(config, "/app/default-config.cfg");
    std::cout << "Config loaded" << std::endl;
    // TODO: this should be logged only with debug profile
    std::cout << *config << std::endl;


    // Setup websocket server
    std::cout << "Starting WebSocket server" << std::endl;
    WebSocketServer ws_server;
    ws_server.run(9002); // Run WebSocket server on port 9002
    return 0;
}