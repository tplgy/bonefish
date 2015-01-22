#ifndef BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/wamp_uri.hpp>

namespace bonefish {

//
// [REGISTER, Request|id, Options|dict, Procedure|uri]
//
class wamp_register_message : public wamp_message
{
public:
    wamp_register_message();
    virtual ~wamp_register_message() override;

    virtual wamp_message_type get_type() const override;

    const wamp_request_id& get_request_id() const;
    const wamp_uri& get_procedure() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_procedure(const wamp_uri& procedure);

private:
    wamp_request_id m_request_id;
    wamp_uri m_procedure;
};

inline wamp_register_message::wamp_register_message()
    : m_request_id()
    , m_procedure()
{
}

inline wamp_register_message::~wamp_register_message()
{
}

inline wamp_message_type wamp_register_message::get_type() const
{
    return wamp_message_type::REGISTER;
}

inline const wamp_request_id& wamp_register_message::get_request_id() const
{
    return m_request_id;
}

inline const wamp_uri& wamp_register_message::get_procedure() const
{
    return m_procedure;
}

inline void wamp_register_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = request_id;
}

inline void wamp_register_message::set_procedure(const wamp_uri& procedure)
{
    m_procedure = procedure;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP
