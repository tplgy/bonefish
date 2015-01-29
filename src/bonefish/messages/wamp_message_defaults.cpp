#include <bonefish/messages/wamp_message_defaults.hpp>

namespace bonefish {

namespace {
msgpack::zone zone;
const std::map<int,int> empty_map;
}

const msgpack::object MSGPACK_EMPTY_MAP(empty_map, &zone);

} // namespace bonefish
