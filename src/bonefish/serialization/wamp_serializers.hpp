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

#ifndef BONEFISH_SERIALIZATION_WAMP_SERIALIZERS_HPP
#define BONEFISH_SERIALIZATION_WAMP_SERIALIZERS_HPP

#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializer_type.hpp>

#include <memory>
#include <unordered_map>

namespace bonefish {

class wamp_serializers
{
public:
    wamp_serializers();
    ~wamp_serializers();

    bool add_serializer(const std::shared_ptr<wamp_serializer>& serializer);
    std::shared_ptr<wamp_serializer> get_serializer(const wamp_serializer_type& type);
    bool has_serializer(const wamp_serializer_type& type) const;
    void remove_serializer(const wamp_serializer_type& type);

private:
    std::unordered_map<wamp_serializer_type, std::shared_ptr<wamp_serializer>> m_serializers;
};

inline wamp_serializers::wamp_serializers()
    : m_serializers()
{
}

inline wamp_serializers::~wamp_serializers()
{
}

inline bool wamp_serializers::add_serializer(const std::shared_ptr<wamp_serializer>& serializer)
{
    auto result = m_serializers.insert(
            std::make_pair(serializer->get_type(), serializer));
    return result.second;
}

inline std::shared_ptr<wamp_serializer> wamp_serializers::get_serializer(const wamp_serializer_type& type)
{
    auto itr = m_serializers.find(type);
    return itr != m_serializers.end() ? itr->second : nullptr;
}

inline bool wamp_serializers::has_serializer(const wamp_serializer_type& type) const
{
    return m_serializers.find(type) != m_serializers.end();
}

inline void wamp_serializers::remove_serializer(const wamp_serializer_type& type)
{
    m_serializers.erase(type);
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_WAMP_SERIALIZERS_HPP
