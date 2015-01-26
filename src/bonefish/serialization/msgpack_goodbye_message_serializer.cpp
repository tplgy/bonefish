#include <bonefish/serialization/msgpack_goodbye_message_serializer.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/wamp_uri.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_goodbye_message* msgpack_goodbye_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    if (fields.size() != 3) {
        throw(std::invalid_argument("invalid number of fields"));
    }

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    if (type != wamp_message_type::GOODBYE) {
        throw(std::invalid_argument("invalid message type"));
    }

    // NOTE: We currently ignore the details as they don't
    // add a whole lot of value at the present time.
    //
    //std::unordered_map<std::string, msgpack::object> details;
    //fields[1].convert(&details);

    wamp_uri reason = fields[2].as<std::string>();
    if (!is_valid_uri(reason)) {
        throw(std::invalid_argument("invalid reason uri"));
    }

    std::unique_ptr<wamp_goodbye_message> goodbye_message(new wamp_goodbye_message);
    goodbye_message->set_reason(reason);

    return goodbye_message.release();
}

size_t msgpack_goodbye_message_serializer::serialize(const wamp_goodbye_message* goodbye_message,
        char* buffer, size_t length)
{
    if (goodbye_message == nullptr) {
        throw(std::invalid_argument("null goodbye message"));
    }

    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);

    packer.pack_array(3);

    packer.pack(static_cast<unsigned>(goodbye_message->get_type()));

    //
    // Details
    //
    // NOTE: No current need for supporting details.
    packer.pack_map(0);

    packer.pack(goodbye_message->get_reason());

    if (sbuffer.size() > length) {
        throw(std::overflow_error("serialization buffer too small"));
    }

    // TODO: Fix this ugly copy. It would be nice if we could just
    //       serialize directly into the buffer.
    std::memcpy(buffer, sbuffer.data(), sbuffer.size());
    return sbuffer.size();
}

} // namespace bonefish
