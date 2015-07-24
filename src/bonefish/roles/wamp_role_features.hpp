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

#ifndef BONEFISH_ROLES_WAMP_ROLE_FEATURES_HPP
#define BONEFISH_ROLES_WAMP_ROLE_FEATURES_HPP

#include <string>
#include <unordered_map>

namespace bonefish {

class wamp_role_features
{
public:
    wamp_role_features();
    ~wamp_role_features();

    wamp_role_features(wamp_role_features&& other);
    wamp_role_features& operator=(wamp_role_features&& other);
    wamp_role_features(const wamp_role_features& other);
    wamp_role_features& operator=(const wamp_role_features& other);
    bool operator==(const wamp_role_features& features) const;

    void set_attribute(const std::string& attribute, bool supported);
    void set_attributes(std::unordered_map<std::string, bool>&& attributes);
    bool get_attribute(const std::string& attribute) const;
    const std::unordered_map<std::string, bool>& get_attributes() const;

private:
    std::unordered_map<std::string, bool> m_attributes;
};

inline wamp_role_features::wamp_role_features()
    : m_attributes()
{
}

inline wamp_role_features::~wamp_role_features()
{
}

inline wamp_role_features::wamp_role_features(wamp_role_features&& other)
    : m_attributes(std::move(other.m_attributes))
{
}

inline wamp_role_features& wamp_role_features::operator=(wamp_role_features&& other)
{
    if (this != &other) {
        m_attributes = std::move(other.m_attributes);
    }

    return *this;
}

inline wamp_role_features::wamp_role_features(const wamp_role_features& other)
    : m_attributes(other.m_attributes)
{
}

inline wamp_role_features& wamp_role_features::operator=(const wamp_role_features& other)
{
    if (this != &other) {
        m_attributes = other.m_attributes;
    }

    return *this;
}

inline bool wamp_role_features::operator==(const wamp_role_features& other) const
{
    if (this == &other) {
        return true;
    }

    return m_attributes == other.m_attributes;
}

inline void wamp_role_features::set_attributes(std::unordered_map<std::string, bool>&& attributes)
{
    m_attributes = std::move(attributes);
}

inline void wamp_role_features::set_attribute(const std::string& attribute, bool supported)
{
    m_attributes[attribute] = supported;
}

inline bool wamp_role_features::get_attribute(const std::string& attribute) const
{
    auto itr = m_attributes.find(attribute);
    return itr != m_attributes.end() ? itr->second : false;
}

inline const std::unordered_map<std::string, bool>& wamp_role_features::get_attributes() const
{
    return m_attributes;
}

} // namespace bonefish

#endif // BONEFISH_ROLES_WAMP_ROLE_FEATURES_HPP
