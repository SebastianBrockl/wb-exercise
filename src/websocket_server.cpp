#include "websocket_server.h"

WebSocketServer::WebSocketServer() {
    m_server.init_asio();
    m_server.set_message_handler(std::bind(&WebSocketServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

void WebSocketServer::add_connection(websocketpp::connection_hdl hdl) {
    m_open_connections.push_back(hdl);
}

void WebSocketServer::remove_connection(websocketpp::connection_hdl hdl) {
    m_open_connections.erase(std::remove(m_open_connections.begin(), m_open_connections.end(), hdl), m_open_connections.end());
}

void WebSocketServer::broadcast_message(std::shared_ptr<std::string> message) {
    for (auto hdl : m_open_connections) {
        m_server.send(hdl, message->c_str(), websocketpp::frame::opcode::text);
    }
}

void WebSocketServer::on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg) {
    std::cout << "Received incoming message: " << msg->get_payload() << std::endl;
}

void WebSocketServer::on_open(websocketpp::connection_hdl hdl) {
    add_connection(hdl);
}

void WebSocketServer::on_close(websocketpp::connection_hdl hdl) {
    remove_connection(hdl);
}

/**
 * Start the server on the specified port.
 */
void WebSocketServer::run(uint16_t port) {
    m_server.listen(port);
    m_server.start_accept();
    m_server.run();
}
