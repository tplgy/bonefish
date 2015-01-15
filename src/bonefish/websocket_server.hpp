#ifndef BONEFISH_WEBSOCKET_SERVER_HPP
#define BONEFISH_WEBSOCKET_SERVER_HPP

#include <bonefish/identifier/session_id_generator.hpp>
#include <bonefish/websocket_config.hpp>
#include <memory>
#include <unordered_map>
#include <websocketpp/server.hpp>

namespace bonefish {

class realm_routers;

class websocket_server
{
public:
    websocket_server(const std::shared_ptr<realm_routers>& routers);
    ~websocket_server();

    void run();
    void shutdown();

private:
    void on_open(websocketpp::connection_hdl handle);
    void on_close(websocketpp::connection_hdl handle);
    void on_fail(websocketpp::connection_hdl handle);
    bool on_ping(websocketpp::connection_hdl handle, std::string message);
    void on_pong(websocketpp::connection_hdl handle, std::string message);
    void on_pong_timeout(websocketpp::connection_hdl handle, std::string message);
    bool on_validate(websocketpp::connection_hdl handle);
    void on_message(websocketpp::connection_hdl handle,
            websocketpp::server<websocket_config>::message_ptr message);

private:
    std::shared_ptr<websocketpp::server<websocket_config>> m_server;
    std::shared_ptr<realm_routers> m_realm_routers;
    session_id_generator m_session_id_generator;
};

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_SERVER_HPP
