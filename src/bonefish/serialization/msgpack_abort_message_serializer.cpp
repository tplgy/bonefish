#include <bonefish/serialization/msgpack_abort_message_serializer.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/wamp_uri.hpp>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_abort_message* msgpack_abort_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    throw(std::logic_error("deserialize is not implemented"));
}

size_t msgpack_abort_message_serializer::serialize(const wamp_abort_message* abort_message,
        char* buffer, size_t length)
{
    if (abort_message == nullptr) {
        throw(std::invalid_argument("null abort message"));
    }

    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);

    packer.pack_array(3);

    packer.pack(static_cast<unsigned>(abort_message->get_type()));

    // NOTE: We currently ignore the details as they don't
    // add a whole lot of value at the present time.
    packer.pack_map(0);

    packer.pack(abort_message->get_reason());

    if (sbuffer.size() > length) {
        throw(std::overflow_error("serialization buffer too small"));
    }

    // TODO: Fix this ugly copy. It would be nice if we could just
    //       serialize directly into the buffer.
    std::memcpy(buffer, sbuffer.data(), sbuffer.size());
    return sbuffer.size();
}

} // namespace bonefish
