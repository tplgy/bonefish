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

#ifndef BONEFISH_MESSAGES_WAMP_RESULT_DETAILS_HPP
#define BONEFISH_MESSAGES_WAMP_RESULT_DETAILS_HPP

#include <msgpack.hpp>
#include <string>
#include <map>

namespace bonefish {

class wamp_result_details
{
public:
    wamp_result_details();
    virtual ~wamp_result_details();

    msgpack::object marshal(msgpack::zone& zone) const;
    void unmarshal(const msgpack::object& details);

    template <typename T>
    T get_detail(const std::string& name) const;

    template <typename T>
    T get_detail_or(const std::string& name, const T& default_value) const;

    template <typename T>
    void set_detail(const std::string& name, const T& value);

private:
    msgpack::zone m_zone;
    std::map<std::string, msgpack::object> m_details;
};

inline wamp_result_details::wamp_result_details()
    : m_zone()
    , m_details()
{
}

inline wamp_result_details::~wamp_result_details()
{
}

template <typename T>
T wamp_result_details::get_detail(const std::string& name) const
{
    const auto detail_itr = m_details.find(name);
    if (detail_itr == m_details.end()) {
        throw std::invalid_argument("invalid detail requested: " + name);
    }

    return detail_itr->second.as<T>();
}

template <typename T>
T wamp_result_details::get_detail_or(const std::string& name, const T& default_value) const
{
    const auto detail_itr = m_details.find(name);
    if (detail_itr == m_details.end()) {
        return default_value;
    }

    return detail_itr->second.as<T>();
}

template <typename T>
void wamp_result_details::set_detail(const std::string& name, const T& value)
{
    m_details[name] = msgpack::object(value, m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_RESULT_DETAILS_HPP
