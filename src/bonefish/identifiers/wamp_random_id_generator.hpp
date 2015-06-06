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

#ifndef BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_GENERATOR_HPP
#define BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_GENERATOR_HPP

#include <cstdint>
#include <random>
#include <stdexcept>

namespace bonefish {

class wamp_random_id_generator
{
public:
    explicit wamp_random_id_generator(uint64_t range_start, uint64_t range_end,
            uint64_t seed = std::mt19937_64::default_seed);
    wamp_random_id_generator(wamp_random_id_generator const&) = delete;
    wamp_random_id_generator& operator=(wamp_random_id_generator const&) = delete;
    uint64_t generate();

private:
    std::mt19937_64 m_generator;
    std::uniform_int_distribution<uint64_t> m_distribution;
};

inline wamp_random_id_generator::wamp_random_id_generator(
    uint64_t range_start, uint64_t range_end, uint64_t seed)
    : m_generator(seed)
    , m_distribution(range_start, range_end)
{
    if (range_start >= range_end) {
        throw std::invalid_argument("range is invalid");
    }
}

inline uint64_t wamp_random_id_generator::generate()
{
    return m_distribution(m_generator);
}

} // namespace bonefish

#endif // BONEFISH_IDENTIFIERS_WAMP_RANDOM_ID_GENERATOR_HPP
