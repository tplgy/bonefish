#include <bonefish/websocket/websocket_transport.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/serialization/expandable_buffer.hpp>
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
    BONEFISH_TRACE("sending message: %1%", message_type_to_string(message->get_type()));
    expandable_buffer buffer = m_serializer->serialize(message);
    m_server->send(m_handle, buffer.data(), buffer.size(), websocketpp::frame::opcode::BINARY);

    return true;
}

} // namespace bonefish
