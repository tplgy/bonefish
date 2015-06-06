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

#include <bonefish/messages/wamp_message_defaults.hpp>

#include <map>
#include <msgpack.hpp>

namespace bonefish {

namespace {
msgpack::zone zone;
const std::map<int,int> EMPTY_MAP;
const msgpack::object MSGPACK_EMPTY_MAP(EMPTY_MAP, &zone);
} // namespace

const msgpack::object& msgpack_empty_map()
{
    return MSGPACK_EMPTY_MAP;
}


} // namespace bonefish
