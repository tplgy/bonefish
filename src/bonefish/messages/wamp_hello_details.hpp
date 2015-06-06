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

#ifndef BONEFISH_MESSAGES_WAMP_HELLO_DETAILS_HPP
#define BONEFISH_MESSAGES_WAMP_HELLO_DETAILS_HPP

#include <bonefish/roles/wamp_role.hpp>

#include <cstddef>
#include <iostream>
#include <msgpack.hpp>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace bonefish {

class wamp_hello_details
{
public:
    wamp_hello_details();
    virtual ~wamp_hello_details();

    msgpack::object marshal(msgpack::zone* zone=nullptr) const;
    void unmarshal(const msgpack::object& details);

    const std::unordered_set<wamp_role>& get_roles() const;
    void add_role(const wamp_role& role);

private:
    msgpack::zone m_zone;
    std::unordered_set<wamp_role> m_roles;
};

inline wamp_hello_details::wamp_hello_details()
    : m_zone()
{
}

inline wamp_hello_details::~wamp_hello_details()
{
}

inline msgpack::object wamp_hello_details::marshal(msgpack::zone*) const
{
    throw std::logic_error("marshal not implemented");
}

inline void wamp_hello_details::unmarshal(const msgpack::object& object)
{
    std::unordered_map<std::string, msgpack::object> details;
    object.convert(&details);

    auto details_itr = details.find("roles");
    if (details_itr == details.end()) {
        return;
    }

    // TODO: Add support for features as per advanced profile support
    std::unordered_map<std::string, msgpack::object /* features */> roles;
    details_itr->second.convert(roles);

    for (const auto& role_itr : roles) {
        add_role(wamp_role(role_type_from_string(role_itr.first)));
    }

    // TODO: Add support for agent as per advanced profile support
}

inline const std::unordered_set<wamp_role>& wamp_hello_details::get_roles() const
{
    return m_roles;
}

inline void wamp_hello_details::add_role(const wamp_role& role)
{
    m_roles.insert(role);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_HELLO_DETAILS_HPP
