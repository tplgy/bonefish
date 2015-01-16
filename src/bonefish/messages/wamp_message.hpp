#ifndef BONEFISH_MESSAGES_WAMP_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_MESSAGE_HPP

#include <bonefish/messages/message_type.hpp>

namespace bonefish {

class wamp_message
{
public:
    wamp_message();
    virtual ~wamp_message();
    virtual message_type get_type() const = 0;
};

inline wamp_message::wamp_message()
{
}

inline wamp_message::~wamp_message()
{
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_MESSAGE_HPP
