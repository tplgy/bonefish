#ifndef BONEFISH_ROLE_TYPE_HPP
#define BONEFISH_ROLE_TYPE_HPP

#include <cstdint>

namespace bonefish {

enum class role_type : uint16_t
{
    callee,
    caller,
    publisher,
    subscriber,
    dealer,
    broker
};

const char* to_string(const role& r);

} // namespace bonefish

#endif // BONEFISH_ROLE_TYPE_HPP
