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

#ifndef BONEFISH_BROKER_WAMP_BROKER_SUBSCRIPTIONS_HPP
#define BONEFISH_BROKER_WAMP_BROKER_SUBSCRIPTIONS_HPP

#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/session/wamp_session.hpp>

#include <unordered_set>

namespace bonefish {

class wamp_broker_subscriptions
{
public:
    using subscriptions = std::unordered_map<wamp_subscription_id, std::shared_ptr<wamp_session>>;
public:
    wamp_broker_subscriptions();
    ~wamp_broker_subscriptions();

    bool add_subscription(
            const wamp_subscription_id& subscription_id,
            const std::shared_ptr<wamp_session>& session);
    bool remove_subscription(const wamp_subscription_id& subscription_id);
    void remove_subscriptions(const std::shared_ptr<wamp_session>& session);
    const subscriptions& get_subscriptions() const;

private:
    subscriptions m_subscriptions;
};

inline wamp_broker_subscriptions::wamp_broker_subscriptions()
    : m_subscriptions()
{
}

inline wamp_broker_subscriptions::~wamp_broker_subscriptions()
{
}

inline bool wamp_broker_subscriptions::add_subscription(
        const wamp_subscription_id& subscription_id,
        const std::shared_ptr<wamp_session>& session)
{
    auto result = m_subscriptions.emplace(subscription_id, session);
    return result.second;
}

inline bool wamp_broker_subscriptions::remove_subscription(
        const wamp_subscription_id& subscription_id)
{
    return m_subscriptions.erase(subscription_id) != 0;
}

inline void wamp_broker_subscriptions::remove_subscriptions(
        const std::shared_ptr<wamp_session>& session)
{
    auto itr = m_subscriptions.begin();
    auto end = m_subscriptions.end();
    while (itr != end) {
        if (itr->second == session) {
            itr = m_subscriptions.erase(itr);
            continue;
        }
        ++itr;
    }
}

inline const wamp_broker_subscriptions::subscriptions& wamp_broker_subscriptions::get_subscriptions() const
{
    return m_subscriptions;
}

} // namespace bonefish

#endif // BONEFISH_BROKER_WAMP_BROKER_SUBSCRIPTIONS_HPP
