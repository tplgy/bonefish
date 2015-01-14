#include <bonefish/role_type.hpp>

#include <cassert>

namespace bonefish {

const char* to_string(const role_type& type)
{
    const char* str = nullptr;
    switch(type)
    {
        case role_type::callee:
            str = "callee";
            break;
        case role_type::caller:
            str = "caller";
            break;
        case role_type::publisher:
            str = "publisher";
            break;
        case role_type::subscriber:
            str = "subscriber";
            break;
        case role_type::dealer:
            str = "dealer";
            break;
        case role_type::broker:
            str = "broker";
            break;
        default:
            assert(0);
            str = "<<invalid>>";
            break;
    }

    return str;
}

} // namespace bonefish
