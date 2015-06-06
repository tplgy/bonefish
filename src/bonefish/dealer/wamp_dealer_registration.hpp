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

#ifndef BONEFISH_DEALER_WAMP_DEALER_SUBSCRIPTION_HPP
#define BONEFISH_DEALER_WAMP_DEALER_SUBSCRIPTION_HPP

#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/session/wamp_session.hpp>

#include <unordered_set>

namespace bonefish {

class wamp_dealer_registration
{
public:
    wamp_dealer_registration();
    wamp_dealer_registration(const std::shared_ptr<wamp_session>& session,
            const wamp_registration_id& registration_id);
    ~wamp_dealer_registration();

    void set_session(const std::shared_ptr<wamp_session>& session);
    void set_registration_id(const wamp_registration_id& registration_id);

    const wamp_registration_id& get_registration_id() const;
    const std::shared_ptr<wamp_session>& get_session() const;

private:
    std::shared_ptr<wamp_session> m_session;
    wamp_registration_id m_registration_id;
};

inline wamp_dealer_registration::wamp_dealer_registration()
    : m_session()
    , m_registration_id()
{
}

inline wamp_dealer_registration::wamp_dealer_registration(const std::shared_ptr<wamp_session>& session,
        const wamp_registration_id& registration_id)
    : m_session(session)
    , m_registration_id(registration_id)
{
}

inline wamp_dealer_registration::~wamp_dealer_registration()
{
}

inline void wamp_dealer_registration::set_session(const std::shared_ptr<wamp_session>& session)
{
    m_session = session;
}

inline void wamp_dealer_registration::set_registration_id(const wamp_registration_id& registration_id)
{
    m_registration_id = registration_id;
}

inline const std::shared_ptr<wamp_session>& wamp_dealer_registration::get_session() const
{
    return m_session;
}

inline const wamp_registration_id& wamp_dealer_registration::get_registration_id() const
{
    return m_registration_id;
}

} // namespace bonefish

#endif // BONEFISH_DEALER_WAMP_DEALER_SUBSCRIPTION_HPP
