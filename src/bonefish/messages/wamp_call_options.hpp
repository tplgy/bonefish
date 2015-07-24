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

#ifndef BONEFISH_MESSAGES_WAMP_CALL_OPTIONS_HPP
#define BONEFISH_MESSAGES_WAMP_CALL_OPTIONS_HPP

#include <msgpack.hpp>
#include <string>
#include <unordered_set>

namespace bonefish {

class wamp_call_options
{
public:
    wamp_call_options();
    virtual ~wamp_call_options();

    msgpack::object marshal(msgpack::zone* zone=nullptr) const;
    void unmarshal(const msgpack::object& options);

    template <typename T>
    T get_option(const std::string& name) const;

    template <typename T>
    T get_option_or(const std::string& name, T default_value) const;

private:
    std::unordered_map<std::string, msgpack::object> m_options;
};

inline wamp_call_options::wamp_call_options()
    : m_options()
{
}

inline wamp_call_options::~wamp_call_options()
{
}

template <typename T>
T wamp_call_options::get_option(const std::string& name) const
{
    const auto option_itr = m_options.find(name);
    if (option_itr == m_options.end()) {
        throw std::invalid_argument("invalid option requested");
    }

    return option_itr->second.as<T>();
}

template <typename T>
T wamp_call_options::get_option_or(const std::string& name, T default_value) const
{
    const auto option_itr = m_options.find(name);
    if (option_itr == m_options.end()) {
        return default_value;
    }

    return option_itr->second.as<T>();
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_CALL_OPTIONS_HPP
