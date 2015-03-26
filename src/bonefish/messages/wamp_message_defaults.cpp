#include <bonefish/messages/wamp_message_defaults.hpp>

#include <map>
#include <msgpack.hpp>

namespace bonefish {

namespace {
msgpack::zone zone;
const std::map<int,int> EMPTY_MAP;
const msgpack::object MSGPACK_EMPTY_MAP(EMPTY_MAP, &zone);
} // namespace

const msgpack::object& msgpack_empty_map()
{
    return MSGPACK_EMPTY_MAP;
}


} // namespace bonefish
