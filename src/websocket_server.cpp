#include "websocket_server.h"

WebSocketServer::WebSocketServer() {
    m_server.init_asio();
    m_server.set_message_handler(std::bind(&WebSocketServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

void WebSocketServer::on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg) {
    std::cout << "Received incoming message: " << msg->get_payload() << std::endl;
}

void WebSocketServer::on_open(websocketpp::connection_hdl hdl) {
    auto connection = m_server.get_con_from_hdl(hdl);
    std::cout << "New connection from " << connection->get_remote_endpoint() << std::endl;
    m_connections.push_back(connection);
}

/**
 * Start the server on the specified port.
 */
void WebSocketServer::run(uint16_t port) {
    m_server.listen(port);
    m_server.start_accept();
    m_server.run();
}
