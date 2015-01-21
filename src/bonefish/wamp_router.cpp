#include <bonefish/wamp_router.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/wamp_session.hpp>
#include <iostream>
#include <stdexcept>

namespace bonefish {

wamp_router::wamp_router(const std::string& realm)
    : m_realm(realm)
    , m_sessions()
{
}

wamp_router::~wamp_router()
{
}

const std::string& wamp_router::get_realm() const
{
    return m_realm;
}

bool wamp_router::has_session(const session_id& id)
{
    return m_sessions.find(id) != m_sessions.end();
}

bool wamp_router::attach_session(std::shared_ptr<wamp_session>&& session)
{
    std::cout << "attach session: " << session->get_session_id() << std::endl;
    auto result = m_sessions.insert(
            std::make_pair(session->get_session_id(), std::move(session)));
    return result.second;
}

bool wamp_router::detach_session(const session_id& id)
{
    std::cout << "detach session:" << id << std::endl;
    return m_sessions.erase(id) == 1;
}

void wamp_router::process_hello_message(const session_id& id,
        const wamp_hello_message* hello_message)
{
    auto session_itr = m_sessions.find(id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("session does not exist"));
    }

    session_itr->second->process_hello_message(hello_message);
}

} // namespace bonefish
