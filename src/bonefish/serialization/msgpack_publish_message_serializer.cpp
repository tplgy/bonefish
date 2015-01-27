#include <bonefish/serialization/msgpack_publish_message_serializer.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/wamp_uri.hpp>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_publish_message* msgpack_publish_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    if (fields.size() < 4 || fields.size() > 6) {
        throw(std::invalid_argument("invalid number of fields"));
    }

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    if (type != wamp_message_type::PUBLISH) {
        throw(std::invalid_argument("invalid message type"));
    }

    std::cerr << "RequestId: " << fields[1] << std::endl;
    wamp_request_id request_id(fields[1].as<uint64_t>());

    // TODO: process options (fields[2])

    wamp_uri topic = fields[3].as<std::string>();
    if (!is_valid_uri(topic)) {
        throw(std::invalid_argument("invalid topic uri"));
    }

    // TODO: process optional arguments list (fields(4))

    // TODO: process optional arguments kw dict (fields(5))

    std::unique_ptr<wamp_publish_message> publish_message(new wamp_publish_message);
    publish_message->set_request_id(request_id);
    publish_message->set_topic(topic);

    return publish_message.release();
}

size_t msgpack_publish_message_serializer::serialize(const wamp_publish_message* publish_message,
        char* buffer, size_t length)
{
    throw(std::logic_error("serialize is not implemented"));
}

} // namespace bonefish
