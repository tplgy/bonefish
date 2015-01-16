#include <bonefish/roles/role_type.hpp>

#include <cassert>
#include <stdexcept>

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
            throw(std::invalid_argument("unknown role type"));
            break;
    }

    return str;
}

role_type from_string(const std::string& type)
{
    if (type.compare("callee")) {
        return role_type::callee;
    }

    if (type.compare("caller")) {
        return role_type::caller;
    }

    if (type.compare("publisher")) {
        return role_type::publisher;
    }

    if (type.compare("subscriber")) {
        return role_type::subscriber;
    }

    if (type.compare("dealer")) {
        return role_type::dealer;
    }

    if (type.compare("broker")) {
        return role_type::broker;
    }

    throw(std::invalid_argument("unknown role type"));
}

} // namespace bonefish
