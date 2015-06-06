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

#ifndef BONEFISH_IDENTIFIERS_WAMP_SEQUENTIAL_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_SEQUENTIAL_ID_GENERATOR_HPP

#include <cstdint>
#include <stdexcept>

namespace bonefish {

class wamp_sequential_id_generator
{
public:
    explicit wamp_sequential_id_generator(uint64_t range_start, uint64_t range_end);
    wamp_sequential_id_generator(wamp_sequential_id_generator const&) = delete;
    wamp_sequential_id_generator& operator=(wamp_sequential_id_generator const&) = delete;
    uint64_t generate();

private:
    const uint64_t m_range_start;
    const uint64_t m_range_end;
    uint64_t m_next_id;
};

inline wamp_sequential_id_generator::wamp_sequential_id_generator(
        uint64_t range_start, uint64_t range_end)
    : m_range_start(range_start)
    , m_range_end(range_end)
    , m_next_id(m_range_start)
{
    if (m_range_start >= m_range_end) {
        throw std::invalid_argument("range is invalid");
    }
}

inline uint64_t wamp_sequential_id_generator::generate()
{
    if (m_next_id < m_range_start ||
        m_next_id > m_range_end) {
        m_next_id = m_range_start;
    }

    return m_next_id++;
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_SEQUENTIAL_ID_GENERATOR_HPP
