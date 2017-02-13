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

#ifndef BONEFISH_IDENTIFIERS_WAMP_SEQUENTIAL_ID_HPP
#define BONEFISH_IDENTIFIERS_WAMP_SEQUENTIAL_ID_HPP

#include <cstdint>
#include <functional>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace bonefish {

class wamp_sequential_id
{
public:
    static const uint64_t MIN = 0;
    static const uint64_t MAX = 1ULL << 53;
#if defined(_MSC_VER)
    static const uint64_t INVALID = UINT64_MAX;
#else
    static const uint64_t INVALID = std::numeric_limits<uint64_t>::max();
#endif

public:
    wamp_sequential_id();
    explicit wamp_sequential_id(uint64_t id);

    bool is_valid() const;
    uint64_t id() const;

    bool operator==(const wamp_sequential_id& other) const;

private:
    uint64_t m_id;
};

inline wamp_sequential_id::wamp_sequential_id()
    : m_id(INVALID)
{
}

inline wamp_sequential_id::wamp_sequential_id(uint64_t id)
    : m_id(id)
{
    if (m_id < MIN || m_id > MAX) {
        std::stringstream ss;
        ss << "sequential id " << m_id << " is out of range";
        throw std::invalid_argument(ss.str());
    }
}

inline bool wamp_sequential_id::is_valid() const
{
    return m_id != INVALID;
}

inline uint64_t wamp_sequential_id::id() const
{
    return m_id;
}

inline bool wamp_sequential_id::operator==(const wamp_sequential_id& other) const
{
    return m_id == other.m_id;
}

inline std::ostream& operator<<(std::ostream& os, const wamp_sequential_id& sid)
{
    if (sid.is_valid()) {
        os << sid.id();
    } else {
        os << "<<invalid>>";
    }

    return os;
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_sequential_id>
{
    size_t operator()(const bonefish::wamp_sequential_id& id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIERS_WAMP_SEQUENTIAL_ID_HPP
