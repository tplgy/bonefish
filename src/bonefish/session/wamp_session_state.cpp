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

#include <bonefish/session/wamp_session_state.hpp>

#include <stdexcept>

namespace bonefish {

const char* session_state_to_string(const wamp_session_state& state)
{
    const char* str = nullptr;
    switch (state)
    {
        case wamp_session_state::NONE:
            str = "none";
            break;
        case wamp_session_state::OPEN:
            str = "open";
            break;
        case wamp_session_state::CLOSING:
            str = "closing";
            break;
        case wamp_session_state::CLOSED:
            str = "closed";
            break;
        default:
            throw std::invalid_argument("unknown session state");
            break;
    }

    return str;
}

} // namespace bonefish
