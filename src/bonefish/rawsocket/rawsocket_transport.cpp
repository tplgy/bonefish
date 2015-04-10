#include <bonefish/rawsocket/rawsocket_transport.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>

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
    // TODO: Fix me to use a proper expandable buffer.
    char buffer[10*1024];
    size_t length = m_serializer->serialize(message, buffer, sizeof(buffer));
    if (length == 0) {
        std::cerr << "failed sending message: " << message_type_to_string(message->get_type()) << std::endl;
        return false;
    }

    std::cerr << "sending message: " << message_type_to_string(message->get_type()) << std::endl;
    m_connection->send_message(buffer, length);

    return true;
}

} // namespace bonefish
