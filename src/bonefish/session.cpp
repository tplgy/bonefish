#include <bonefish/session.hpp>
#include <bonefish/session_transport.hpp>

namespace bonefish {

session::session()
    : m_session_id()
    , m_transport()
{
}

session::session(const session_id& id,
        std::unique_ptr<session_transport> transport)
    : m_session_id(id)
    , m_transport(std::move(transport))
{
}

session::~session()
{
}

const session_id& session::get_session_id() const
{
    return m_session_id;
}

} // namespace bonefish
