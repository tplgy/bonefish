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

#include <bonefish/messages/wamp_hello_details.hpp>
#include <bonefish/trace/trace.hpp>

#include <stdexcept>

namespace bonefish {

msgpack::object wamp_hello_details::marshal(msgpack::zone& zone) const
{
    throw std::logic_error("marshal not implemented");
}

void wamp_hello_details::unmarshal(const msgpack::object& object)
{
    std::unordered_map<std::string, msgpack::object> details;
    object.convert(&details);

    auto details_itr = details.find("roles");
    if (details_itr == details.end()) {
        return;
    }

    std::unordered_map<std::string, msgpack::object> roles;
    details_itr->second.convert(roles);

    for (const auto& roles_itr : roles) {
        std::unordered_map<std::string, msgpack::object> properties;
        roles_itr.second.convert(properties);

        wamp_role_features features;
        auto properties_itr = properties.find("features");
        if (properties_itr != properties.end()) {
            std::unordered_map<std::string, bool> attributes;
            properties_itr->second.convert(attributes);
            features.set_attributes(std::move(attributes));
        }

        wamp_role_type role_type = role_type_from_string(roles_itr.first);
        wamp_role role(role_type);

        role.set_features(std::move(features));
        add_role(std::move(role));
    }
}

} // namespace bonefish
