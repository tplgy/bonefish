#include <bonefish/roles/wamp_role_type.hpp>

#include <cassert>
#include <stdexcept>

namespace bonefish {

const char* role_type_to_string(const wamp_role_type& type)
{
    const char* str = nullptr;
    switch(type)
    {
        case wamp_role_type::CALLEE:
            str = "callee";
            break;
        case wamp_role_type::CALLER:
            str = "caller";
            break;
        case wamp_role_type::PUBLISHER:
            str = "publisher";
            break;
        case wamp_role_type::SUBSCRIBER:
            str = "subscriber";
            break;
        case wamp_role_type::DEALER:
            str = "dealer";
            break;
        case wamp_role_type::BROKER:
            str = "broker";
            break;
        default:
            throw(std::invalid_argument("unknown role type"));
            break;
    }

    return str;
}

wamp_role_type role_type_from_string(const std::string& type)
{
    if (type.compare("callee")) {
        return wamp_role_type::CALLEE;
    }

    if (type.compare("caller")) {
        return wamp_role_type::CALLER;
    }

    if (type.compare("publisher")) {
        return wamp_role_type::PUBLISHER;
    }

    if (type.compare("subscriber")) {
        return wamp_role_type::SUBSCRIBER;
    }

    if (type.compare("dealer")) {
        return wamp_role_type::DEALER;
    }

    if (type.compare("broker")) {
        return wamp_role_type::BROKER;
    }

    throw(std::invalid_argument("unknown role type"));
}

} // namespace bonefish
