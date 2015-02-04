#include <bonefish/messages/wamp_welcome_details.hpp>
#include <iostream>

namespace bonefish {

msgpack::object wamp_welcome_details::marshal(msgpack::zone* zone)
{
    if (zone) {
        return msgpack::object(*this, zone);
    }

    return msgpack::object(*this, &m_zone);
}

void wamp_welcome_details::unmarshal(const msgpack::object& object)
{
    throw std::logic_error("unmarshal not implemented");
}

} // namespace bonefish

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {

inline object const& operator>> (msgpack::object const&, bonefish::wamp_welcome_details&)
{
    throw std::logic_error("no msgpack object deserializer defined");
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>&, bonefish::wamp_welcome_details const&)
{
    throw std::logic_error("no msgpack object packer serializer defined");
}

inline void operator<< (object::with_zone& details,
        bonefish::wamp_welcome_details const& welcome_details)
{
    object::with_zone roles(details.zone);
    roles.type = type::MAP;
    roles.via.map.size = welcome_details.get_roles().size();
    roles.via.map.ptr = static_cast<object_kv*>(
            roles.zone.allocate_align(sizeof(object_kv) * roles.via.map.size));

    unsigned index = 0;
    for (const auto& role : welcome_details.get_roles()) {
        const std::string role_name(role_type_to_string(role.get_type()));
        roles.via.map.ptr[index].key = object(role_name, roles.zone);
        roles.via.map.ptr[index].val = object(role.get_features().get_options(), roles.zone);
        ++index;
    }

    details.type = type::MAP;
    details.via.map.size = 1;
    details.via.map.ptr = static_cast<object_kv*>(
            details.zone.allocate_align(sizeof(object_kv) * details.via.map.size));
    details.via.map.ptr[0].key = object(std::string("roles"), details.zone);
    details.via.map.ptr[0].val = *(static_cast<object*>(&roles));
}

} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack
