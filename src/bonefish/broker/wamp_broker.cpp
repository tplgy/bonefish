#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/broker/wamp_broker_subscription.hpp>
#include <bonefish/broker/wamp_broker_topic.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_event_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_published_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_subscribed_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_unsubscribed_message.hpp>
#include <bonefish/wamp_session.hpp>
#include <iostream>

namespace bonefish {

wamp_broker::wamp_broker()
    : m_publication_id_generator()
    , m_subscription_id_generator()
    , m_sessions()
    , m_session_subscriptions()
    , m_topic_subscriptions()
    , m_subscription_topics()
{
}

wamp_broker::~wamp_broker()
{
}

void wamp_broker::attach_session(const std::shared_ptr<wamp_session>& session)
{
    std::cerr << "attach session: " << session->get_session_id() << std::endl;
    auto result = m_sessions.insert(
            std::make_pair(session->get_session_id(), std::move(session)));
    if (!result.second) {
        throw(std::logic_error("broker session already registered"));
    }
}

void wamp_broker::detach_session(const wamp_session_id& session_id)
{
    std::cerr << "detach session:" << session_id << std::endl;

    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("broker session does not exist"));
    }

    auto session_subscriptions_itr = m_session_subscriptions.find(session_id);
    if (session_subscriptions_itr == m_session_subscriptions.end()) {
        return;
    }

    for (const auto& subscription_id : session_subscriptions_itr->second) {
        auto subscription_topics_itr = m_subscription_topics.find(subscription_id);
        if (subscription_topics_itr == m_subscription_topics.end()) {
            throw(std::logic_error("broker subscription topics out of sync"));
        }

        wamp_uri topic = subscription_topics_itr->second->get_topic();
        subscription_topics_itr->second->remove_session(session_itr->second);
        if (subscription_topics_itr->second->get_sessions().size() == 0) {
            m_subscription_topics.erase(subscription_id);
        }

        auto topic_subscriptions_itr = m_topic_subscriptions.find(topic);
        if (topic_subscriptions_itr == m_topic_subscriptions.end()) {
            throw(std::logic_error("broker topic subscriptions out of sync"));
        }

        topic_subscriptions_itr->second->remove_session(session_itr->second);
        if (topic_subscriptions_itr->second->get_sessions().size() == 0) {
            m_topic_subscriptions.erase(topic);
        }
    }

    m_session_subscriptions.erase(session_subscriptions_itr);
    m_sessions.erase(session_id);
}

void wamp_broker::process_publish_message(const wamp_session_id& session_id,
        const wamp_publish_message* publish_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
        error_message->set_request_type(publish_message->get_type());
        error_message->set_request_id(publish_message->get_request_id());
        error_message->set_error("wamp.error.no_such_session");
        session_itr->second->get_transport()->send_message(error_message.get());
        return;
    }

    const wamp_uri& topic = publish_message->get_topic();
    const wamp_publication_id publication_id = m_publication_id_generator.generate();

    auto topic_subscriptions_itr = m_topic_subscriptions.find(topic);
    if (topic_subscriptions_itr != m_topic_subscriptions.end()) {
        std::unique_ptr<wamp_event_message> event_message(new wamp_event_message);
        event_message->set_subscription_id(topic_subscriptions_itr->second->get_subscription_id());
        event_message->set_publication_id(publication_id);

        for (const auto& session : topic_subscriptions_itr->second->get_sessions()) {
            // TODO: Improve performacne here by offering a transport api that
            //       takes in a pre-serialized buffer. That way we can serialize
            //       the message once and then send it to all of the subscribers.
            session->get_transport()->send_message(event_message.get());
        }
    }

    std::unique_ptr<wamp_published_message> published_message(new wamp_published_message);
    published_message->set_request_id(publish_message->get_request_id());
    published_message->set_publication_id(publication_id);
    session_itr->second->get_transport()->send_message(published_message.get());
}

void wamp_broker::process_subscribe_message(const wamp_session_id& session_id,
        const wamp_subscribe_message* subscribe_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
        error_message->set_request_type(subscribe_message->get_type());
        error_message->set_request_id(subscribe_message->get_request_id());
        error_message->set_error("wamp.error.no_such_session");
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

void wamp_broker::process_unsubscribe_message(const wamp_session_id& session_id,
        const wamp_unsubscribe_message* unsubscribe_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
        error_message->set_request_type(unsubscribe_message->get_type());
        error_message->set_request_id(unsubscribe_message->get_request_id());
        error_message->set_error("wamp.error.no_such_session");
        session_itr->second->get_transport()->send_message(error_message.get());
        return;
    }

    auto session_subscriptions_itr = m_session_subscriptions.find(session_id);
    if (session_subscriptions_itr == m_session_subscriptions.end()) {
        std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
        error_message->set_request_type(unsubscribe_message->get_type());
        error_message->set_request_id(unsubscribe_message->get_request_id());
        error_message->set_error("wamp.error.no_subscriptions_for_session");
        session_itr->second->get_transport()->send_message(error_message.get());
        return;
    }

    const wamp_subscription_id& subscription_id = unsubscribe_message->get_subscription_id();
    if (session_subscriptions_itr->second.erase(subscription_id)) {
        std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
        error_message->set_request_type(unsubscribe_message->get_type());
        error_message->set_request_id(unsubscribe_message->get_request_id());
        error_message->set_error("wamp.error.no_such_subscription");
        session_itr->second->get_transport()->send_message(error_message.get());
        return;
    }

    auto subscription_topics_itr = m_subscription_topics.find(subscription_id);
    if (subscription_topics_itr == m_subscription_topics.end()) {
        throw(std::logic_error("broker subscription topics out of sync"));
    }

    wamp_uri topic = subscription_topics_itr->second->get_topic();
    subscription_topics_itr->second->remove_session(session_itr->second);
    if (subscription_topics_itr->second->get_sessions().size() == 0) {
        m_subscription_topics.erase(subscription_id);
    }

    auto topic_subscriptions_itr = m_topic_subscriptions.find(topic);
    if (topic_subscriptions_itr == m_topic_subscriptions.end()) {
        throw(std::logic_error("broker topic subscriptions out of sync"));
    }

    topic_subscriptions_itr->second->remove_session(session_itr->second);
    if (topic_subscriptions_itr->second->get_sessions().size() == 0) {
        m_topic_subscriptions.erase(topic);
    }
}

} // namespace bonefish
