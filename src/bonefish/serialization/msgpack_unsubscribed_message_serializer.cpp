#include <bonefish/serialization/msgpack_unsubscribed_message_serializer.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_unsubscribed_message.hpp>
#include <bonefish/wamp_uri.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_unsubscribed_message* msgpack_unsubscribed_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    throw(std::logic_error("serialize is not implemented"));
}

size_t msgpack_unsubscribed_message_serializer::serialize(const wamp_unsubscribed_message* unsubscribed_message,
        char* buffer, size_t length)
{
    if (unsubscribed_message == nullptr) {
        throw(std::invalid_argument("null message"));
    }

    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);

    packer.pack_array(2);

    packer.pack(static_cast<unsigned>(unsubscribed_message->get_type()));
    packer.pack(unsubscribed_message->get_request_id().id());

    if (sbuffer.size() > length) {
        throw(std::overflow_error("serialization buffer too small"));
    }

    // TODO: Fix this ugly copy. It would be nice if we could just
    //       serialize directly into the buffer.
    std::memcpy(buffer, sbuffer.data(), sbuffer.size());
    return sbuffer.size();
}

} // namespace bonefish
