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

#include <bonefish/messages/wamp_yield_options.hpp>

#include <stdexcept>

namespace bonefish {

msgpack::object wamp_yield_options::marshal(msgpack::zone& zone) const
{
    return msgpack::object(m_options, zone);
}

void wamp_yield_options::unmarshal(const msgpack::object& object)
{
    object.convert(m_options);
}

} // namespace bonefish
