#include <bonefish/router.hpp>

#include <iostream>

namespace bonefish {

router::router()
    : m_io_service()
    , m_work(new boost::asio::io_service::work(m_io_service))
    , m_sessions()
{
}

router::~router()
{
}

bool router::has_session(const session_id& id)
{
    return m_sessions.find(id) != m_sessions.end();
}

bool router::attach_session(std::shared_ptr<session>&& s)
{
    std::cout << "attach session: " << s->get_session_id() << std::endl;
    auto result = m_sessions.insert(
            std::make_pair(s->get_session_id(), std::move(s)));
    return result.second;
}

bool router::detach_session(const session_id& id)
{
    std::cout << "detach session:" << id << std::endl;
    return m_sessions.erase(id) == 1;
}

void router::run()
{
    m_io_service.run();
}

void router::shutdown()
{
    m_work.reset();
}

} // namespace bonefish
