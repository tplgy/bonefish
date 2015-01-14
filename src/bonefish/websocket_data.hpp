#ifndef BONEFISH_WEBSOCKET_DATA_HPP
#define BONEFISH_WEBSOCKET_DATA_HPP

#include <bonefish/identifier/session_id.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

namespace bonefish {

class websocket_data : public websocketpp::config::asio
{
public:
    websocket_data();
    ~websocket_data();

    void set_session_id(session_id id);
    session_id get_session_id();

private:
    session_id m_session_id;
};

inline websocket_data::websocket_data()
    : m_session_id()
{
}

inline websocket_data::~websocket_data()
{
}

inline void websocket_data::set_session_id(session_id id)
{
    m_session_id = id;
}

inline session_id websocket_data::get_session_id()
{
    return m_session_id;
}

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_DATA_HPP
