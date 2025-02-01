#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

typedef websocketpp::server<websocketpp::config::asio> server;

class WebSocketServer {
public:
    WebSocketServer() {
        ws_server.init_asio();
        ws_server.set_open_handler(bind(&WebSocketServer::on_open, this, ::_1));
        ws_server.set_message_handler(bind(&WebSocketServer::on_message, this, ::_1, ::_2));
    }

    void run(uint16_t port) {
        ws_server.listen(port);
        ws_server.start_accept();
        ws_server.run();
    }

private:
    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "New WebSocket connection!" << std::endl;
    }

    void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::cout << "Received message: " << msg->get_payload() << std::endl;
    }

    server ws_server;
};
