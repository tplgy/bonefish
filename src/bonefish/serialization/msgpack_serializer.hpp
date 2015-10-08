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

#ifndef BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP
#define BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP

#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/serialization/wamp_serializer_type.hpp>

#include <cstddef>

namespace bonefish {

class wamp_message;

class msgpack_serializer : public wamp_serializer
{
public:
    msgpack_serializer();
    virtual ~msgpack_serializer() override;

    virtual wamp_serializer_type get_type() const override;
    virtual wamp_message* deserialize(const char* buffer, size_t length) const override;
    virtual expandable_buffer serialize(const wamp_message& message) const override;
};

inline msgpack_serializer::msgpack_serializer()
{
}

inline msgpack_serializer::~msgpack_serializer()
{
}

inline wamp_serializer_type msgpack_serializer::get_type() const
{
    return wamp_serializer_type::MSGPACK;
}

} // namespace bonefish

#endif // BONEFISH_SERIALIZATION_MSGPACK_SERIALIZER_HPP
