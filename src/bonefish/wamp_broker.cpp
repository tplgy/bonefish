#include <bonefish/wamp_broker.hpp>
#include <bonefish/wamp_session.hpp>
#include <iostream>

namespace bonefish {

wamp_broker::wamp_broker()
    : m_sessions()
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
    return m_sessions.erase(session_id) == 1;
}

} // namespace bonefish
