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

#ifndef BONEFISH_SERIALIZATION_WAMP_SERIALIZER_TYPE_HPP
#define BONEFISH_SERIALIZATION_WAMP_SERIALIZER_TYPE_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>

namespace bonefish {

enum class wamp_serializer_type : unsigned
{
    MSGPACK,
    JSON
};

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_serializer_type>
{
    size_t operator()(const bonefish::wamp_serializer_type& type) const
    {
        return hash<std::underlying_type<bonefish::wamp_serializer_type>::type>()(
                static_cast<std::underlying_type<bonefish::wamp_serializer_type>::type>(type));
    }
};

} // namespace std

#endif // BONEFISH_SERIALIZATION_WAMP_SERIALIZER_TYPE_HPP
