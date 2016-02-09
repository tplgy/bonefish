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

#ifndef BONEFISH_MESSAGES_WAMP_YIELD_OPTIONS_HPP
#define BONEFISH_MESSAGES_WAMP_YIELD_OPTIONS_HPP

#include <msgpack.hpp>
#include <string>
#include <map>

namespace bonefish {

class wamp_yield_options
{
public:
    wamp_yield_options();
    virtual ~wamp_yield_options();

    msgpack::object marshal(msgpack::zone& zone) const;
    void unmarshal(const msgpack::object& options);

    template <typename T>
    T get_option(const std::string& name) const;

    template <typename T>
    T get_option_or(const std::string& name, const T& default_value) const;

    template <typename T>
    void set_option(const std::string& name, const T& value);

private:
    msgpack::zone m_zone;
    std::map<std::string, msgpack::object> m_options;
};

inline wamp_yield_options::wamp_yield_options()
    : m_zone()
    , m_options()
{
}

inline wamp_yield_options::~wamp_yield_options()
{
}

template <typename T>
T wamp_yield_options::get_option(const std::string& name) const
{
    const auto option_itr = m_options.find(name);
    if (option_itr == m_options.end()) {
        throw std::invalid_argument("invalid option requested: " + name);
    }

    return option_itr->second.as<T>();
}

template <typename T>
T wamp_yield_options::get_option_or(const std::string& name, const T& default_value) const
{
    const auto option_itr = m_options.find(name);
    if (option_itr == m_options.end()) {
        return default_value;
    }

    return option_itr->second.as<T>();
}

template <typename T>
void wamp_yield_options::set_option(const std::string& name, const T& value)
{
    m_options[name] = msgpack::object(value, m_zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_YIELD_OPTIONS_HPP
