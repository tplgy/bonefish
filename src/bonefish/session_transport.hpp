#ifndef BONEFISH_SESSION_TRANSPORT_HPP
#define BONEFISH_SESSION_TRANSPORT_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/serialization/serializer.hpp>
#include <memory>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/server.hpp>

namespace bonefish {

class session_transport
{
public:
    session_transport(std::unique_ptr<serializer> s,
            const websocketpp::connection_hdl& handle,
            const std::shared_ptr<websocketpp::server<websocket_config>>& server);
    ~session_transport();
    bool send_message(const wamp_message* message);

private:
    std::unique_ptr<serializer> m_serializer;
    websocketpp::connection_hdl m_handle;
    std::shared_ptr<websocketpp::server<websocket_config>> m_server;
};

inline session_transport::session_transport(std::unique_ptr<serializer> s,
        const websocketpp::connection_hdl& handle,
        const std::shared_ptr<websocketpp::server<websocket_config>>& server)
    : m_serializer(std::move(s))
    , m_handle(handle)
    , m_server(server)
{
}

inline session_transport::~session_transport()
{
}

inline bool session_transport::send_message(const wamp_message* message)
{
    // TODO: Fix me to use a proper expandable buffer;
    char buffer[10*1024];
    size_t length = m_serializer->serialize(message, buffer, sizeof(buffer));
    if (length != 0) {
        m_server->send(m_handle, buffer, length, websocketpp::frame::opcode::BINARY);
        return true;
    }

    return false;
}

} // namespace bonefish

#endif // BONEFISH_SESSION_TRANSPORT_HPP
