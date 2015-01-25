#include <bonefish/serialization/msgpack_subscribe_message_serializer.hpp>
#include <bonefish/dentifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/wamp_uri.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_subscribe_message* msgpack_subscribe_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    if (fields.size() != 4) {
        throw(std::invalid_argument("subscribe message serializer - invalid number of fields"));
    }

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    if (type != wamp_message_type::GOODBYE) {
        throw(std::invalid_argument("subscribe message serializer - invalid message type"));
    }

    wamp_request_id request_id(fields[1].as<unsigned>());

    // NOTE: We currently ignore the details as they don't
    // add a whole lot of value at the present time.
    //
    //std::unordered_map<std::string, msgpack::object> details;
    //fields[1].convert(&details);

    wamp_uri topic = fields[2].as<std::string>();
    if (!is_valid_uri(reason)) {
        throw(std::invalid_argument("subscribe message serializer - invalid reason uri"));
    }

    std::unique_ptr<wamp_subscribe_message> subscribe_message(new wamp_subscribe_message);
    subscribe_message->set_request_id(request_id);
    subscribe_message->set_topic(topic);

    return subscribe_message.release();
}

size_t msgpack_subscribe_message_serializer::serialize(const wamp_subscribe_message* subscribe_message,
        char* buffer, size_t length)
{
    throw(std::logic_error("hello message serializer - serialize is not implemented"));
}

} // namespace bonefish
