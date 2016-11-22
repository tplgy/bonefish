/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <bonefish/messages/wamp_welcome_details.hpp>

#include <msgpack/object.hpp>
#include <msgpack/pack.hpp>
#include <stdexcept>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {

object const& operator>> (msgpack::object const&, bonefish::wamp_welcome_details&)
{
    throw std::logic_error("no msgpack object deserializer defined");
}

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>&, bonefish::wamp_welcome_details const&)
{
    throw std::logic_error("no msgpack object packer serializer defined");
}

template<>
void operator<< (object::with_zone& details,
        bonefish::wamp_welcome_details const& welcome_details)
{
    object::with_zone roles(details.zone);
    roles.type = type::MAP;
    roles.via.map.size = welcome_details.get_roles().size();
    roles.via.map.ptr = static_cast<object_kv*>(
            roles.zone.allocate_align(sizeof(object_kv) * roles.via.map.size));

    unsigned index = 0;
    for (const auto& role : welcome_details.get_roles()) {
        object::with_zone features(roles.zone);
        features.type = type::MAP;

        const auto& attributes = role.get_features().get_attributes();
        if (attributes.size() == 0) {
            features.via.map.size = 0;
            features.via.map.ptr = nullptr;
        } else {
            features.via.map.size = 1;
            features.via.map.ptr = static_cast<object_kv*>(
                features.zone.allocate_align(sizeof(object_kv) * features.via.map.size));
            features.via.map.ptr[0].key = object(std::string("features"), features.zone);
            features.via.map.ptr[0].val = object(attributes, features.zone);
        }

        const std::string role_name(role_type_to_string(role.get_type()));
        roles.via.map.ptr[index].key = object(role_name, roles.zone);
        roles.via.map.ptr[index].val = *(static_cast<object*>(&features));
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

namespace bonefish {

msgpack::object wamp_welcome_details::marshal(msgpack::zone& zone)
{
    return msgpack::object(*this, zone);
}

void wamp_welcome_details::unmarshal(const msgpack::object& object)
{
    throw std::logic_error("unmarshal not implemented");
}

} // namespace bonefish
