#ifndef BONEFISH_MESSAGES_WAMP_ERROR_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_ERROR_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>

namespace bonefish {

//
// [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri]
// [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list]
// [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list, ArgumentsKw|dict]
//
class wamp_error_message : public wamp_message
{
public:
    wamp_error_message();
    virtual ~wamp_error_message() override;

    virtual wamp_message_type get_type() const override;

    wamp_message_type get_request_type() const;
    const wamp_request_id& get_request_id() const;
    const wamp_uri& get_error() const;

    void set_request_type(const wamp_message_type& request_type);
    void set_request_id(const wamp_request_id& request_id);
    void set_error(const wamp_uri& error);

private:
    wamp_message_type m_request_type;
    wamp_request_id m_request_id;
    wamp_uri m_error;
};

inline wamp_error_message::wamp_error_message()
    : m_request_type()
    , m_request_id()
    , m_error()
{
}

inline wamp_error_message::~wamp_error_message()
{
}

inline wamp_message_type wamp_error_message::get_type() const
{
    return wamp_message_type::ERROR;
}

inline wamp_message_type wamp_error_message::get_request_type() const
{
    return m_request_type;
}

inline const wamp_request_id& wamp_error_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_uri& wamp_error_message::get_error() const
{
    return m_error;
}

inline void wamp_error_message::set_request_type(const wamp_message_type& request_type)
{
    m_request_type = request_type;
}

inline void wamp_error_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_error_message::set_error(const wamp_uri& error)
{
    m_error = error;
}

} // namespace bonefish

#endif // BONEFISH_WAMP_MESSAGES_ERROR_MESSAGE_HPP
