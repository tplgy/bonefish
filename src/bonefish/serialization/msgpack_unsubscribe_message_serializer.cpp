#include <bonefish/serialization/msgpack_unsubscribe_message_serializer.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/wamp_uri.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_unsubscribe_message* msgpack_unsubscribe_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    if (fields.size() != 3) {
        throw(std::invalid_argument("invalid number of fields"));
    }

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    if (type != wamp_message_type::UNSUBSCRIBE) {
        throw(std::invalid_argument("invalid message type"));
    }

    wamp_request_id request_id(fields[1].as<unsigned>());
    wamp_subscription_id subscription_id(fields[2].as<unsigned>());

    std::unique_ptr<wamp_unsubscribe_message> unsubscribe_message(new wamp_unsubscribe_message);
    unsubscribe_message->set_request_id(request_id);
    unsubscribe_message->set_subscription_id(subscription_id);

    return unsubscribe_message.release();
}

size_t msgpack_unsubscribe_message_serializer::serialize(const wamp_unsubscribe_message* unsubscribe_message,
        char* buffer, size_t length)
{
    throw(std::logic_error("serialize is not implemented"));
}

} // namespace bonefish
