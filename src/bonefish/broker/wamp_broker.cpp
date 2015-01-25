#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/broker/wamp_broker_subscription.hpp>
#include <bonefish/broker/wamp_broker_topic.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_subscribed_message.hpp>
#include <bonefish/wamp_session.hpp>
#include <iostream>

namespace bonefish {

wamp_broker::wamp_broker()
    : m_subscription_id_generator()
    , m_sessions()
    , m_session_subscriptions()
    , m_topic_subscriptions()
    , m_subscription_topics()
{
}

wamp_broker::~wamp_broker()
{
}

bool wamp_broker::attach_session(const std::shared_ptr<wamp_session>& session)
{
    std::cerr << "attach session: " << session->get_session_id() << std::endl;
    auto result = m_sessions.insert(
            std::make_pair(session->get_session_id(), std::move(session)));
    return result.second;
}

bool wamp_broker::detach_session(const wamp_session_id& session_id)
{
    std::cerr << "detach session:" << session_id << std::endl;

    //
    // TODO: Cleanup topics and subscriptions
    //

    return m_sessions.erase(session_id) == 1;
}

void wamp_broker::process_subscribe_message(const wamp_session_id& session_id,
        const wamp_subscribe_message* subscribe_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
        error_message->set_request_type(subscribe_message->get_type());
        error_message->set_request_id(subscribe_message->get_request_id());
        error_message->set_error("wamp.error.no_such_subscription");
        session_itr->second->get_transport()->send_message(error_message.get());
        return;
    }

    wamp_subscription_id subscription_id;
    auto& session = session_itr->second;
    {
        auto result = m_topic_subscriptions.insert(
                std::make_pair(subscribe_message->get_topic(), nullptr));
        if (result.second) {
            subscription_id = m_subscription_id_generator.generate();
            result.first->second.reset(new wamp_broker_subscription(subscription_id));
            result.first->second->add_session(session);
        } else {
            subscription_id = result.first->second->get_subscription_id();
            result.first->second->add_session(session);
        }
    }

    {
        auto result = m_subscription_topics.insert(std::make_pair(subscription_id, nullptr));
        if (result.second) {
            result.first->second.reset(new wamp_broker_topic(subscribe_message->get_topic()));
            result.first->second->add_session(session);
        } else {
            result.first->second->add_session(session);
        }
    }

    m_session_subscriptions[session_id].insert(subscription_id);

    std::unique_ptr<wamp_subscribed_message> subscribed_message(new wamp_subscribed_message);
    subscribed_message->set_request_id(subscribe_message->get_request_id());
    subscribed_message->set_subscription_id(subscription_id);
    session->get_transport()->send_message(subscribed_message.get());
}

} // namespace bonefish
