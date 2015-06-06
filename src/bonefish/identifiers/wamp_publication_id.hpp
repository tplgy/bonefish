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

#ifndef BONEFISH_IDENTIFIERS_WAMP_PUBLICATION_ID_HPP
#define BONEFISH_IDENTIFIERS_WAMP_PUBLICATION_ID_HPP

#include <bonefish/identifiers/wamp_random_id.hpp>

#include <functional>
#include <ostream>

namespace bonefish {

class wamp_publication_id : public wamp_random_id
{
public:
    wamp_publication_id();
    explicit wamp_publication_id(uint64_t id);
};

inline wamp_publication_id::wamp_publication_id()
    : wamp_random_id()
{
}

inline wamp_publication_id::wamp_publication_id(uint64_t id)
    : wamp_random_id(id)
{
}

inline std::ostream& operator<<(std::ostream& os, const wamp_publication_id& pid)
{
    if (pid.is_valid()) {
        os << pid.id();
    } else {
        os << "<<invalid>>";
    }

    return os;
}

} // namespace bonefish

namespace std {

template<> struct hash<bonefish::wamp_publication_id>
{
    size_t operator()(const bonefish::wamp_publication_id& id) const
    {
        return hash<uint64_t>()(id.id());
    }
};

} // namespace std

#endif // BONEFISH_IDENTIFIERS_WAMP_PUBLICATION_ID_HPP
