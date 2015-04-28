#include <bonefish/websocket/websocket_transport.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/trace/trace.hpp>

#include <iostream>

namespace bonefish {

websocket_transport::websocket_transport(const std::shared_ptr<wamp_serializer>& serializer,
        const websocketpp::connection_hdl& handle,
        const std::shared_ptr<websocketpp::server<websocket_config>>& server)
    : m_serializer(serializer)
    , m_handle(handle)
    , m_server(server)
{
}

bool websocket_transport::send_message(const wamp_message* message)
{
    // TODO: Fix me to use a proper expandable buffer.
    char buffer[10*1024];
    size_t length = m_serializer->serialize(message, buffer, sizeof(buffer));
    if (length != 0) {
        BONEFISH_TRACE("sending message: %1%", message_type_to_string(message->get_type()));
        m_server->send(m_handle, buffer, length, websocketpp::frame::opcode::BINARY);
        return true;
    }

    BONEFISH_TRACE("failed sending message: %1%", message_type_to_string(message->get_type()));
    return false;
}

} // namespace bonefish
