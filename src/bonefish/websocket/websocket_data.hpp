#ifndef BONEFISH_WEBSOCKET_WEBSOCKET_DATA_HPP
#define BONEFISH_WEBSOCKET_WEBSOCKET_DATA_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>

#include <websocketpp/config/asio_no_tls.hpp>

namespace bonefish {

class websocket_data : public websocketpp::config::asio
{
public:
    websocket_data();
    ~websocket_data();

    bool has_realm() const;
    void set_realm(const std::string& realm);
    const std::string& get_realm() const;

    bool has_session_id() const;
    void set_session_id(const wamp_session_id& id);
    const wamp_session_id& get_session_id() const;

    void clear_data();

private:
    std::string m_realm;
    wamp_session_id m_session_id;
};

inline websocket_data::websocket_data()
    : m_realm()
    , m_session_id()
{
}

inline websocket_data::~websocket_data()
{
}

inline bool websocket_data::has_realm() const
{
    return !m_realm.empty();
}

inline void websocket_data::set_realm(const std::string& realm)
{
    m_realm = realm;
}

inline const std::string& websocket_data::get_realm() const
{
    return m_realm;
}

inline bool websocket_data::has_session_id() const
{
    return m_session_id.is_valid();
}

inline void websocket_data::set_session_id(const wamp_session_id& id)
{
    m_session_id = id;
}

inline const wamp_session_id& websocket_data::get_session_id() const
{
    return m_session_id;
}

inline void websocket_data::clear_data()
{
    m_session_id = wamp_session_id();
    m_realm = std::string();
}

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_WEBSOCKET_DATA_HPP
