#ifndef BONEFISH_WAMP_MESSAGE_HPP
#define BONEFISH_WAMP_MESSAGE_HPP

#include <msgpack.hpp>
#include <vector>

namespace bonefish {

using wamp_message = std::vector<msgpack::object>;

} // namespace bonefish

#endif // BONEFISH_WAMP_MESSAGE_HPP
