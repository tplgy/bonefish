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

#ifndef BONEFISH_SESSION_WAMP_SESSION_STATE_HPP
#define BONEFISH_SESSION_WAMP_SESSION_STATE_HPP

#include <cstdint>
#include <ostream>
#include <string>

namespace bonefish {

enum class wamp_session_state : uint8_t {
    NONE,
    OPEN,
    CLOSING,
    CLOSED
};

const char* session_state_to_string(const wamp_session_state& state);

inline std::ostream& operator<<(std::ostream& os, const wamp_session_state& state)
{
    os << bonefish::session_state_to_string(state);
    return os;
}

} // namespace bonefish

#endif // BONEFISH_SESSION_WAMP_SESSION_STATE_HPP
