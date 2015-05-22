#include <bonefish/rawsocket/rawsocket_transport.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>
#include <bonefish/serialization/expandable_buffer.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/trace/trace.hpp>

#include <iostream>

namespace bonefish {

rawsocket_transport::rawsocket_transport(
        const std::shared_ptr<wamp_serializer>& serializer,
        const std::shared_ptr<rawsocket_connection>& connection)
    : m_serializer(serializer)
    , m_connection(connection)
{
}

bool rawsocket_transport::send_message(const wamp_message* message)
{
    BONEFISH_TRACE("sending message: %1%", message_type_to_string(message->get_type()));
    expandable_buffer buffer = m_serializer->serialize(message);
    m_connection->send_message(buffer.data(), buffer.size());

    return true;
}

} // namespace bonefish
