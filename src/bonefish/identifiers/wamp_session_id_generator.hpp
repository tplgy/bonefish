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

#ifndef BONEFISH_IDENTIFIERS_WAMP_SESSION_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_SESSION_ID_GENERATOR_HPP

#include <bonefish/identifiers/wamp_random_id_generator.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>

#include <cstdint>

namespace bonefish {

class wamp_session_id_generator
{
public:
    wamp_session_id_generator();
    explicit wamp_session_id_generator(uint64_t seed);
    wamp_session_id_generator(wamp_session_id_generator const&) = delete;
    wamp_session_id_generator& operator=(wamp_session_id_generator const&) = delete;
    wamp_session_id generate();

private:
    wamp_random_id_generator m_random_id_generator;
};

inline wamp_session_id_generator::wamp_session_id_generator()
    : m_random_id_generator(wamp_session_id::MIN, wamp_session_id::MAX)
{
}

inline wamp_session_id_generator::wamp_session_id_generator(uint64_t seed)
    : m_random_id_generator(wamp_session_id::MIN, wamp_session_id::MAX, seed)
{
}

inline wamp_session_id wamp_session_id_generator::generate()
{
    return wamp_session_id(m_random_id_generator.generate());
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_SESSION_ID_GENERATOR_HPP
