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

#ifndef BONEFISH_MESSAGES_WAMP_WELCOME_DETAILS_HPP
#define BONEFISH_MESSAGES_WAMP_WELCOME_DETAILS_HPP

#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/roles/wamp_role_type.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <unordered_set>

namespace bonefish {

class wamp_welcome_details
{
public:
    wamp_welcome_details();
    virtual ~wamp_welcome_details();

    msgpack::object marshal(msgpack::zone* zone=nullptr);
    void unmarshal(const msgpack::object& details);

    const std::unordered_set<wamp_role>& get_roles() const;
    const wamp_role* get_role(wamp_role_type role_type) const;
    void add_role(wamp_role&& role);

private:
    msgpack::zone m_zone;
    std::unordered_set<wamp_role> m_roles;
};

inline wamp_welcome_details::wamp_welcome_details()
    : m_zone()
    , m_roles()
{
}

inline wamp_welcome_details::~wamp_welcome_details()
{
}

inline const std::unordered_set<wamp_role>& wamp_welcome_details::get_roles() const
{
    return m_roles;
}

inline const wamp_role* wamp_welcome_details::get_role(wamp_role_type role_type) const
{
    for (const auto& role : m_roles) {
        if (role.get_type() == role_type) {
            return &role;
        }
    }

    return nullptr;
}

inline void wamp_welcome_details::add_role(wamp_role&& role)
{
    m_roles.insert(std::move(role));
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_WELCOME_DETAILS_HPP
