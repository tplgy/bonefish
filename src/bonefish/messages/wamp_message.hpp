#ifndef BONEFISH_MESSAGES_WAMP_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_MESSAGE_HPP

#include <bonefish/messages/wamp_message_type.hpp>

#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_message
{
public:
    wamp_message();
    virtual ~wamp_message();

    wamp_message(const wamp_message& other) = delete;
    wamp_message& operator=(wamp_message const&) = delete;

    virtual wamp_message_type get_type() const = 0;
    virtual std::vector<msgpack::object> marshal() const = 0;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) = 0;
};

inline wamp_message::wamp_message()
{
}

inline wamp_message::~wamp_message()
{
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_MESSAGE_HPP
