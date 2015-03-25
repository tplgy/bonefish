#ifndef BONEFISH_WEBSOCKET_TRANSPORT_HPP
#define BONEFISH_WEBSOCKET_TRANSPORT_HPP

#include <bonefish/transport/wamp_transport.hpp>
#include <bonefish/websocket/websocket_config.hpp>

#include <memory>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/server.hpp>

namespace bonefish {

class wamp_message;
class wamp_serializer;

class websocket_transport : public wamp_transport
{
public:
    websocket_transport(const std::shared_ptr<wamp_serializer>& serializer,
            const websocketpp::connection_hdl& handle,
            const std::shared_ptr<websocketpp::server<websocket_config>>& server);

    virtual bool send_message(const wamp_message* message) override;

private:
    std::shared_ptr<wamp_serializer> m_serializer;
    websocketpp::connection_hdl m_handle;
    std::shared_ptr<websocketpp::server<websocket_config>> m_server;
};

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_TRANSPORT_HPP
